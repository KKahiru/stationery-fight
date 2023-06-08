//
//  Common.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/16.
//

# ifndef Common_hpp
# define Common_hpp

# include <Siv3D.hpp> // OpenSiv3D v0.6.8

# include "logic.hpp"
# include "player.hpp"


using namespace stfi;

// 描画時のテクスチャサイズ
constexpr int TextureSize = 140;
// コンフィグ
inline JSON ConfigJson;

struct GameData
{
	// 音楽がミュートされているか
	bool IsMusicMuted = false;
	// 難易度
	int DifficultyLevel = 2;
};

class ButtonUI
{
	
	Font buttonFont;
	
public:
	
	// ボタンの位置
	Vec2 pos;
	
	// ラベル
	String label;
	
	// 背景色
	Color fill;
	
	// マウスオーバー時の背景色
	Color mouseOveredFill;
	
	// ラベルの色
	Color labelColor;
	
	// ボーダーの色
	Color border;
	
	// 背景の領域
	RectF background;
	
	ButtonUI(Vec2 pos, String label, double fontSize = 45, Color fill = Palette::Azure, Color mouseOveredFill = Palette::Cadetblue, Color labelColor = Palette::Black, Color border = Palette::Black);
	
	ButtonUI(Vec2 pos, String label, double fontSize, double width, Color fill = Palette::Azure, Color mouseOveredFill = Palette::Cadetblue, Color labelColor = Palette::Black, Color border = Palette::Black);
	
	// 描画を行う関数
	void draw();
	
	// 描画を行う関数
	void draw() const;
	
	// 左クリックで押されたかを返す関数
	bool leftPressed() const;
};

// シーンマネージャーをAppに割り当てる
using App = SceneManager<String, GameData>;

# endif /* Common_hpp */
