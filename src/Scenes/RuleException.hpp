//
//  RuleExplanation.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/05/07.
//

# ifndef RuleExplanation_hpp
# define RuleExplanation_hpp

# include "Common.hpp"

class RuleExplanation : public App::Scene
{
    Font ViewFont{ 30 };
    //ページ数(1から数える)
    uint8 page_num = 1;
    Array<Texture> image_data = {
        Texture{Resource(U"resource/tutorial/normal.png"), TextureDesc::Mipped},
        Texture{Resource(U"resource/tutorial/castle.png"), TextureDesc::Mipped},
        Texture{Resource(U"resource/tutorial/money.png"), TextureDesc::Mipped},
        Texture{Resource(U"resource/tutorial/summon_button.png"), TextureDesc::Mipped},
        Texture{Resource(U"resource/tutorial/money_button.png"), TextureDesc::Mipped},
        Texture{Resource(U"resource/tutorial/fixing.png"), TextureDesc::Mipped}
    };
    Array<String> page_data = {
        U"たまったお金で文房具を増やして、\n敵の文房具を倒し、最終的に\n相手の「城」を陥落させたら勝ちです。",
        U"右の青い城が自分で、左の赤い城が敵です。\n城の上のバーを見れば、城が受けたダメージがわかります。",
        U"表示された金額を払って、文房具を呼び出します。\n手持ちのお金はボタンの上に表示されています。",
        U"お金が貯まり、下の文房具ボタンの色が変わったら、\nその文房具を呼び出せます。",
        U"お金は時間経過で貯まりますが、\n投資ボタン（右下の黄色いボタン）をクリックすることで\n貯まる速度を早めることができます。",
        U"自分の文房具を左クリックすると\nその場に固定し、右クリックすると固定を\n解除することもできます。"
    };
    public:
        //コンストラクタ
        RuleExplanation(const InitData& init)
        : IScene{ init }
        {
            
        }
        // 更新関数（オプション）
        void update() override
        {
            if (SimpleGUI::Button(U"<", Vec2{Scene::Center().x - 150, 485}) and page_num != 1)
            {
                page_num --;
            }
            if (SimpleGUI::Button(U">", Vec2{Scene::Center().x + 100, 485}) and page_num < page_data.size())
            {
                page_num ++;
            }
            if (SimpleGUI::Button(U"タイトル画面に戻る", Vec2{10, 50}))
            {
                AudioAsset(U"choose").playOneShot();
                changeScene(U"Title");
            }
            if (SimpleGUI::Button(U"ゲームを始める", Vec2{Scene::Width() - 210, 50}))
            {
                AudioAsset(U"choose").playOneShot();
                changeScene(U"DifficultySetting");
            }
        }
        
        // 描画関数（オプション）
        void draw() const override
        {
            image_data[page_num - 1].resized(480, 270).drawAt(Scene::Center().x, Scene::Center().y - 100);
            ViewFont(page_data[page_num - 1]).drawAt(Scene::Center().x, Scene::Center().y + 100);
            ViewFont( Format(page_num) + U"/" + Format(page_data.size()) ).drawAt(Scene::Center().x, 500);
            
        }
};

# endif /* RuleExplanation_hpp */
