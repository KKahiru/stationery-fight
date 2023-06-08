//
//  Game.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# ifndef Game_hpp
# define Game_hpp

# include "../Common.hpp"

// ゲームシーン
class Game : public App::Scene
{
	
	// 傾斜の角度
	static constexpr double slopeDegree = 0.125;
	//コンフィグの値を取得する関数
	template <class Type>
	Type GetConfigValue(String type, String key) const;
	
	String GetStringValue(String type, String key) const;
	
	double GetXPos(const double pos) const;
	
	double GetYPos(const double pos) const;
	
	double GetYOffset(const uint16 pos) const;
	
	//召喚ボタンの情報を格納する構造体
	struct summon_button
	{
		summon_button(String type, uint16 count)
		{
			//種類の登録
			this->type = type;
			//ボタンのX座標
			this->x = TextureSize * (count + 0.5) - 3 * count;
			this->texture = Texture{ Resource(U"resource/texture/" + type + U"/friend.png"), TextureDesc::Mipped };
			//ボタンの背景
			this->background = Rect{TextureSize * count - 3 * count,Scene::Size().y - TextureSize,TextureSize,TextureSize};
		};
		String type;
		Texture texture;
		uint16 x;
		Rect background;
		
	};
	// 蓄積された時間（秒）
	double actionAccumulator = 0.0;
	double incomeAccumulator = 0.0;
	// 1ティックの長さ（秒）
	const float actionTickLong = 0.2;
	const float incomeTickLong = 0.4;
	//ひび割れ
	TextureRegion crack[3] = { Texture{ Resource(U"resource/texture/crack/crack-1.png"), TextureDesc::Mipped }.resized(TextureSize),
		Texture{ Resource(U"resource/texture/crack/crack-2.png"), TextureDesc::Mipped }.resized(TextureSize),
		Texture{ Resource(U"resource/texture/crack/crack-3.png"), TextureDesc::Mipped }.resized(TextureSize) };
	//攻撃音
	const Audio HitPop{ Resource(U"resource/sound/hit_pop_1.ogg") };
	//召喚音
	const Audio SummonSound{ Resource(U"resource/sound/summon.ogg") };
	//資金力ボタンが押せるようになった時の音
	const Audio MoneyAvailable{ Resource(U"resource/sound/money_available.mp3") };
	//BGM
	const Audio BGM{ Audio::Stream, Resource(U"resource/sound/bgm.mp3"), Loop::Yes };
	// 土と草原のボーダー
	const uint16 border = Scene::Center().y * 1.25;
	// 経過時間（秒）
	double totalTime = 0;
	//エフェクト
	Effect effect;
	//マスク用のシェーダー
	PixelShader maskShader;
	//召喚ボタンのリスト
	Array<summon_button> summonButtonList;
	//召喚ボタンのラベルのフォント
	Font summonButtonFont{ (int32)(TextureSize * 0.15) };
	//資金力強化ボタンの背景
	Rect moneyButton{ Scene::Size().x - TextureSize, Scene::Size().y - TextureSize, TextureSize };
	//資金力強化ボタンのラベルのフォント
	Font upgradeButtonFont{ 22 };
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
	
	Game(const InitData& init);
	
	void update() override;
	
	void draw() const override;
};

# endif /* Game_hpp */
