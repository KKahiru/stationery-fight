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
	Font titleFont{ 100, Typeface::Bold };
	ButtonUI playButton{ Vec2{ Scene::Center().x, Scene::Center().y - 20 }, U"プレイ！", 45, 500 };
	ButtonUI ruleButton{ Vec2{ Scene::Center().x, Scene::Center().y + 120 }, U"遊び方・ルール", 45, 500 };
	ButtonUI exitButton{ Vec2{ Scene::Center().x, Scene::Center().y + 260 }, U"終了", 45, 500 };
	// ミュート関連
	const uint8 iconSize = 60;
	const uint8 iconMargin = iconSize / 2 + 20;
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
