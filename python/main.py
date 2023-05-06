# coding=utf-8

from sys import argv

import gym
import gym_envs

from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten
from keras.optimizers import Adam

from rl.agents.dqn import DQNAgent
from rl.policy import BoltzmannQPolicy
from rl.memory import SequentialMemory

import datetime


if __name__ == '__main__':
    if len(argv) >= 2:
        port = argv[1]
    else:
        port = 1

    ENV_NAME = 'StFi-v0'

    STEP_LIMIT = 50000

    env = gym.make(ENV_NAME,
                   option_list=["upgrade",
                                "eraser",
                                "pencil",
                                "triangle",
                                "mechanical_pencil",
                                "pencil_sharpener",
                                "compass"],
                   port=port,
                   nb_steps=STEP_LIMIT)

    nb_actions = env.action_space.n

    # モデルの定義
    model = Sequential()
    model.add(Flatten(input_shape=(1,) + env.observation_space.shape))
    model.add(Activation('relu'))
    model.add(Dense(16))
    model.add(Activation('relu'))
    model.add(Dense(16))
    model.add(Activation('relu'))
    model.add(Dense(16))
    model.add(Activation('relu'))
    model.add(Dense(nb_actions))
    model.add(Activation('linear'))

    memory = SequentialMemory(limit=STEP_LIMIT, window_length=1)  # experience reply で用いるmemory
    policy = BoltzmannQPolicy()  # 行動選択手法の定義

    dqn = DQNAgent(model=model, nb_actions=nb_actions, memory=memory, nb_steps_warmup=100,
                   target_model_update=1e-2, policy=policy)
    dqn.compile(Adam(learning_rate=1e-3), metrics=['mae'])

    # try:
    # 学習
    history = dqn.fit(env, nb_steps=STEP_LIMIT, visualize=True, verbose=2)
    # except Exception as e:
    #     # 問題が発生した際は伝える
    #     socket.send(b"abort")
    #     raise

    # モデルの保存
    dqn.save_weights('models/weights_{}_port_{}.h5f'.format(datetime.datetime.now().strftime('%Y-%m-%d_%H.%M.%S'), port), overwrite=True)

    # テスト
    dqn.test(env, nb_episodes=5, visualize=True)

    env.close()
