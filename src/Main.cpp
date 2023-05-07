# include <Siv3D.hpp> // OpenSiv3D v0.6.8
# include "logic.hpp"
# include "player.hpp"

using namespace stfi;

const int texture_size = 120;
//コンフィグ
JSON ConfigJson;
//コンフィグの値を取得する関数
template <class Type>
Type get_config_value(String type, String key)
{
	return ConfigJson[U"ability_config"][type][key].get<Type>();
}

String GetStringValue (String type, String key)
{
	return ConfigJson[U"ability_config"][type][key].getString();
}

uint16 GetXPos (const double& pos)
{
	return pos * Scene::Width();
}
//難易度
int difficult_level = 2;
//シーンマネージャーをAppに割り当てる
using App = SceneManager<String>;
// タイトルシーン
class Title : public App::Scene
{
	Font title_font{90, Typeface::Bold};
	Font button_font{35};
	Point play_button_vec{Scene::Center().x, Scene::Center().y - 10};
	Rect play_button{Arg::center(play_button_vec), 320, 60};
	Point rule_button_vec{Scene::Center().x, Scene::Center().y + 90};
	Rect rule_button{Arg::center(rule_button_vec), 320, 60};

	public:
		// コンストラクタ（必ず実装）
		Title(const InitData& init)
		: IScene{ init }
		{
			//背景を青くする
			Scene::SetBackground(Palette::Skyblue);
		}
		
		// 更新関数（オプション）
		void update() override
		{
			if (play_button.leftClicked())
			{
				AudioAsset(U"choose").playOneShot();
				changeScene(U"DifficultySetting");
			}
			else if (rule_button.leftClicked())
			{
				AudioAsset(U"choose").playOneShot();
				changeScene(U"RuleExplanation");
			}
		}
		
