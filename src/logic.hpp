//
//  logic.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/14.
//

# pragma once

# ifdef HEADLESS
# include "types.hpp"
# else
# include <Siv3D.hpp>
# endif

namespace stfi {
#ifndef HEADLESS
constexpr uint8 textureSize = 120;
#endif

constexpr double knockbackAmount = 0.005;

enum class Features
{
    normal,
    eraser,
    longRange,
    gun
};

inline HashTable<String, Features> FeaturesTable =
{
    { U"normal", Features::normal },
    { U"eraser", Features::eraser },
    { U"long_range", Features::longRange },
    { U"shot", Features::gun },
};

struct GameUnitType
{
public:
    // タイプ名
    String typeName;
    // 特性
    Features feature;
    // 耐久値
    uint16 durability;
    // 攻撃力
    uint16 attackPower;
    // 攻撃範囲
    double attackRange;
    //総合的な攻撃の強さ
    uint16 generalPower;
    // クールダウン
    uint16 cooldown;
    // 移動速度
    double speed;
    // コスト
    uint16 cost;
    // ローカライズ済みの名前
    String localizeName;
    // 説明
    String description;
# ifndef HEADLESS
    //通常時テクスチャー
    Texture friendTexture;
    //攻撃時テクスチャー
    Texture friendAttackingTexture;
    //通常時テクスチャー
    Texture enemyTexture;
    //攻撃時テクスチャー
    Texture enemyAttackingTexture;
    //通常時テクスチャの形のマスクのレンダーテクスチャ
    RenderTexture friendNormalMaskRenderTexture;
    RenderTexture enemyNormalMaskRenderTexture;
    //攻撃時テクスチャの形のマスクのレンダーテクスチャ
    RenderTexture friendAttackingMaskRenderTexture;
    RenderTexture enemyAttackingMaskRenderTexture;
    
    TextureRegion getNormalTexture(bool isFriend);
    
    TextureRegion getAttackingTexture(bool isFriend);
    
    
# endif
    // コンストラクタ
    GameUnitType();
    GameUnitType(StringView typeName,
                 Features feature,
                 uint16 durability,
                 uint16 attackPower,
                 double attackRange,
                 uint16 cooldown,
                 double speed,
                 uint16 cost,
                 String localizeName,
                 String description);
};
//ユニットのクラス
class GameUnit{
public:
    //味方かのbool値
    bool isFriend;
    //ユニットのタイプ
    String type;
    // 位置
    double pos;
    //耐久値
    int16 durability;
    //クールダウン
    uint16 cooldown = 0;
    //ノックバック
    uint16 knockBack = 0;
    //固定されているか
    bool isFixed = false;
    //コンストラクター
    GameUnit(GameUnitType type, bool isFriend);
    GameUnit(GameUnitType type, bool isFriend, double pos);
    //当たり判定
    Rect collisionDetection{ textureSize };
};
//プレイヤーの情報を格納する構造体
struct CampInfo
{
    uint16 money = 0;
    uint16 profitLevel = 0;
    uint16 gameUnitPower;
};

# ifndef HEADLESS
//ゲーム終了時のエフェクト
struct FinishEffect : IEffect
{
    //ゲーム終了時のエフェクトのフォント
    Font finishFont{ 120 };
    bool isFriendWinner;

    // このコンストラクタ引数が、Effect::add<FinishEffect>() の引数になる
    explicit FinishEffect(const bool input);
    
    bool update(double t) override;
};
# endif

class GameState
{
public:
# ifndef HEADLESS
    GameState(JSON ConfigJson);
# endif
    // GameUnitTypeの連装配列
    HashTable<String, GameUnitType> GameUnitTypeList = {};
    // GameUnitからGameUnitTypeを取得する関数
    GameUnitType getGameUnitType(const GameUnit& target);
    // プレイヤーと敵の情報生成
    CampInfo FriendCamp, EnemyCamp;
    // ステージ上のユニットのリスト
    Array<GameUnit> GameUnitList;
    // ユニットを召喚する関数
    bool summonGameUnit(GameUnitType type, bool isFriend);
    // 勝敗情報
    uint8 winner = 0;
    // 行動処理
# ifdef HEADLESS
    void actionProcess();
# else
    void actionProcess(const Audio& HitPop, Effect& effect);
# endif
};
class WeakAI
{
    //敵AIの攻撃のモード
    uint8 AIMode = 0;
    //敵AIの進行度
    uint16 AIStep = 0;
    //敵AIが召喚するターゲット
    String AITarget = U"";
    //AIの貯金
    uint16 AISavingMoney = 0;
    void saveMoney(CampInfo& info);
public:
    // 判断する関数
    void judge(GameState& state);
};

}
