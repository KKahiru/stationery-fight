//
//  Title.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# ifndef Title_hpp
# define Title_hpp

# include "Common.hpp"

// タイトルシーン
class Title : public App::Scene
{
	Font titleFont{ 90, Typeface::Bold };
	Font buttonFont{ 35 };
	Point playButtonVec{ Scene::Center().x, Scene::Center().y - 10 };
	Rect playButton{ Arg::center(playButtonVec), 320, 60 };
	Point ruleButtonVec{ Scene::Center().x, Scene::Center().y + 90 };
	Rect ruleButton{ Arg::center(ruleButtonVec), 320, 60 };
	// ミュート関連
	const uint8 iconSize = 40;
	const uint8 iconMargin = iconSize / 2 + 10;
	const Texture musicIcon{ 0xf075a_icon, iconSize };
	const Circle musicBack{ Arg::center(Scene::Width() - iconMargin * 2 - iconSize, Scene::Height() - iconMargin), iconSize * 0.625 };
	const Circle musicMuteBack{ Arg::center(Scene::Width() - iconMargin, Scene::Height() - iconMargin), iconSize * 0.625 };
	const Texture musicMuteIcon{ 0xf075b_icon, iconSize };
public:
	Title(const InitData& init);
	// 更新関数
	void update() override;
	// 描画関数
	void draw() const override;
};

# endif /* Title_hpp */
