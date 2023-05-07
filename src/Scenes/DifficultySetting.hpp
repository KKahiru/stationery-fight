//
//  DifficultySetting.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# ifndef DifficultySetting_hpp
# define DifficultySetting_hpp

# include "Common.hpp"

// 難易度設定
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
	DifficultySetting(const InitData& init);
	
	void update() override;
	
	void draw() const override;
};

# endif /* DifficultySetting_hpp */
