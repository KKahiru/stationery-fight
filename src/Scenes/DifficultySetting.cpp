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
	if (EasyButton.leftClicked())
	{
		getData().DifficultyLevel = 1;
		AudioAsset(U"choose").playOneShot();
		changeScene(U"Game");
	}
	else if (NormalButton.leftClicked())
	{
		getData().DifficultyLevel = 2;
		AudioAsset(U"choose").playOneShot();
		changeScene(U"Game");
	}
	else if (HardButton.leftClicked())
	{
		getData().DifficultyLevel = 3;
		AudioAsset(U"choose").playOneShot();
		changeScene(U"Game");
	}
}

// 描画関数
void DifficultySetting::draw() const
{
	GuideFont(U"難易度を選択してください").drawAt(Scene::Center().x, 100);
	EasyButton.draw(EasyButton.mouseOver() ? Palette::Green : Palette::Lightgreen).drawFrame(1, Palette::Black);
	ButtonFont(U"簡単").drawAt(EasyButtonPos,Palette::Black);
	NormalButton.draw(NormalButton.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
	ButtonFont(U"ちょっと難しい（ノーマル）").drawAt(NormalButtonPos,Palette::Black);
	HardButton.draw(HardButton.mouseOver() ? Palette::Darkred : Palette::Orangered).drawFrame(1, Palette::Black);
	ButtonFont(U"ほぼ不可能").drawAt(HardButtonPos,Palette::Black);
	
}
