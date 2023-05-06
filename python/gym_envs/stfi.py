from os import makedirs

import numpy as np
import pygame
import gym
import json
import zmq


class StFiEnv(gym.Env):
    metadata = {"render.modes": ["human", "ansi"]}

    # スクリーンのサイズ
    SCREEN_W = 640
    SCREEN_H = 480

    def __init__(self, option_list, port, nb_steps):
        super(StFiEnv, self).__init__()

        # 選択肢リスト
        self.optionList = option_list

        # 通信時のポート
        self.port = port

        # 通信用のソケットの定義
        self.ctx = None
        self.socket = None

        # 状態
        self.state = {}

        # 最終的な学習回数
        self.nb_steps = nb_steps
        # ステップ数のカウント
        self.total_step_count = 0
        self.step_count = None

        # 行動空間の定義(選択肢)
        self.action_space = gym.spaces.Discrete(len(option_list))
        # 観察空間の定義
        feature_count = 5
        low = np.full(feature_count * 2 + 2, 0)
        high = np.concatenate([np.array([1250, 4]), np.full(feature_count * 2, 65536)])
        self.observation_space = gym.spaces.Box(low=low, high=high)

        # 描画関連
        # PyGameの初期化
        pygame.init()
        # 描画用のスクリーン
        self.screen = pygame.display.set_mode((self.SCREEN_W, self.SCREEN_H))
        # 描画用のフォント
        # 情報表示用
        self.infoFont = pygame.font.SysFont("helvetica", 20)
        # ユニットの表示用
        self.unitFont = pygame.font.SysFont("helvetica", 20)

    def _communicate(self, action: np.array):
        if self._is_end_of_train():
            self.socket.send(b"reset")
        else:
            self.socket.send(b"%d" % action)
        # タイムアウト(10秒)を設定して待機
        if self.socket.poll(timeout=10000) == 0:
            raise TimeoutError
        msg = self.socket.recv()
        self.state = json.loads(msg)

    # 観察空間の定義通りの形式で状態を返す関数
    def _get_state(self):
        return np.concatenate((np.array([
            self.state["money"], self.state["profit_level"]
        ]),
                               np.array(self.state["friend_power"]),
                               np.array(self.state["enemy_power"])
        ))

    def reset(self):
        self.step_count = 0
        # 通信用のソケットを初期化する
        self.ctx = zmq.Context()
        self.socket = self.ctx.socket(zmq.PAIR)
        try:
            makedirs("/tmp/feeds")
        except FileExistsError:
            pass
        self.socket.bind("ipc:///tmp/feeds/" + str(self.port))

        # 初期状態を受信する
        # タイムアウト(10秒)を設定して待機
        if self.socket.poll(timeout=10000) == 0:
            raise TimeoutError
        msg = self.socket.recv()
        self.state = json.loads(msg)
        # AIのバージョンを送信する
        self.socket.send(b"0")
        return self._get_state()

    def _get_reward(self):
        # 行動に対する報酬を返す
        if self._is_done():
            if self.state["winner"] == 1:
                # 勝利した
                return 500
            else:
                # 敗北した
                return -100
        elif self.state["power_ratio"] >= 1:
            # 優勢
            return 2
        else:
            # 劣勢
            return -1

    # 完了したかの判定
    def _is_done(self):
        return self.state["winner"] != 0

    def _is_end_of_train(self):
        # もしnb_stepsが有効な値なら
        if self.nb_steps > 0:
            if self.total_step_count >= self.nb_steps:
                return True
        # その他の場合はFalseを返す
        return False

    # 後処理
    def _close_socket(self):
        self.socket.close()
        self.ctx.destroy()

    # ステップ関数の実装
    def step(self, action: np.array):
        self.total_step_count += 1
        self.step_count += 1
        self._communicate(action)
        state_after = self._get_state()
        reward = self._get_reward()
        is_done = self._is_done()
        info = {
            "is_done": is_done,
            "reward": reward,
        }
        if self._is_done() or self._is_end_of_train():
            self._close_socket()
        if self._is_end_of_train():
            self.total_step_count = 0
            # nb_stepsを無効な値に設定
            self.nb_steps = -1
        return state_after, reward, is_done, info

    # 描画関数の実装
    def render(self, mode="human"):
        if mode == "human":
            self.screen.fill((128, 128, 128))
            rendered_total_step_count = self.infoFont.render("Total steps: " + str(self.total_step_count),
                                                             True,
                                                             (255, 255, 255))
            rendered_step_count = self.infoFont.render("Steps: " + str(self.step_count),
                                                       True,
                                                       (255, 255, 255))
            self.screen.blit(rendered_total_step_count, (10, 10))
            self.screen.blit(rendered_step_count, (10, 40))
            for i in self.state["stage"]:
                # 中央の座標
                pos = (i["pos"] * self.SCREEN_W, self.SCREEN_H - (i["y"] + 1) * 45)
                pygame.draw.circle(self.screen,
                                   (255, 255, 255),
                                   pos,
                                   20)
                rendered_unit_font = self.unitFont.render(i["type"][:2],
                                                          True,
                                                          (0, 0, 0))
                self.screen.blit(rendered_unit_font, (pos[0] - 20, pos[1] - 10))
            pygame.display.update()
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    self.socket.send(b"abort")
                    raise KeyboardInterrupt
        elif mode == "ansi":
            return ""

    def close(self):
        # 通信用のソケットを初期化する
        self.ctx = zmq.Context()
        self.socket = self.ctx.socket(zmq.PAIR)
        try:
            makedirs("/tmp/feeds")
        except FileExistsError:
            pass
        self.socket.bind("ipc:///tmp/feeds/" + str(self.port))

        self.socket.send(b"finish")

        self._close_socket()
