//
//  logic.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/14.
//

# include "logic.hpp"

namespace stfi {
GameUnitType::GameUnitType(){}
GameUnitType::GameUnitType(StringView typeName,
                           Features feature,
                           uint16 durability,
                           uint16 attackPower,
                           uint16 attackRange,
                           uint16 cooldown,
                           uint16 speed,
                           uint16 cost,
                           String localizeName,
                           String description)
    : typeName(typeName),feature(feature), durability(durability), attackPower(attackPower), attackRange(attackRange), cooldown(cooldown), speed(speed), cost(cost), localizeName(localizeName), description(description)
{
    generalPower = attackPower / cooldown * attackRange;
#ifndef HEADLESS
    friendTexture = Texture{ Resource(U"resource/texture/" + typeName + U"/friend.png"), TextureDesc::Mipped };
    friendAttackingTexture = Texture{ Resource(U"resource/texture/" + typeName + U"/friend_attack.png"), TextureDesc::Mipped };
    enemyTexture = Texture{ Resource(U"resource/texture/" + typeName + U"/enemy.png"), TextureDesc::Mipped };
    enemyAttackingTexture = Texture{ Resource(U"resource/texture/" + typeName + U"/enemy_attack.png"), TextureDesc::Mipped };
    friendNormalMaskRenderTexture = RenderTexture{ friendTexture.size(), ColorF{ 0.0, 1.0 } };
    friendAttackingMaskRenderTexture = RenderTexture{ friendAttackingTexture.size(), ColorF{ 0.0, 1.0 } };
    enemyNormalMaskRenderTexture = RenderTexture{ enemyTexture.size(), ColorF{ 0.0, 1.0 } };
    enemyAttackingMaskRenderTexture = RenderTexture{ enemyAttackingTexture.size(), ColorF{ 0.0, 1.0 } };
    {
        // レンダーターゲットを設定
        const ScopedRenderTarget2D target{ friendNormalMaskRenderTexture };
        Texture{ Resource(U"resource/texture/" + typeName + U"/friend_silhouette.png"), TextureDesc::Mipped }.resized(friendNormalMaskRenderTexture.size()).drawAt(friendNormalMaskRenderTexture.size() / 2);
    }
    {
        // レンダーターゲットを設定
        const ScopedRenderTarget2D target{ enemyNormalMaskRenderTexture };
        Texture{ Resource(U"resource/texture/" + typeName + U"/enemy_silhouette.png"), TextureDesc::Mipped }.resized(friendAttackingMaskRenderTexture.size()).drawAt(friendAttackingMaskRenderTexture.size() / 2);
    }
    {
        // レンダーターゲットを設定
        const ScopedRenderTarget2D target{ friendAttackingMaskRenderTexture };
        Texture{ Resource(U"resource/texture/" + typeName + U"/friend_attack_silhouette.png"), TextureDesc::Mipped }.resized(enemyNormalMaskRenderTexture.size()).drawAt(enemyNormalMaskRenderTexture.size() / 2);
    }
    {
        // レンダーターゲットを設定
        const ScopedRenderTarget2D target{ enemyAttackingMaskRenderTexture };
        Texture{ Resource(U"resource/texture/" + typeName + U"/enemy_attack_silhouette.png"), TextureDesc::Mipped }.resized(enemyAttackingMaskRenderTexture.size()).drawAt(enemyAttackingMaskRenderTexture.size() / 2);
    }
#endif
}
#ifndef HEADLESS
TextureRegion GameUnitType::getNormalTexture(bool isFriend)
{
    return isFriend ? this->friendTexture.resized(textureSize) : this->enemyTexture.resized(textureSize);
}
TextureRegion GameUnitType::getAttackingTexture(bool isFriend)
{
    return isFriend ? this->friendAttackingTexture.resized(textureSize) : this->enemyAttackingTexture.resized(textureSize);
}
#endif
// Siv3Dの関数に依存
GameUnit::GameUnit(GameUnitType type, bool is_friend)
    : GameUnit(type, is_friend, is_friend ? 50 : Scene::Width() - 50)
{}

GameUnit::GameUnit(GameUnitType type, bool is_friend, uint16 x)
{
    this->x = x;
    this->type = type.typeName;
    this->isFriend = is_friend;
    this->durability = type.durability;
}

}
