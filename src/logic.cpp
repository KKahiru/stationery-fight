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
                           double attackRange,
                           uint16 cooldown,
                           double speed,
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
GameUnit::GameUnit(GameUnitType type, bool isFriend)
    : GameUnit(type, isFriend, isFriend ? 0 : 1)
{}

GameUnit::GameUnit(GameUnitType type, bool isFriend, double pos)
{
    this->pos = pos;
    this->type = type.typeName;
    this->isFriend = isFriend;
    this->durability = type.durability;
}

# ifndef HEADLESS
GameState::GameState(JSON ConfigJson)
{
    //召喚ボタン・ユニットの情報の登録
    for( unsigned long int i=0; i <  ConfigJson[U"all_types"].size(); i++) {
        String type = ConfigJson[U"all_types"][i].getString();
        JSON unit = ConfigJson[U"ability_config"][type];
        GameUnitTypeList.emplace(type, GameUnitType(type,
                                                    FeaturesTable[unit[U"feature"].get<String>()],
                                                    unit[U"durability"].get<uint16>(),
                                                    unit[U"attack_power"].get<uint16>(),
                                                    unit[U"attack_range_end"].get<double>() / 1000,
                                                    unit[U"cooldown"].get<uint16>(),
                                                    unit[U"speed"].get<double>() / 1000,
                                                    unit[U"cost"].get<uint16>(),
                                                    unit[U"ja_name"].get<String>(),
                                                    unit[U"description"].get<String>()
                                                    ));
    };
    //城を造る
    GameUnitList.push_back(GameUnit(GameUnitTypeList[U"castle"], true));
    GameUnitList.push_back(GameUnit(GameUnitTypeList[U"castle"], false));
}
# endif
//GameUnitからGameUnitTypeを取得する関数
GameUnitType GameState::getGameUnitType(const GameUnit& target)
{
    return GameUnitTypeList.at(target.type);
}

bool GameState::summonGameUnit(GameUnitType type, bool isFriend){
    if ((isFriend ? FriendCamp.money : EnemyCamp.money) >= type.cost){
        GameUnitList.push_back(GameUnit(type, isFriend));
        (isFriend ? FriendCamp.money : EnemyCamp.money) -= type.cost;
    } else {
        return false;
    }
    return true;
}

void WeakAI::saveMoney(CampInfo& info)
{
    if (AISavingMoney < (AIStep + 1) * 25 * Math::Pow(2, info.profitLevel) )
    {
        AISavingMoney += info.money;
        info.money = 0;
    }
    return;
}
void WeakAI::judge(GameState& state)
{
    //勢力の計算
    double friendPower = 0, enemyPower = 0;
    for (unsigned long int i = 0; i < state.GameUnitList.size(); i++)
    {
        if (state.GameUnitList[i].type != U"castle" and state.GameUnitList[i].type != U"pencil_lead")
        {
            //強さは攻撃の強さと耐久力の合計
            (state.GameUnitList[i].isFriend ? friendPower : enemyPower) += (state.getGameUnitType(state.GameUnitList[i]).generalPower + state.GameUnitList[i].durability);
        }
    }
    float powerRatio;
    //0除算の防止
    if (friendPower == 0) {
        powerRatio = enemyPower == 0 ? 1 : 2;
    } else {
        powerRatio = enemyPower / friendPower;
    }
    //下準備
    if (AIMode == 0){
        AIMode = round(Random()) + 1;
    }
    //召喚するユニットの決定
    if (AIStep != 3) {
        //勢力の割合に応じた分岐
        if (powerRatio < 0.5) {
            switch (state.EnemyCamp.profitLevel) {
                case 0:
                case 1:
                    AITarget = U"eraser";
                    break;
                case 2:
                case 3:
                    AITarget = U"triangle";
                    break;
                case 4:
                default:
                    if (AIStep == 0){
                        AITarget = U"pencil_sharpener";
                    } else {
                        AITarget = U"mechanical_pencil";
                    }
                    break;
            }
        } else if (powerRatio < 1.25){
            switch (state.EnemyCamp.profitLevel) {
                case 0:
                case 1:
                    saveMoney(state.EnemyCamp);
                    AITarget = U"pencil";
                    break;
                case 2:
                case 3:
                    
                    AITarget = U"triangle";
                    break;
                case 4:
                default:
                    saveMoney(state.EnemyCamp);
                    if (AIMode == 1) {
                        //遠距離の召喚
                        AITarget = (AIStep == 0) ? U"mechanical_pencil" : U"triangle";
                    } else {
                        AITarget = U"triangle";
                    }
                    break;
            }
        } else {
            //攻めプロセス
            switch (state.EnemyCamp.profitLevel) {
                case 0:
                case 1:
                    saveMoney(state.EnemyCamp);
                    break;
                case 2:
                case 3:
                    saveMoney(state.EnemyCamp);
                    AITarget=U"triangle";
                    break;
                case 4:
                default:
                    saveMoney(state.EnemyCamp);
                    if (AIStep == 0){
                        AITarget=U"pencil_sharpener";
                    } else {
                        AITarget=U"mechanical_pencil";
                    }
                    break;
            }
        }
    }
    if (AIStep == 3) {
        state.EnemyCamp.money += AISavingMoney;
        AISavingMoney = 0;
        if (120 * Math::Pow(2, state.EnemyCamp.profitLevel) <= state.EnemyCamp.money) {
            state.EnemyCamp.money -= 120 * Math::Pow(2, state.EnemyCamp.profitLevel);
            state.EnemyCamp.profitLevel ++;
            AIStep = 0;
            AIMode = 0;
        }
    } else /*召喚を試みる*/if (not AITarget.isEmpty() and state.summonGameUnit(state.GameUnitTypeList[AITarget], false)) {
        AIStep ++;
        AITarget = U"";
    }
#if defined(DEBUG) && !defined(HEADLESS)
    if (KeyShift.pressed()) {
        Print << U"Mode: " << AIMode;
        Print << U"Target: " << (AITarget == U"" ? U"資金力強化" : AITarget);
        Print << U"power" + Format(friendPower) + U"/" + Format(enemyPower);
        Print << U"money: " + Format(state.FriendCamp.money) + U" / " + Format(state.FriendCamp.money);
    }
#endif
    
}

