//
//  Common.cpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/16.
//

# include "Common.hpp"

ButtonUI::ButtonUI(Vec2 pos, String label, double fontSize, Color fill, Color mouseOveredFill, Color labelColor, Color border)
	: buttonFont(fontSize), pos(pos), label(label), fill(fill), mouseOveredFill(mouseOveredFill), labelColor(labelColor), border(border)
{
	background = buttonFont(label).region(Arg::center = pos).stretched(20, 5);
}
ButtonUI::ButtonUI(Vec2 pos, String label, double fontSize, double width, Color fill, Color mouseOveredFill, Color labelColor, Color border)
	: buttonFont(fontSize), pos(pos), label(label), fill(fill), mouseOveredFill(mouseOveredFill), labelColor(labelColor), border(border)
{
	RectF textBackground = buttonFont(label).region(Arg::center = pos);
	background = RectF{ Arg::center = textBackground.center(), width, textBackground.h + 5 };
}

void ButtonUI::draw()
{
	DrawableText text = buttonFont(label);
	// 背景の領域を取得
	background = text.region(Arg::center = pos).stretched(20, 5);
	// 背景とボーダーを描画
	background.draw(background.mouseOver() ? mouseOveredFill : fill)
	.drawFrame(1, border);
	// 文字の描画
	background = text.drawAt(pos, labelColor);
}

void ButtonUI::draw() const
{
	// 背景とボーダーを描画
	background.draw(background.mouseOver() ? mouseOveredFill : fill)
	.drawFrame(1, border);
	// 文字の描画
	buttonFont(label).drawAt(pos, labelColor);
}

bool ButtonUI::leftPressed() const
{
	return background.leftReleased();
}
