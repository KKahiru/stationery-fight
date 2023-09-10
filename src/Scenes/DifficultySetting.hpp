//
//  DifficultySetting.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# ifndef DifficultySetting_hpp
# define DifficultySetting_hpp

# include "../Common.hpp"

// 難易度設定
class DifficultySetting : public App::Scene
{
	Font GuideFont{ 40 };
	ButtonUI EasyButton{ Vec2{ Scene::Center().x, Scene::Center().y - 100 }, U"イージー", 45, 500, Palette::Lightgreen, Palette::Green };
	ButtonUI NormalButton{ Vec2{ Scene::Center().x, Scene::Center().y + 40 }, U"やや難", 45, 500, Palette::Azure ,Palette::Cadetblue };
# ifdef DEBUG
	ButtonUI HardButton{ Vec2{ Scene::Center().x, Scene::Center().y + 180 }, U"狂気のSATAケーブル", 45, 500, Palette::Orangered, Palette::Darkred };
# else
	ButtonUI HardButton{ Vec2{ Scene::Center().x, Scene::Center().y + 180 }, U"不可能", 45, 500, Palette::Orangered, Palette::Darkred };
#endif
	
public:
	//コンストラクタ
	DifficultySetting(const InitData& init);
	
	void update() override;
	
	void draw() const override;
};

# endif /* DifficultySetting_hpp */
