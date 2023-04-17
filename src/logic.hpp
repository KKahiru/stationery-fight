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
const uint8 textureSize = 120;
#endif
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
    uint16 attackRange;
    //総合的な攻撃の強さ
    uint16 generalPower;
    // クールダウン
    uint16 cooldown;
    // 移動速度
    uint16 speed;
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
                 uint16 attackRange,
                 uint16 cooldown,
                 uint16 speed,
                 uint16 cost,
                 String localizeName,
                 String description);
};
//game_unitのクラス
class GameUnit{
public:
    //味方かのbool値
    bool isFriend;
    //game_unitのタイプ
    String type;
    //x座標
    uint16 x;
    //耐久値
    int16 durability;
    //クールダウン
    uint16 cooldown = 0;
    //ノックバック
    uint16 knock_back = 0;
    //固定されているか
    bool is_fixed = false;
    //コンストラクター
    GameUnit(GameUnitType type, bool is_friend);
    GameUnit(GameUnitType type, bool is_friend, uint16 x);
    //当たり判定
    Rect collisionDetection{ textureSize };
};

}
