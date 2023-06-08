//
//  Title.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

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
	if (playButton.leftPressed())
	{
		AudioAsset(U"choose").playOneShot();
		changeScene(U"DifficultySetting");
	}
	else if (ruleButton.leftPressed())
	{
		AudioAsset(U"choose").playOneShot();
		changeScene(U"RuleExplanation");
	}
	else if (exitButton.leftPressed())
	{
		AudioAsset(U"choose").playOneShot();
		System::Exit();
	}
	if (musicBack.leftClicked())
	{
		getData().IsMusicMuted = false;
	}
	else if (musicMuteBack.leftClicked())
	{
		getData().IsMusicMuted = true;
	}
}

// 描画関数
void Title::draw() const
{
	
	titleFont(U"Stationery Fight!").drawAt(Scene::Center().x, Scene::Center().y - 240, Palette::Dodgerblue);
	// プレイボタン
	playButton.draw();
	// ルールボタン
	ruleButton.draw();
	// 終了ボタン
	exitButton.draw();
	// ミュートボタン
	if (not getData().IsMusicMuted)
	{
		musicBack.draw(ColorF{ 0.2, 0.8, 0.2 });
	}
	else if (musicBack.mouseOver())
	{
		musicBack.draw(ColorF{ 0.7 });
	}
	else
	{
		musicBack.draw(ColorF{ 0.85 });
	}
	musicIcon.drawAt(Scene::Width() - iconMargin * 2 - iconSize, Scene::Height() - iconMargin);
	if (getData().IsMusicMuted)
	{
		musicMuteBack.draw(ColorF{ 0.2, 0.8, 0.2 });
	}
	else if (musicMuteBack.mouseOver())
	{
		musicMuteBack.draw(ColorF{ 0.7 });
	}
	else
	{
		musicMuteBack.draw(ColorF{ 0.85 });
	}
	musicMuteIcon.drawAt(Scene::Width() - iconMargin, Scene::Height() - iconMargin);
}
