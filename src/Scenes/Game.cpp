//
//  Game.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

#include "Game.hpp"

//コンフィグの値を取得する関数
template <class Type>
Type Game::GetConfigValue(String type, String key) const
{
	return ConfigJson[U"ability_config"][type][key].get<Type>();
}

String Game::GetStringValue(String type, String key) const
{
	return ConfigJson[U"ability_config"][type][key].getString();
}

double Game::GetXPos(const double pos) const
{
	return pos * Scene::Width();
}

double Game::GetYPos(const double pos) const
{
	return border / 2 - Math::Tan(-slopeDegree) * GetXPos(pos);
}

double Game::GetYOffset(const uint16 pos) const
{
	return Math::Tan(-slopeDegree) * pos;
}

// コンストラクタ
Game::Game(const InitData& init)
	: IScene{ init },
	state(ConfigJson)
{
	// マスク用のシェーダーの読み込み
	maskShader = HLSL{ Resource(U"resource/shader/hlsl/multi_texture_mask.hlsl"), U"PS" }
	| GLSL{ Resource(U"resource/shader/glsl/multi_texture_mask.frag"), {{U"PSConstants2D", 0}} };
	if (not maskShader)
	{
		throw Error{U"Error! Failed to load a shader file."};
	}
	
	for( unsigned long int i = 0; i <  ConfigJson[U"available_types"].size(); i++)
	{
		summonButtonList.push_back(summon_button(ConfigJson[U"available_types"][i].getString(), i));
	};
	if (not getData().IsMusicMuted)
	{
		BGM.play();
	}
}

// 更新処理
void Game::update()
{
	//蓄積された秒数の記録
	totalTime += Scene::DeltaTime();
	actionAccumulator += Scene::DeltaTime();
	incomeAccumulator += Scene::DeltaTime();
	//ユニットのクリック時の処理
	for (GameUnit& item : GameUnitList)
	{
		//クリック時の処理
		RectF collisionDetection{ Arg::center(GetXPos(item.pos), Scene::Center().y - item.y * TextureSize / 8), textureSize };
		if (item.isFriend and item.type != U"pencil_lead")
		{
			if (collisionDetection.leftClicked() and item.isFriend)
			{
				//左クリック時に（味方なら）固定
				item.isFixed = true;
			}
			else if(collisionDetection.rightClicked() and item.isFriend)
			{
				//右クリック時に（味方なら）固定解除
				item.isFixed = false;
			}
		}
	}
	// 召喚ボタンのクリック時の処理
	for (unsigned long int i = 0; i < summonButtonList.size(); i++)
	{
		// ボタンがクリックされたら
		if (summonButtonList[i].background.leftClicked())
		{
			// 可能ならユニットを召喚する
			if (state.summonGameUnit(GameUnitTypeList[summonButtonList[i].type], true))
			{
				SummonSound.playOneShot();
			}
		}
	}
	
	// 資金力強化ボタン
	if (moneyButton.leftClicked() and state.FriendCamp.money >= 120 * Math::Pow(2, state.FriendCamp.profitLevel))
	{
		state.FriendCamp.money -=  120 * Math::Pow(2, state.FriendCamp.profitLevel);
		state.FriendCamp.profitLevel ++;
		SummonSound.playOneShot();
	}
	// 強化ボタンが押せるようになったら音を鳴らす
	if (state.FriendCamp.money >= 120 * Math::Pow(2, state.FriendCamp.profitLevel))
	{
		if (not WasMoneyButtonAvilable)
		{
			MoneyAvailable.playOneShot();
			WasMoneyButtonAvilable = true;
		}
	}
	else
	{
		WasMoneyButtonAvilable = false;
	}
	
	// ユニットの行動処理
	while ( actionTickLong <= actionAccumulator)
	{
		// デバック情報の削除
		ClearPrint();
		state.actionProcess(HitPop, effect);
		// AI
		if (state.winner == 0)
		{
			String target = AI.judge(state);
			if (target != U"")
			{
				if (target == U"upgrade")
				{
					state.upgradeProfitLevel(false);
				}
				else
				{
					state.summonGameUnit(state.GameUnitTypeList[target], false);
				}
			}
		}
		// 蓄積された秒数を減らす
		actionAccumulator -= actionTickLong;
	}
	// 各陣営の収入処理
	while (incomeTickLong <= incomeAccumulator)
	{
		state.profitProcess(getData().DifficultyLevel);
		incomeAccumulator -= incomeTickLong;
		
	}
	waiting_time += state.winner != 0 ? Scene::DeltaTime() : 0;
	if (waiting_time >= 4)
	{
		changeScene(U"Title");
	}
}

