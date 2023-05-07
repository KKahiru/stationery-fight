//
//  Title.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# include "Common.hpp"
# include "Title.hpp"

// タイトルシーン

// コンストラクタ
Title::Title(const InitData& init)
: IScene{ init }
{
	//背景を青くする
	Scene::SetBackground(Palette::Skyblue);
}

// 更新関数
void Title::update()
{
	if (playButton.leftClicked())
	{
		AudioAsset(U"choose").playOneShot();
		changeScene(U"DifficultySetting");
	}
	else if (ruleButton.leftClicked())
	{
		AudioAsset(U"choose").playOneShot();
		changeScene(U"RuleExplanation");
	}
}

// 描画関数
void Title::draw() const
{
	
	titleFont(U"Stationery Fight!").drawAt(Scene::Center().x, Scene::Center().y - 190, Palette::Dodgerblue);
	//プレイボタン
	playButton.draw(playButton.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
	buttonFont(U"プレイ！").drawAt(playButtonVec,Palette::Black);
	//ルールボタン
	ruleButton.draw(ruleButton.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
	buttonFont(U"遊び方・ルール").drawAt(ruleButtonVec,Palette::Black);
}
