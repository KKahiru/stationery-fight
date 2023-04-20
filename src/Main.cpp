# include <Siv3D.hpp> // OpenSiv3D v0.6.8
# include "logic.hpp"

using namespace stfi;

const int texture_size = 120;
//コンフィグ
JSON ConfigJson;
//コンフィグの値を取得する関数
template <class Type>
Type get_config_value(String type, String key){
    return ConfigJson[U"ability_config"][type][key].get<Type>();
}

String GetStringValue (String type, String key) {
    return ConfigJson[U"ability_config"][type][key].getString();
}

uint16 GetXPos (const double& pos)
{
    return pos * Scene::Width();
}
//難易度
int difficult_level = 2;
//シーンマネージャーをAppに割り当てる
using App = SceneManager<String>;
// タイトルシーン
class Title : public App::Scene
{
    Font title_font{90, Typeface::Bold};
    Font button_font{35};
    Point play_button_vec{Scene::Center().x, Scene::Center().y - 10};
    Rect play_button{Arg::center(play_button_vec), 320, 60};
    Point rule_button_vec{Scene::Center().x, Scene::Center().y + 90};
    Rect rule_button{Arg::center(rule_button_vec), 320, 60};

    public:
        // コンストラクタ（必ず実装）
        Title(const InitData& init)
        : IScene{ init }
        {
            //背景を青くする
            Scene::SetBackground(Palette::Skyblue);
        }
        
        // 更新関数（オプション）
        void update() override
        {
            if (play_button.leftClicked()) {
                AudioAsset(U"choose").playOneShot();
                changeScene(U"DifficultySetting");
            } else if (rule_button.leftClicked()){
                AudioAsset(U"choose").playOneShot();
                changeScene(U"RuleExplanation");
            }
        }
        
