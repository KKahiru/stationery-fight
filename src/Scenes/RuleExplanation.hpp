//
//  RuleExplanation.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# ifndef RuleExplanation_hpp
# define RuleExplanation_hpp

# include "../Common.hpp"

class RuleExplanation : public App::Scene
{
	Font ViewFont{ 35 };
	// ページ番号
	uint8 currentPage = 0;
	// 画像
	Array<Texture> imageData = {
		Texture{Resource(U"resource/tutorial/normal.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/castle.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/money.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/money_button.png"), TextureDesc::Mipped},
		Texture{Resource(U"resource/tutorial/fixing.png"), TextureDesc::Mipped}
	};
	// 説明
	Array<String> pageData = {
		U"・ このゲームの基本\nたまったお金で文房具を召喚して、敵の文房具を倒し、\n最終的に相手の「城」を陥落させたら勝ちです。",
		U"・ 「城」について\n右の青い城が自分で、左の赤い城が敵です。\n城の上のバーを見れば、城が受けたダメージがわかります。",
		U"・ 文房具の召喚方法\n文房具を呼び出すには、下のボタンを押して表示された金額を払います。\n手持ちのお金はボタンの上に表示されています。",
		U"・ 攻略の要—投資ボタンについて\n投資ボタン（右下の黄色いボタン）をクリックすることで\nお金が貯まる速度を早めることができます。",
		U"・ 固定機能について\n自分の文房具を左クリックするとその場に固定し、\n右クリックすると固定を解除することもできます。"
	};
	
	ButtonUI playGameButton{ Vec2{ Scene::Width() - 140, 60 }, U"ゲームを始める", 25, 235 };
	ButtonUI goBackButton{ Vec2{ 140, 60 }, U"タイトル画面に戻る", 25, 235 };
	// 次ページボタン
	ButtonUI nextButton{ Vec2{ Scene::Center().x + 160, Scene::Center().y + 300 }, U">", 30 };
	// 前ページボタン
	ButtonUI previousButton{ Vec2{ Scene::Center().x - 160, Scene::Center().y + 300 }, U"<", 30 };
	
	public:
	// コンストラクタ
	RuleExplanation(const InitData& init);
	
	// 更新関数
	void update() override;
	
	// 描画関数
	void draw() const override;
};

# endif /* RuleExplanation_hpp */