// 描画
void Game::draw() const
{
	// 時刻
	const double time = Math::Fmod(totalTime, 45) / 45;
	// 0~1で表される明るさ
	double brightness = Periodic::Sine1_1(1, time);
	if (brightness < 0) {
		brightness = 0;
	}
	// 背景の描画
	{
		ColorF skyColor = HSV{196, 1, brightness + 0.1};
		// 日の出であるか
		bool isSunrise;
		if ((isSunrise = time > 0.05 and time < 0.1) or (time > 0.4 and time < 0.45))
		{
			double degree;
			if (isSunrise)
			{
				degree = Periodic::Sine1_1(0.1, time - 0.05);
			}
			else
			{
				degree = Periodic::Sine1_1(0.1, 0.45 - time);
			}
			skyColor.r += degree * 0.85;
			skyColor.g += degree * 0.2;
			skyColor.b -= degree * 0.4;
		}
		Scene::SetBackground(skyColor);
		//土！！！！
		Quad{ Vec2{ 0, Scene::Height() }, Vec2{ 0, border },  Vec2{ Scene::Width(), border - GetYOffset(Scene::Width()) }, Vec2{ Scene::Width(), Scene::Height() } }
		.draw(HSV{ 31, 0.8, 0.25 + brightness * 0.5 });
		//草！！！！
		Quad{ Vec2{ 0, border }, Vec2{ 0, 0 },  Vec2{ Scene::Width(), 0 - GetYOffset(Scene::Width()) }, Vec2{ Scene::Width(), border - GetYOffset(Scene::Width()) } }
		.draw(HSV{ 120, 0.65, 0.35 + brightness * 0.5 });
	}
	
	// ユニットの描画
	for (const GameUnit& item : GameUnitList)
	{
		GameUnitType info = state.getGameUnitType(item);
		//座標
		Vec2 pos;
		double XOffset;
		double margin = Scene::Width() * 0.05;
		// スムーズに移動する
		if (item.knockBack != 0)
		// ノックバック時
		{
			XOffset = (item.isFriend ? -0.5 : 0.5) * info.speed * (actionAccumulator / actionTickLong - 1);
		}
		else if (item.cooldown == 0 and not item.isFixed)
		// 移動時
		{
			XOffset = (item.isFriend ? 1 : -1) * info.speed * (actionAccumulator / actionTickLong - 1);
		}
		else
		{
			XOffset = 0;
		}
		double YOffset = (item.y - item.previousY) * -(actionAccumulator / actionTickLong - 1);
		if (item.type == U"pencil_lead")
		{
			
			pos = { margin + GetXPos(item.pos + XOffset) * 0.9, GetYPos(item.pos) };
		}
		else
		{
			pos = { margin + GetXPos(item.pos + XOffset) * 0.9, GetYPos(item.pos) - (item.y - item.maxY / 2 - YOffset) * TextureSize / 4 };
		}
		//耐久値の割合
		float damage_proportion = (float)item.durability / info.durability;
		//シルエット
		RenderTexture silhouette;
		//攻撃しているか
		if (item.cooldown > GameUnitTypeList.at(item.type).cooldown / 5 * 3)
		{
			//攻撃テクスチャーで描画
			state.getGameUnitType(item).getAttackingTexture(item.isFriend).rotated(slopeDegree).drawAt(pos);
			silhouette = (item.isFriend ? state.getGameUnitType(item).friendAttackingMaskRenderTexture : state.getGameUnitType(item).enemyAttackingMaskRenderTexture);
		}
		else
		{
			//通常テクスチャーで描画
			state.getGameUnitType(item).getNormalTexture(item.isFriend).rotated(slopeDegree).drawAt(pos);
			silhouette = (item.isFriend ? state.getGameUnitType(item).friendNormalMaskRenderTexture : state.getGameUnitType(item).enemyNormalMaskRenderTexture);
		}
		//割れ目の描画
		if (damage_proportion < 0.6) {
			// 何番目の割れ目を使って描画するか
			size_t crackIndex;
			if (damage_proportion < 0.15)
			{
				crackIndex = 2;
			}
			else if (damage_proportion < 0.3)
			{
				crackIndex = 1;
			}
			else
			{
				crackIndex = 0;
			}
			
			Graphics2D::SetPSTexture(1, silhouette);
			// マルチテクスチャによるマスクのシェーダを開始
			const ScopedCustomShader2D shader{ maskShader };
			crack[crackIndex].rotated(slopeDegree).drawAt(pos);
		}
	}
	
	// 耐久力バーの描画
	if (state.winner == 0)
	{
		float friendCastleDamage = (float)GameUnitList[0].durability / state.getGameUnitType(GameUnitList[0]).durability;
		float enemyCastleDamage = (float)GameUnitList[1].durability / state.getGameUnitType(GameUnitList[0]).durability;
		Color friend_castle_bar_color;
		Color enemy_castle_bar_color;
		if (friendCastleDamage > 0.6 )
		{
			friend_castle_bar_color = Palette::Greenyellow;
		}
		else if (friendCastleDamage > 0.3)
		{
			friend_castle_bar_color = Palette::Yellow;
		}
		else
		{
			friend_castle_bar_color = Palette::Red;
		}
		
		if (enemyCastleDamage > 0.6 )
		{
			enemy_castle_bar_color = Palette::Greenyellow;
		}
		else if (enemyCastleDamage > 0.3)
		{
			enemy_castle_bar_color = Palette::Yellow;
		}
		else
		{
			enemy_castle_bar_color = Palette::Red;
		}

		Line{10, Scene::Center().y - 100, 110, Scene::Center().y - 100}.draw(10, Palette::Darkgray);
		Line{10, Scene::Center().y - 100, 10 + 100 * friendCastleDamage, Scene::Center().y - 100}.draw(10, friend_castle_bar_color);
		Line{Scene::Width() - 110, Scene::Center().y - 100, Scene::Width() - 10, Scene::Center().y - 100}.draw(10, Palette::Darkgray);
		Line{Scene::Width() - 10 - 100 * enemyCastleDamage, Scene::Center().y - 100, Scene::Width() - 10, Scene::Center().y - 100}.draw(10, enemy_castle_bar_color);
	}
	
	// 資金残高表記
	moneyInfoFont(U"手持ちのお金: " + Format(state.FriendCamp.money) + U"円").draw(0, Scene::Height() - TextureSize - 40);
	
	// マウスオーバーされた召喚ボタン
	String MouseOveredButton = U"";
	//召喚ボタンの描画
	for (const auto& item : summonButtonList)
	{
		//背景と枠線の描画
		item.background.draw(GetConfigValue<int>(item.type, U"cost") > state.FriendCamp.money ? ColorF{ 0.4, 0.4, 0.4 } : item.background.mouseOver() ? ColorF{ 0.5, 0.5, 0.5 } : ColorF{ 0.7, 0.7, 0.7 }).drawFrame(3, 0, ColorF{ 0.3, 0.3, 0.3 });
		//画像の描画(* 0.4 = * 0.8 / 2)
		item.texture.resized(TextureSize * 0.8).drawAt(item.x,Scene::Size().y - TextureSize * 0.4);
		//ラベルの描画
		summonButtonFont(Format(GameUnitTypeList[item.type].cost) + U"円").drawAt(item.x,Scene::Size().y - TextureSize * 0.875);
		if (item.background.mouseOver())
		{
			MouseOveredButton = item.type;
		}
	}
	
	// 資金力強化ボタン
	moneyButton.draw(120 * Math::Pow(2, state.FriendCamp.profitLevel) > state.FriendCamp.money ? static_cast<HSV>(Palette::Darkgoldenrod) : moneyButton.mouseOver() ? static_cast<HSV>(Palette::Goldenrod) : HSV{ 40, 0.8, 1.0 }).drawFrame(3, 0, Palette::Goldenrod);
	upgradeButtonFont( U"資金力強化に\n投資する\n" + Format(120 * Math::Pow(2, state.FriendCamp.profitLevel)) + U"円" ).drawAt(Scene::Width() - TextureSize / 2, Scene::Height() - TextureSize * 0.5);
	
	// 説明欄
	if (not (MouseOveredButton == U""))
	{
		Rect{0, 0, Scene::Width(), 150}.draw(ColorF(Palette::Dimgray, 0.8)).drawFrame(2, 0, Palette::Black);
		descriptionFont(GetStringValue(MouseOveredButton, U"description")).draw(TextureSize + 30, 10);
		GameUnitTypeList[MouseOveredButton].getNormalTexture(true).draw(14, 2);
		nameFont(GetStringValue(MouseOveredButton, U"ja_name")).draw(2, TextureSize + 2);
	}
	// エフェクトの更新
	effect.update();
}