        // 描画関数（オプション）
        void draw() const override
        {
            
            title_font(U"Stationery Fight!").drawAt(Scene::Center().x, Scene::Center().y - 190, Palette::Dodgerblue);
            //プレイボタン
            play_button.draw(play_button.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
            button_font(U"プレイ！").drawAt(play_button_vec,Palette::Black);
            //ルールボタン
            rule_button.draw(rule_button.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
            button_font(U"遊び方・ルール").drawAt(rule_button_vec,Palette::Black);
        }
};

class RuleExplanation : public App::Scene {
    Font ViewFont{30};
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
            if (SimpleGUI::Button(U"<", Vec2{Scene::Center().x - 150, 485}) and page_num != 1) {
                page_num --;
            }
            if (SimpleGUI::Button(U">", Vec2{Scene::Center().x + 100, 485}) and page_num < page_data.size()) {
                page_num ++;
            }
            if (SimpleGUI::Button(U"タイトル画面に戻る", Vec2{10, 50})) {
                AudioAsset(U"choose").playOneShot();
                changeScene(U"Title");
            }
            if (SimpleGUI::Button(U"ゲームを始める", Vec2{Scene::Width() - 210, 50})){
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
class DifficultySetting : public App::Scene {
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
        DifficultySetting(const InitData& init)
        : IScene{ init }
        {

        }
        // 更新関数（オプション）
        void update() override
        {
            if (EasyButton.leftClicked()) {
                difficult_level = 1;
                AudioAsset(U"choose").playOneShot();
                changeScene(U"GameScene");
            } else if (NormalButton.leftClicked()){
                difficult_level = 2;
                AudioAsset(U"choose").playOneShot();
                changeScene(U"GameScene");
            } else if (HardButton.leftClicked()) {
                difficult_level = 3;
                AudioAsset(U"choose").playOneShot();
                changeScene(U"GameScene");
            }
        }
        
        // 描画関数（オプション）
        void draw() const override
        {
            GuideFont(U"難易度を選択してください").drawAt(Scene::Center().x, 100);
            EasyButton.draw(EasyButton.mouseOver() ? Palette::Green : Palette::Lightgreen).drawFrame(1, Palette::Black);
            ButtonFont(U"簡単").drawAt(EasyButtonPos,Palette::Black);
            NormalButton.draw(NormalButton.mouseOver() ? Palette::Cadetblue : Palette::Azure).drawFrame(1, Palette::Black);
            ButtonFont(U"ちょっと難しい（ノーマル）").drawAt(NormalButtonPos,Palette::Black);
            HardButton.draw(HardButton.mouseOver() ? Palette::Darkred : Palette::Orangered).drawFrame(1, Palette::Black);
            ButtonFont(U"ほぼ不可能").drawAt(HardButtonPos,Palette::Black);
            
        }
};

// ゲームシーン
class GameScene : public App::Scene
{
    //召喚ボタンの情報を格納する構造体
    struct summon_button{
        summon_button(String type, uint16 count){
            //種類の登録
            this->type = type;
            //ボタンのX座標
            this->x = texture_size * (count + 0.5) - 3 * count;
            this->texture = Texture{ Resource(U"resource/texture/" + type + U"/friend.png"), TextureDesc::Mipped };
            //ボタンの背景
            this->background = Rect{texture_size * count - 3 * count,Scene::Size().y - texture_size,texture_size,texture_size};
        };
        String type;
        Texture texture;
        uint16 x;
        Rect background;
        
    };
    // 蓄積された時間（秒）
    double actionAccumulator = 0.0;
    double incomeAccumulator = 0.0;
    //ひび割れ
    TextureRegion crack[3] = { Texture{ Resource(U"resource/texture/crack/crack-1.png"), TextureDesc::Mipped }.resized(texture_size),
        Texture{ Resource(U"resource/texture/crack/crack-2.png"), TextureDesc::Mipped }.resized(texture_size),
        Texture{ Resource(U"resource/texture/crack/crack-3.png"), TextureDesc::Mipped }.resized(texture_size) };
    //攻撃音
    const Audio HitPop{Resource(U"resource/sound/hit_pop_1.ogg")};
    //召喚音
    const Audio SummonSound{Resource(U"resource/sound/summon.ogg")};
    //資金力ボタンが押せるようになった時の音
    const Audio MoneyAvailable{Resource(U"resource/sound/money_available.mp3")};
    //BGM
    const Audio BGM{Audio::Stream, Resource(U"resource/sound/bgm.mp3"), Loop::Yes};
    //エフェクト
    Effect effect;
    //マスク用のシェーダー
    PixelShader mask_shader;
    //召喚ボタンのリスト
    Array<summon_button> summon_button_list;
    //召喚ボタンのラベルのフォント
    Font sumbtn_label_font{(int32)(texture_size * 0.15)};
    //資金力強化ボタンの背景
    Rect money_button{Scene::Size().x - texture_size, Scene::Size().y - texture_size, texture_size, texture_size};
    //資金力強化ボタンのラベルのフォント
    Font money_label_font{18};
    //資金情報
    Font money_info_label_font{22};
    //解説のフォント
    Font DescriptionFont{25};
    Font NameFont{20};
    
    GameState state;
    
    WeakAI AI;
    
    Array<GameUnit>& GameUnitList = state.GameUnitList;
    HashTable<String, GameUnitType>& GameUnitTypeList = state.GameUnitTypeList;
    
    //勝敗決定後の待機時間
    float waiting_time = 0;
    //「1ティック前に」資金力ボタンが押せたか
    bool WasMoneyButtonAvilable = false;
public:
    // コンストラクタ（必ず実装）
    GameScene(const InitData& init)
        : IScene{ init },
        state(ConfigJson)
    {
        Scene::SetBackground(Palette::Lightskyblue);
        mask_shader = HLSL{ Resource(U"resource/shader/hlsl/multi_texture_mask.hlsl"), U"PS" }
        | GLSL{ Resource(U"resource/shader/glsl/multi_texture_mask.frag"), {{U"PSConstants2D", 0}} };
        if (not mask_shader) {
            throw Error{U"Error! Failed to load a shader file."};
        }
        
        for( unsigned long int i=0; i <  ConfigJson[U"available_types"].size(); i++) {
            summon_button_list.push_back(summon_button(ConfigJson[U"available_types"][i].getString(), i));
        };
        BGM.play();
    }
    
    // 更新関数（オプション）
    void update() override
    {
        //蓄積された秒数の記録
        actionAccumulator += Scene::DeltaTime();
        incomeAccumulator += Scene::DeltaTime();
        //game_unitの描画・クリック時の処理
        for (unsigned long int i = 0; i < GameUnitList.size(); i++) {
            //自分の前に重なっているgame_unitのカウント数
            size_t overlapped_unit_count = 0;
            for (long int j = 0; j < distance(GameUnitList.begin(), GameUnitList.begin() + i); j++){
                if (GetXPos(GameUnitList[j].pos) < GetXPos(GameUnitList[i].pos) + texture_size/4*3
                    and GetXPos(GameUnitList[j].pos) > GetXPos(GameUnitList[i].pos) - texture_size/4*3
                    and GameUnitList[j].type != U"castle"
                    and GameUnitList[j].type != U"pencil_lead") {
                    //カウント
                    overlapped_unit_count++;
                }
            }
            //座標
            Vec2 pos;
            if (GameUnitList[i].type == U"pencil_lead"){
                pos = { GetXPos(GameUnitList[i].pos) , Scene::Center().y };
            } else {
                pos = { GetXPos(GameUnitList[i].pos) , Scene::Center().y - overlapped_unit_count * texture_size / 8 };
            }
            //耐久値の割合
            float damage_proportion = (float)GameUnitList[i].durability / state.getGameUnitType(GameUnitList[i]).durability;
            //シルエット
            RenderTexture silhouette;
            //攻撃しているか
            if (GameUnitList[i].cooldown > GameUnitTypeList.at(GameUnitList[i].type).cooldown / 5 * 3){
                //攻撃テクスチャーで描画
                state.getGameUnitType(GameUnitList[i]).getAttackingTexture(GameUnitList[i].isFriend).drawAt(pos);
                silhouette = (GameUnitList[i].isFriend ? state.getGameUnitType(GameUnitList[i]).friendAttackingMaskRenderTexture : state.getGameUnitType(GameUnitList[i]).enemyAttackingMaskRenderTexture);
            }
            else {
                //通常テクスチャーで描画
                state.getGameUnitType(GameUnitList[i]).getNormalTexture(GameUnitList[i].isFriend).drawAt(pos);
                silhouette = (GameUnitList[i].isFriend ? state.getGameUnitType(GameUnitList[i]).friendNormalMaskRenderTexture : state.getGameUnitType(GameUnitList[i]).enemyNormalMaskRenderTexture);
            }
            //割れ目の描画
            if (damage_proportion < 0.15){
                Graphics2D::SetPSTexture(1, silhouette);
                // マルチテクスチャによるマスクのシェーダを開始
                const ScopedCustomShader2D shader{ mask_shader };
                crack[2].drawAt(pos);
            } else if (damage_proportion < 0.3){
                Graphics2D::SetPSTexture(1, silhouette);
                // マルチテクスチャによるマスクのシェーダを開始
                const ScopedCustomShader2D shader{ mask_shader };
                crack[1].drawAt(pos);
                
            } else if (damage_proportion < 0.6){
                Graphics2D::SetPSTexture(1, silhouette);
                // マルチテクスチャによるマスクのシェーダを開始
                const ScopedCustomShader2D shader{ mask_shader };
                crack[0].drawAt(pos);
                
            }
            //クリック時の処理
            if (GameUnitList[i].isFriend and GameUnitList[i].type != U"pencil_lead"){
                if (GameUnitList[i].collisionDetection.leftClicked() and GameUnitList[i].isFriend) {
                    //左クリック時に（味方なら）固定
                    GameUnitList[i].isFixed = true;
                } else if(GameUnitList[i].collisionDetection.rightClicked() and GameUnitList[i].isFriend){
                    //右クリック時に（味方なら）固定解除
                    GameUnitList[i].isFixed = false;
                }
            }
            //当たり判定の図形の移動
            GameUnitList[i].collisionDetection.setPos(Arg::center(GetXPos(GameUnitList[i].pos), Scene::Center().y - overlapped_unit_count * texture_size / 8));
        }
        //敵を召喚する簡易ボタン
#ifdef DEBUG
        if (KeyShift.pressed()) {
                if (SimpleGUI::Button(U"敵鉛筆召喚", Vec2{ 0, 100 }))
            {
                state.summonGameUnit(GameUnitTypeList[U"pencil"],false);
            }
            if (SimpleGUI::Button(U"敵消しゴム召喚", Vec2{ 150, 100 }))
            {
                state.summonGameUnit(GameUnitTypeList[U"eraser"],false);
            }
            if (SimpleGUI::Button(U"敵三角定規召喚", Vec2{ 350, 100 }))
            {
                state.summonGameUnit(GameUnitTypeList[U"triangle"],false);
            }
        }
#endif
        //背景の描画
        {
            const uint16 line = Scene::Center().y + texture_size/2;
            //土！！！！
            Rect{ 0, line, Scene::Width(), Scene::Height() - line }.draw(Color{ 175, 108, 53 });
            //草！！！！
            Rect{ 0, line, Scene::Width(), 25 }.draw(Palette::Forestgreen);
        }
        //
        String MouseOveredButton = U"";
        //召喚ボタンの描画
        for (unsigned long int i = 0; i < summon_button_list.size(); i++){
            //背景と枠線の描画
            summon_button_list[i].background.draw(get_config_value<int>(summon_button_list[i].type, U"cost") > state.FriendCamp.money ? ColorF{ 0.4, 0.4, 0.4 } : summon_button_list[i].background.mouseOver() ? ColorF{ 0.5, 0.5, 0.5 } : ColorF{ 0.7, 0.7, 0.7 }).drawFrame(3, 0, ColorF{ 0.3, 0.3, 0.3 });
            //画像の描画(* 0.4 = * 0.8 / 2)
            summon_button_list[i].texture.resized(texture_size * 0.8).drawAt(summon_button_list[i].x,Scene::Size().y - texture_size * 0.4);
            //ラベルの描画
            sumbtn_label_font(Format(GameUnitTypeList[summon_button_list[i].type].cost) + U"円").drawAt(summon_button_list[i].x,Scene::Size().y - texture_size * 0.875);
            if (summon_button_list[i].background.mouseOver()) {
                MouseOveredButton = summon_button_list[i].type;
            }
            //ボタンがクリックされたら
            if (summon_button_list[i].background.leftClicked()) {
                //条件式を評価することでgame_unitを召喚する
                if (state.summonGameUnit(GameUnitTypeList[summon_button_list[i].type], true)){
                    SummonSound.playOneShot();
                }
            }
        }
        
        //資金力強化ボタン
        money_button.draw(120 * Math::Pow(2, state.FriendCamp.profitLevel) > state.FriendCamp.money ? static_cast<HSV>(Palette::Darkgoldenrod) : money_button.mouseOver() ? static_cast<HSV>(Palette::Goldenrod) : HSV{ 40, 0.8, 1.0 }).drawFrame(3, 0, Palette::Goldenrod);
        money_label_font( U"資金力強化に\n投資する\n" + Format(120 * Math::Pow(2, state.FriendCamp.profitLevel)) + U"円" ).drawAt(Scene::Width() - texture_size / 2, Scene::Height() - texture_size * 0.5);
        if (money_button.leftClicked() and state.FriendCamp.money >= 120 * Math::Pow(2, state.FriendCamp.profitLevel)) {
            state.FriendCamp.money -=  120 * Math::Pow(2, state.FriendCamp.profitLevel);
            state.FriendCamp.profitLevel ++;
            SummonSound.playOneShot();
        }
        if (state.FriendCamp.money >= 120 * Math::Pow(2, state.FriendCamp.profitLevel)) {
            if (not WasMoneyButtonAvilable) {
                MoneyAvailable.playOneShot();
                WasMoneyButtonAvilable = true;
                
            }
        } else {
            WasMoneyButtonAvilable = false;
        }
        
        //資金残高表記
        money_info_label_font(U"手持ちのお金: " + Format(state.FriendCamp.money) + U"円").draw(0, Scene::Height() - texture_size - 40);
        
        //説明欄
        if (not (MouseOveredButton == U"")) {
            Rect{0, 0, Scene::Width(), 150}.draw(ColorF(Palette::Dimgray, 0.8)).drawFrame(2, 0, Palette::Black);
            DescriptionFont(GetStringValue(MouseOveredButton, U"description")).draw(texture_size + 30, 10);
            GameUnitTypeList[MouseOveredButton].getNormalTexture(true).draw(14, 2);
            NameFont(GetStringValue(MouseOveredButton, U"ja_name")).draw(2, texture_size + 2);
        }
              
        //耐久力バーの描画
        if (state.winner == 0) {
            // TODO: remove hard coding
            float friend_castle_damage_proportion = (float)GameUnitList[0].durability / 1200;
            float enemy_castle_damage_proportion = (float)GameUnitList[1].durability / 1200;
            Color friend_castle_bar_color;
            Color enemy_castle_bar_color;
            if (friend_castle_damage_proportion > 0.6 ){
                friend_castle_bar_color = Palette::Greenyellow;
            } else if (friend_castle_damage_proportion > 0.3) {
                friend_castle_bar_color = Palette::Yellow;
            } else {
                friend_castle_bar_color = Palette::Red;
            }
            
            if (enemy_castle_damage_proportion > 0.6 ){
                enemy_castle_bar_color = Palette::Greenyellow;
            } else if (enemy_castle_damage_proportion > 0.3) {
                enemy_castle_bar_color = Palette::Yellow;
            } else {
                enemy_castle_bar_color = Palette::Red;
            }

            Line{10, Scene::Center().y - 100, 110, Scene::Center().y - 100}.draw(10, Palette::Darkgray);
            Line{10, Scene::Center().y - 100, 10 + 100 * friend_castle_damage_proportion, Scene::Center().y - 100}.draw(10, friend_castle_bar_color);
            Line{Scene::Width() - 110, Scene::Center().y - 100, Scene::Width() - 10, Scene::Center().y - 100}.draw(10, Palette::Darkgray);
            Line{Scene::Width() - 10 - 100 * enemy_castle_damage_proportion, Scene::Center().y - 100, Scene::Width() - 10, Scene::Center().y - 100}.draw(10, enemy_castle_bar_color);
        }
        
        //game_unitの行動処理（0.05秒刻み）
        while ( 0.05 <= actionAccumulator) {
            //デバック情報の削除
            ClearPrint();
            state.actionProcess(HitPop, effect);
            //AI
            if (state.winner == 0)
            {
                AI.judge(state);
            }
            //蓄積された秒数を減らす
            actionAccumulator -= 0.1;
        }
        //各陣営の収入処理
        while (0.2 <= incomeAccumulator) {
            //難易度に合わせて資金を増加させる
            switch (difficult_level) {
                case 1:
                    state.FriendCamp.money += 10 * Math::Pow(1.5, state.FriendCamp.profitLevel);
                    state.EnemyCamp.money += 7 * Math::Pow(1.5, state.EnemyCamp.profitLevel);
                    break;
                default:
                case 2:
                    state.FriendCamp.money += 10 * Math::Pow(1.5, state.FriendCamp.profitLevel);
                    state.EnemyCamp.money += 12 * Math::Pow(1.5, state.EnemyCamp.profitLevel);
                    break;
                case 3:
                    state.FriendCamp.money += 7 * Math::Pow(1.5, state.FriendCamp.profitLevel);
                    state.EnemyCamp.money += 12 * Math::Pow(1.5, state.EnemyCamp.profitLevel);
                    break;
            }
            incomeAccumulator -= 0.5;
            
        }
        waiting_time += state.winner != 0 ? Scene::DeltaTime() : 0;
        if (waiting_time >= 4) {
            changeScene(U"Title");
        }
        effect.update();
    }
    
    // 描画関数（オプション）
    void draw() const override
    {
        
        
        
        
    }
};



void Main(){
    //ウィンドウサイズの設定
    Window::Resize(960,540);
    //ウィンドウタイトルの設定
    Window::SetTitle(U"Stationery Fight!");
    // シーンマネージャーを作成
    App manager;
    // シーンの登録
    manager.add<Title>(U"Title");
    manager.add<GameScene>(U"GameScene");
    manager.add<RuleExplanation>(U"RuleExplanation");
    manager.add<DifficultySetting>(U"DifficultySetting");
    Scene::SetResizeMode(ResizeMode::Keep);
    Window::SetStyle(WindowStyle::Sizable);
    //オーディオアセットの登録
    AudioAsset::Register(U"choose", Resource(U"resource/sound/choose.ogg"));
    //コンフィグのロード
    ConfigJson = JSON::Load(Resource(U"resource/config.json"));
    if (not ConfigJson) {
        throw Error{U"Error! Failed to load a config file."};
    }
    //毎フレームごとの処理
    while (System::Update()) {
        if (not manager.update()){
            break;
        }
    }
}
