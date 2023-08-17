//
//  DifficultySetting.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

#include "DifficultySetting.hpp"

// コンストラクタ
DifficultySetting::DifficultySetting(const InitData& init)
: IScene{ init }
{

}
// 更新関数
void DifficultySetting::update()
{
	if (EasyButton.leftPressed())
	{
		getData().DifficultyLevel = 1;
		AudioAsset(U"Choose").playOneShot();
		changeScene(U"Game");
	}
	else if (NormalButton.leftPressed())
	{
		getData().DifficultyLevel = 2;
		AudioAsset(U"Choose").playOneShot();
		changeScene(U"Game");
	}
	else if (HardButton.leftPressed())
	{
		getData().DifficultyLevel = 3;
		AudioAsset(U"Choose").playOneShot();
		changeScene(U"Game");
	}
}

// 描画関数
void DifficultySetting::draw() const
{
	GuideFont(U"難易度を選択してください").drawAt(Scene::Center().x, 100);
	EasyButton.draw();
	NormalButton.draw();
	HardButton.draw();
	
}
