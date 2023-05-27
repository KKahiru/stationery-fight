//
//  RuleExplanation.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# include "RuleExplanation.hpp"

// コンストラクタ
RuleExplanation::RuleExplanation(const InitData& init)
: IScene{ init }
{}

// 更新関数
void RuleExplanation::update()
{
	if (previousButton.leftPressed())
	{
		if (currentPage == 0)
		{
			currentPage = pageData.size() - 1;
		}
		else
		{
			currentPage --;
		}
		
	}
	
	if (nextButton.leftPressed())
	{
		if (currentPage < pageData.size() - 1)
		{
			currentPage ++;
		}
		else
		{
			currentPage = 0;
		}
	}
	
	if (playGameButton.leftPressed())
	{
		AudioAsset(U"choose").playOneShot();
		changeScene(U"DifficultySetting");
	}
	
	if (goBackButton.leftPressed())
	{
		AudioAsset(U"choose").playOneShot();
		changeScene(U"Title");
	}
}

void RuleExplanation::draw() const
{
	imageData[currentPage].resized(720, 405).drawAt(Scene::Center().x, Scene::Center().y - 130);
	ViewFont(pageData[currentPage]).drawAt(Scene::Center().x, Scene::Center().y + 170);
	ViewFont( Format(currentPage + 1) + U" / " + Format(pageData.size()) ).drawAt(Scene::Center().x, Scene::Center().y + 300);
	// ボタンの描画
	playGameButton.draw();
	goBackButton.draw();
	nextButton.draw();
	previousButton.draw();
}