		// 描画関数（オプション）
		void draw() const override
		{
			
			title_font(U"Stationery Fight!").drawAt(Scene::Center().x, Scene::Center().y - 190, Palette::Dodgerblue);
			//プレイボタン
			play_button.draw(play_button.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
			button_font(U"プレイ！").drawAt(play_button_vec,Palette::Black);
			//ルールボタン
			rule_button.draw(rule_button.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
			button_font(U"遊び方・ルール").drawAt(rule_button_vec,Palette::Black);
		}
};

class RuleExplanation : public App::Scene
{
	Font ViewFont{30};
	//ページ数(1から数える)
	uint8 page_num = 1;
	Array<Texture> image_data = {
		Texture{Resource(U"resource/tutorial/normal.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/castle.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/money.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/summon_button.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/money_button.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/fixing.png"), TextureDesc::Mipped}
	};
	Array<String> page_data = {
		U"たまったお金で文房具を増やして、\n敵の文房具を倒し、最終的に\n相手の「城」を陥落させたら勝ちです。",
		U"右の青い城が自分で、左の赤い城が敵です。\n城の上のバーを見れば、城が受けたダメージがわかります。",
		U"表示された金額を払って、文房具を呼び出します。\n手持ちのお金はボタンの上に表示されています。",
		U"お金が貯まり、下の文房具ボタンの色が変わったら、\nその文房具を呼び出せます。",
		U"お金は時間経過で貯まりますが、\n投資ボタン（右下の黄色いボタン）をクリックすることで\n貯まる速度を早めることができます。",
		U"自分の文房具を左クリックすると\nその場に固定し、右クリックすると固定を\n解除することもできます。"
	};
	public:
		//コンストラクタ
		RuleExplanation(const InitData& init)
		: IScene{ init }
		{
			
		}
		// 更新関数（オプション）
		void update() override
		{
			if (SimpleGUI::Button(U"<", Vec2{Scene::Center().x - 150, 485}) and page_num != 1)
			{
				page_num --;
			}
			if (SimpleGUI::Button(U">", Vec2{Scene::Center().x + 100, 485}) and page_num < page_data.size())
			{
				page_num ++;
			}
			if (SimpleGUI::Button(U"タイトル画面に戻る", Vec2{10, 50}))
			{
				AudioAsset(U"choose").playOneShot();
				changeScene(U"Title");
			}
			if (SimpleGUI::Button(U"ゲームを始める", Vec2{Scene::Width() - 210, 50}))
			{
				AudioAsset(U"choose").playOneShot();
				changeScene(U"DifficultySetting");
			}
		}
		
		// 描画関数（オプション）
		void draw() const override
		{
			image_data[page_num - 1].resized(480, 270).drawAt(Scene::Center().x, Scene::Center().y - 100);
			ViewFont(page_data[page_num - 1]).drawAt(Scene::Center().x, Scene::Center().y + 100);
			ViewFont( Format(page_num) + U"/" + Format(page_data.size()) ).drawAt(Scene::Center().x, 500);
			
		}
};
class DifficultySetting : public App::Scene
{
	Font GuideFont{40};
	Font ButtonFont{35};
	Point EasyButtonPos{Scene::Center().x, Scene::Center().y - 30};
	Rect EasyButton{Arg::center(EasyButtonPos), 500, 60};
	Point NormalButtonPos{Scene::Center().x, Scene::Center().y + 60};
	Rect NormalButton{Arg::center(NormalButtonPos), 500, 60};
	Point HardButtonPos{Scene::Center().x, Scene::Center().y + 150};
	Rect HardButton{Arg::center(HardButtonPos), 500, 60};
	public:
		//コンストラクタ
		DifficultySetting(const InitData& init)
		: IScene{ init }
		{

		}
		// 更新関数（オプション）
		void update() override
		{
			if (EasyButton.leftClicked())
			{
				difficult_level = 1;
				AudioAsset(U"choose").playOneShot();
				changeScene(U"GameScene");
			}
			else if (NormalButton.leftClicked())
			{
				difficult_level = 2;
				AudioAsset(U"choose").playOneShot();
				changeScene(U"GameScene");
			}
			else if (HardButton.leftClicked())
			{
				difficult_level = 3;
				AudioAsset(U"choose").playOneShot();
				changeScene(U"GameScene");
			}
		}
		
		// 描画関数（オプション）
		void draw() const override
		{
			GuideFont(U"難易度を選択してください").drawAt(Scene::Center().x, 100);
			EasyButton.draw(EasyButton.mouseOver() ? Palette::Green : Palette::Lightgreen).drawFrame(1, Palette::Black);
			ButtonFont(U"簡単").drawAt(EasyButtonPos,Palette::Black);
			NormalButton.draw(NormalButton.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
			ButtonFont(U"ちょっと難しい（ノーマル）").drawAt(NormalButtonPos,Palette::Black);
			HardButton.draw(HardButton.mouseOver() ? Palette::Darkred : Palette::Orangered).drawFrame(1, Palette::Black);
			ButtonFont(U"ほぼ不可能").drawAt(HardButtonPos,Palette::Black);
			
		}
};

// ゲームシーン
class GameScene : public App::Scene
{
	//召喚ボタンの情報を格納する構造体
	struct summon_button
	{
		summon_button(String type, uint16 count)
		{
			//種類の登録
			this->type = type;
			//ボタンのX座標
			this->x = texture_size * (count + 0.5) - 3 * count;
			this->texture = Texture{ Resource(U"resource/texture/" + type + U"/friend.png"), TextureDesc::Mipped };
			//ボタンの背景
			this->background = Rect{texture_size * count - 3 * count,Scene::Size().y - texture_size,texture_size,texture_size};
		};
		String type;
		Texture texture;
		uint16 x;
		Rect background;
		
	};
	// 蓄積された時間（秒）
	double actionAccumulator = 0.0;
	double incomeAccumulator = 0.0;
	//ひび割れ
	TextureRegion crack[3] = { Texture{ Resource(U"resource/texture/crack/crack-1.png"), TextureDesc::Mipped }.resized(texture_size),
		Texture{ Resource(U"resource/texture/crack/crack-2.png"), TextureDesc::Mipped }.resized(texture_size),
		Texture{ Resource(U"resource/texture/crack/crack-3.png"), TextureDesc::Mipped }.resized(texture_size) };
	//攻撃音
	const Audio HitPop{ Resource(U"resource/sound/hit_pop_1.ogg") };
	//召喚音
	const Audio SummonSound{ Resource(U"resource/sound/summon.ogg") };
	//資金力ボタンが押せるようになった時の音
	const Audio MoneyAvailable{ Resource(U"resource/sound/money_available.mp3") };
	//BGM
	const Audio BGM{ Audio::Stream, Resource(U"resource/sound/bgm.mp3"), Loop::Yes };
	//エフェクト
	Effect effect;
	//マスク用のシェーダー
	PixelShader maskShader;
	//召喚ボタンのリスト
	Array<summon_button> summonButtonList;
	//召喚ボタンのラベルのフォント
	Font summonButtonFont{ (int32)(texture_size * 0.15) };
	//資金力強化ボタンの背景
	Rect moneyButton{ Scene::Size().x - texture_size, Scene::Size().y - texture_size, texture_size };
	//資金力強化ボタンのラベルのフォント
	Font upgradeButtonFont{ 18 };
	//資金情報
	Font moneyInfoFont{ 22 };
	//解説のフォント
	Font descriptionFont{ 25 };
	Font nameFont{ 20 };
	
	GameState state;
	
	WeakAI AI;
	
	Array<GameUnit>& GameUnitList = state.GameUnitList;
	HashTable<String, GameUnitType>& GameUnitTypeList = state.GameUnitTypeList;
	
	//勝敗決定後の待機時間
	float waiting_time = 0;
	//「1ティック前に」資金力ボタンが押せたか
	bool WasMoneyButtonAvilable = false;
public:
	// コンストラクタ（必ず実装）
	GameScene(const InitData& init)
		: IScene{ init },
		state(ConfigJson)
	{
		// 背景の設定
		Scene::SetBackground(Palette::Lightskyblue);
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
		BGM.play();
	}
	
	// 更新処理
	void update() override
	{
		//蓄積された秒数の記録
		actionAccumulator += Scene::DeltaTime();
		incomeAccumulator += Scene::DeltaTime();
		//ユニットのクリック時の処理
		for (GameUnit& item : GameUnitList)
		{
			//クリック時の処理
			RectF collisionDetection{ Arg::center(GetXPos(item.pos), Scene::Center().y - item.y * texture_size / 8), textureSize };
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
		
		// ユニットの行動処理（0.05秒刻み）
		while ( 0.05 <= actionAccumulator)
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
			actionAccumulator -= 0.1;
		}
		// 各陣営の収入処理
		while (0.4 <= incomeAccumulator)
		{
			state.profitProcess(difficult_level);
			incomeAccumulator -= 0.4;
			
		}
		waiting_time += state.winner != 0 ? Scene::DeltaTime() : 0;
		if (waiting_time >= 4)
		{
			changeScene(U"Title");
		}
		// エフェクトの更新
		effect.update();
	}
	
	// 描画
	void draw() const override
	{
		// ユニットの描画
		for (const GameUnit& item : GameUnitList)
		{
			//座標
			Vec2 pos;
			if (item.type == U"pencil_lead")
			{
				pos = { GetXPos(item.pos) , Scene::Center().y };
			}
			else
			{
				pos = { GetXPos(item.pos) , Scene::Center().y - item.y * texture_size / 8 };
			}
			//耐久値の割合
			float damage_proportion = (float)item.durability / state.getGameUnitType(item).durability;
			//シルエット
			RenderTexture silhouette;
			//攻撃しているか
			if (item.cooldown > GameUnitTypeList.at(item.type).cooldown / 5 * 3)
			{
				//攻撃テクスチャーで描画
				state.getGameUnitType(item).getAttackingTexture(item.isFriend).drawAt(pos);
				silhouette = (item.isFriend ? state.getGameUnitType(item).friendAttackingMaskRenderTexture : state.getGameUnitType(item).enemyAttackingMaskRenderTexture);
			}
			else
			{
				//通常テクスチャーで描画
				state.getGameUnitType(item).getNormalTexture(item.isFriend).drawAt(pos);
				silhouette = (item.isFriend ? state.getGameUnitType(item).friendNormalMaskRenderTexture : state.getGameUnitType(item).enemyNormalMaskRenderTexture);
			}
			//割れ目の描画
			if (damage_proportion < 0.15)
			{
				Graphics2D::SetPSTexture(1, silhouette);
				// マルチテクスチャによるマスクのシェーダを開始
				const ScopedCustomShader2D shader{ maskShader };
				crack[2].drawAt(pos);
			}
			else if (damage_proportion < 0.3)
			{
				Graphics2D::SetPSTexture(1, silhouette);
				// マルチテクスチャによるマスクのシェーダを開始
				const ScopedCustomShader2D shader{ maskShader };
				crack[1].drawAt(pos);
			}
			else if (damage_proportion < 0.6)
			{
				Graphics2D::SetPSTexture(1, silhouette);
				// マルチテクスチャによるマスクのシェーダを開始
				const ScopedCustomShader2D shader{ maskShader };
				crack[0].drawAt(pos);
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
		//背景の描画
		{
			const uint16 line = Scene::Center().y + texture_size / 2;
			//土！！！！
			Rect{ 0, line, Scene::Width(), Scene::Height() - line }.draw(Color{ 175, 108, 53 });
			//草！！！！
			Rect{ 0, line, Scene::Width(), 25 }.draw(Palette::Forestgreen);
		}
		
		// 資金残高表記
		moneyInfoFont(U"手持ちのお金: " + Format(state.FriendCamp.money) + U"円").draw(0, Scene::Height() - texture_size - 40);
		
		// マウスオーバーされた召喚ボタン
		String MouseOveredButton = U"";
		//召喚ボタンの描画
		for (const auto& item : summonButtonList)
		{
			//背景と枠線の描画
			item.background.draw(get_config_value<int>(item.type, U"cost") > state.FriendCamp.money ? ColorF{ 0.4, 0.4, 0.4 } : item.background.mouseOver() ? ColorF{ 0.5, 0.5, 0.5 } : ColorF{ 0.7, 0.7, 0.7 }).drawFrame(3, 0, ColorF{ 0.3, 0.3, 0.3 });
			//画像の描画(* 0.4 = * 0.8 / 2)
			item.texture.resized(texture_size * 0.8).drawAt(item.x,Scene::Size().y - texture_size * 0.4);
			//ラベルの描画
			summonButtonFont(Format(GameUnitTypeList[item.type].cost) + U"円").drawAt(item.x,Scene::Size().y - texture_size * 0.875);
			if (item.background.mouseOver())
			{
				MouseOveredButton = item.type;
			}
		}
		
		// 資金力強化ボタン
		moneyButton.draw(120 * Math::Pow(2, state.FriendCamp.profitLevel) > state.FriendCamp.money ? static_cast<HSV>(Palette::Darkgoldenrod) : moneyButton.mouseOver() ? static_cast<HSV>(Palette::Goldenrod) : HSV{ 40, 0.8, 1.0 }).drawFrame(3, 0, Palette::Goldenrod);
		upgradeButtonFont( U"資金力強化に\n投資する\n" + Format(120 * Math::Pow(2, state.FriendCamp.profitLevel)) + U"円" ).drawAt(Scene::Width() - texture_size / 2, Scene::Height() - texture_size * 0.5);
		
		// 説明欄
		if (not (MouseOveredButton == U""))
		{
			Rect{0, 0, Scene::Width(), 150}.draw(ColorF(Palette::Dimgray, 0.8)).drawFrame(2, 0, Palette::Black);
			descriptionFont(GetStringValue(MouseOveredButton, U"description")).draw(texture_size + 30, 10);
			GameUnitTypeList[MouseOveredButton].getNormalTexture(true).draw(14, 2);
			nameFont(GetStringValue(MouseOveredButton, U"ja_name")).draw(2, texture_size + 2);
		}
		
	}
};



void Main()
{
	//ウィンドウサイズの設定
	Window::Resize(960,540);
	//ウィンドウタイトルの設定
	Window::SetTitle(U"Stationery Fight!");
	// シーンマネージャーを作成
	App manager;
	// シーンの登録
	manager.add<Title>(U"Title");
	manager.add<GameScene>(U"GameScene");
	manager.add<RuleExplanation>(U"RuleExplanation");
	manager.add<DifficultySetting>(U"DifficultySetting");
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetStyle(WindowStyle::Sizable);
	//オーディオアセットの登録
	AudioAsset::Register(U"choose", Resource(U"resource/sound/choose.ogg"));
	//コンフィグのロード
	ConfigJson = JSON::Load(Resource(U"resource/config.json"));
	if (not ConfigJson)
	{
		throw Error{U"Error! Failed to load a config file."};
	}
	//毎フレームごとの処理
	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