# ifndef HEADLESS
FinishEffect::FinishEffect(const bool input)
    : isFriendWinner{input}
{}

bool FinishEffect::update(double t)
{
    //描画
    finishFont(isFriendWinner ? U"You Win!!" : U"You Lose...").drawAt(Scene::Center(),isFriendWinner ? ColorF{0.9, 0.9, 0} :  ColorF{0, 0.1, 0.9});
    // 3 秒未満なら継続
    return (t < 3.0);
}
# endif

# ifdef HEADLESS
void GameState::actionProcess()
# else
void GameState::actionProcess(const Audio& HitPop, Effect& effect)
# endif
{
    //ユニットの行動処理
    for (unsigned long int i = 0; i < GameUnitList.size(); i++)
    {
        //クールダウンが終わったかの判定
        if (GameUnitList[i].cooldown == 0 and (GameUnitList[i].knockBack == 0 or (GameUnitList[i].type == U"castle" or GameUnitList[i].type == U"pencil_sharpener" )))
        {
            //攻撃したかのフラグ
            bool hasAttacked = false;
            GameUnit* target = nullptr;
            //攻撃するユニットの検索
            for (unsigned long int j = 0; j < GameUnitList.size(); j++)
            {
                //相手が敵で、攻撃範囲内にいるかを判別する
                if (( GameUnitList[i].isFriend
                     ? not GameUnitList[j].isFriend
                     and GameUnitList[j].pos <= GameUnitList[i].pos + getGameUnitType(GameUnitList[i]).attackRange
                     : GameUnitList[j].isFriend
                     and GameUnitList[j].pos >= GameUnitList[i].pos - getGameUnitType(GameUnitList[i]).attackRange )
                    and GameUnitList[j].type != U"pencil_lead")
                {
                    if (target == nullptr)
                    {
                        target = &GameUnitList[j];
                    }
                    else if ( GameUnitList[i].isFriend ? GameUnitList[j].pos < target->pos : GameUnitList[j].pos > target->pos ){
                        target = &GameUnitList[j];
                    }
                }
            }
            //ターゲットが見つかったのなら
            if (target != nullptr)
            {
                //攻撃処理
                if (getGameUnitType(GameUnitList[i]).feature == Features::gun)
                {
                    //シャー芯を召喚
                    GameUnitList.push_back(GameUnit(GameUnitTypeList[U"pencil_lead"], GameUnitList[i].isFriend, GameUnitList[i].pos));
                }
                else
                {
                    target->durability -= getGameUnitType(GameUnitList[i]).attackPower;
                    //攻撃時の、種類ごとのユニークな処理
                    if (GameUnitList[i].type == U"eraser")
                    {
                        GameUnitList[i].durability -= 5;
                    } else if (GameUnitList[i].type == U"pencil_lead")
                    {
                        GameUnitList[i].durability = 0;
                        //ノックバック
                        target->knockBack = 10;
                    } else {
                        //ノックバック
                        target->knockBack = 1;
                    }
                }
                //クールダウン
                GameUnitList[i].cooldown = getGameUnitType(GameUnitList[i]).cooldown;
                //攻撃したフラグを立てる
                hasAttacked = true;
            }
            //攻撃したかの判定
            if (not hasAttacked)
            {
                if (not GameUnitList[i].isFixed)
                {
                    //移動
                    //味方かの判定
                    if (GameUnitList[i].isFriend)
                    {
                        //正の方向に移動
                        GameUnitList[i].pos += getGameUnitType(GameUnitList[i]).speed;
                    }
                    else
                    {
                        //負の方向に移動
                        GameUnitList[i].pos -= getGameUnitType(GameUnitList[i]).speed;
                    }
                }
            }
            else
            {
                //攻撃の効果音
# ifndef HEADLESS
                HitPop.playOneShot();
# endif
            }
        }
        else
        {
            if (GameUnitList[i].knockBack == 0)
            {
                //クールダウンのカウントを減らす
                GameUnitList[i].cooldown --;
            }
            else
            {
                //ノックバックのカウントを減らし、後ろに弾き飛ばす
                GameUnitList[i].knockBack --;
                if (GameUnitList[i].type != U"castle"
                    and GameUnitList[i].type != U"pencil_sharpener")
                {
                    if (GameUnitList[i].isFriend)
                    {
                        GameUnitList[i].pos -= knockbackAmount;
                    }
                    else
                    {
                        GameUnitList[i].pos += knockbackAmount;
                    }
                }
            }
        }
        //破壊判定
        for (long int i = GameUnitList.size() - 1; i >= 0;  i--)
        {
            if (GameUnitList[i].durability <= 0)
            {
                //城が破壊された場合は、勝敗を記録し、終了エフェクトを表示する
                if (GameUnitList[i].type == U"castle")
                {
                    winner = GameUnitList ? 1 : 2;
# ifndef HEADLESS
                    effect.add<FinishEffect>(!GameUnitList[i].isFriend);
# endif
                }
                //削除
                GameUnitList.erase(GameUnitList.begin() + i);
            }
        }
    }
}

}
