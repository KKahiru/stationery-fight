//
//  logic.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/14.
//

# include <algorithm>

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

# ifdef HEADLESS
GameState::GameState(json ConfigJson)
{
	//召喚ボタン・ユニットの情報の登録
	for (unsigned long int i = 0; i <  ConfigJson["all_types"].size(); i++)
	{
		std::string type = ConfigJson["all_types"][i].get<std::string>();
		String wideType = Widen(type);
		json unit = ConfigJson["ability_config"][type];
		GameUnitTypeList.emplace(wideType, GameUnitType(wideType,
														FeaturesTable[Widen(unit["feature"].get<std::string>())],
														unit["durability"].get<uint16>(),
														unit["attack_power"].get<uint16>(),
														unit["attack_range_end"].get<double>() / 1000,
														unit["cooldown"].get<uint16>(),
														unit["speed"].get<double>() / 1000,
														unit["cost"].get<uint16>(),
														Widen(unit["ja_name"].get<std::string>()),
														Widen(unit["description"].get<std::string>())
														));
	};
	// 選択肢リストの設定
	OptionList.push_back(U"upgrade");
	for (unsigned long int i = 0; i <  ConfigJson["all_types"].size(); i++)
	{
		OptionList.push_back(Widen(ConfigJson["all_types"][i].get<std::string>()));
	}
	//城を造る
	GameUnitList.push_back(GameUnit(GameUnitTypeList[U"castle"], true));
	GameUnitList.push_back(GameUnit(GameUnitTypeList[U"castle"], false));
}
# else
GameState::GameState(JSON ConfigJson)
{
	//召喚ボタン・ユニットの情報の登録
	for (unsigned long int i = 0; i <  ConfigJson[U"all_types"].size(); i++)
	{
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
	// 選択肢リストの設定
	OptionList.push_back(U"upgarde");
	for (unsigned long int i = 0; i <  ConfigJson[U"all_types"].size(); i++)
	{
		OptionList.push_back(ConfigJson[U"all_types"][i].get<String>());
	}
	//城を造る
	GameUnitList.push_back(GameUnit(GameUnitTypeList[U"castle"], true));
	GameUnitList.push_back(GameUnit(GameUnitTypeList[U"castle"], false));
}
# endif
//GameUnitからGameUnitTypeを取得する関数
GameUnitType GameState::getGameUnitType(const GameUnit& target) const
{
	return GameUnitTypeList.at(target.type);
}

bool GameState::summonGameUnit(GameUnitType type, bool isFriend)
{
	CampInfo& camp = isFriend ? FriendCamp : EnemyCamp;
	if (camp.money >= type.cost)
	{
		GameUnitList.push_back(GameUnit(type, isFriend));
		camp.money -= type.cost;
		return true;
	}
	else
	{
		return false;
	}
}

bool GameState::upgradeProfitLevel(bool isFriend)
{
	CampInfo& camp = isFriend ? FriendCamp : EnemyCamp;
	const uint16 cost = 120 * Math::Pow(2, camp.profitLevel);
	if (camp.money >= cost)
	{
		camp.money -= cost;
		camp.profitLevel++;
		return true;
	}
	else
	{
		return false;
	}

}

void GameState::profitProcess(uint8 difficulty)
{
	//難易度に合わせて資金を増加させる
	switch (difficulty) {
		case 1:
			FriendCamp.money += 10 * Math::Pow(1.5, FriendCamp.profitLevel);
			EnemyCamp.money += 7 * Math::Pow(1.5, EnemyCamp.profitLevel);
			break;
		default:
		case 2:
			FriendCamp.money += 10 * Math::Pow(1.5, FriendCamp.profitLevel);
			EnemyCamp.money += 10 * Math::Pow(1.5, EnemyCamp.profitLevel);
			break;
		case 3:
			FriendCamp.money += 7 * Math::Pow(1.5, FriendCamp.profitLevel);
			EnemyCamp.money += 12 * Math::Pow(1.5, EnemyCamp.profitLevel);
			break;
	}
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
						target->knockBack = 5;
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
						GameUnitList[i].pos -= getGameUnitType(GameUnitList[i]).speed / 2;
					}
					else
					{
						GameUnitList[i].pos += getGameUnitType(GameUnitList[i]).speed / 2;
					}
				}
			}
		}
		// Y座標の更新処理
		{
			if (GameUnitList[i].type == U"castle")
			{
				// 城は初期値（0）で固定する
				continue;
			}
			
			// 影響を受ける範囲の定義
			constexpr double influenceRange = 0.1;
			const double influenceRangeBegin = GameUnitList[i].pos - influenceRange / 2;
			const double influenceRangeEnd = influenceRangeBegin + influenceRange;
			// すでに占領されているY座標
			Array<uint8> filledYList;
			for (size_t j = 0; j < GameUnitList.size(); j++)
			{
				const GameUnit& target = GameUnitList[j];

				// 範囲内なら
				if (target.pos >= influenceRangeBegin
					and target.pos <= influenceRangeEnd
					and target.type != U"castle"
					and &target != &GameUnitList[i])
				{
					filledYList.push_back(target.y);
				}
			}
			if (filledYList.size() == 0)
			{
				GameUnitList[i].previousY = GameUnitList[i].y;
				GameUnitList[i].maxY = 0;
				GameUnitList[i].y = 0;
			}
			else
			{
				// Y座標関連の情報を設定
				GameUnitList[i].previousY = GameUnitList[i].y;
				GameUnitList[i].maxY = *max_element(filledYList.begin(), filledYList.end());
				// 空いてなかったら一番上へ行く
				GameUnitList[i].y = filledYList.size();
				// 一つずつ空いているか確認する
				for (size_t j = 0; j < filledYList.size(); j++)
				{
					// 空いているなら
					if (*std::find(filledYList.begin(), filledYList.end(), j) != j)
					{
						GameUnitList[i].y = j;
						break;
					}
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
				winner = GameUnitList[i].isFriend ? 2 : 1;
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
