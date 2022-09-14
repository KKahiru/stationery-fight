# include <Siv3D.hpp>
#include <iostream>
using namespace std;
//MacでSceneManagerを使うための定義
#ifdef TARGET_OS_MAC
bool s3d::operator==(s3d::String const& left, s3d::String const& right) noexcept {
    return left.narrow() == right.narrow();
}
#endif

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
    //game_unitのクラス
    class game_unit{
    public:
        //味方かのbool値
        bool is_friend;
        //game_unitのタイプ
        String type;
        //x座標
        uint16 x;
        //耐久値
        int16 durability;
        //クールダウン
        uint16 cooldown = 0;
        //ノックバック
        uint16 knock_back = 0;
        //固定されているか
        bool is_fixed = false;
        //コンストラクター
        game_unit(String type, bool is_friend){
            if (is_friend){
                this->x = 50;
            } else{
                this->x = Scene::Size().x - 50;
            }
            this->type = type;
            this->is_friend = is_friend;
            this->durability = get_config_value<uint16>(type, U"durability");
        };
        game_unit(String type, bool is_friend, uint16 x){
            this->x = x;
            this->type = type;
            this->is_friend = is_friend;
            this->durability = get_config_value<uint16>(type, U"durability");
        }
        //当たり判定
        Rect collision_detection{120,120};
    };
    //プレイヤーの情報を格納する構造体
    struct player_info{
        uint16 money = 0;
        uint16 money_level = 0;
        uint16 game_unit_power;
    };
    //プレイヤーと敵の情報生成
    player_info player;
    player_info enemy;
    //ステージ上のgame_unitのリスト
    Array<game_unit> game_unit_list;
    //game_unitを召喚する関数
    bool summon_game_unit(String type, bool is_friend){
        if ((is_friend ? player.money : enemy.money) >= get_config_value<uint16>(type, U"cost")){
            game_unit_list.push_back(game_unit(type,is_friend));
            (is_friend ? player.money : enemy.money) -= get_config_value<uint16>(type, U"cost");
        } else {
            return false;
        }
        return true;
    }
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
    //game_unitの情報を格納する構造体
    struct game_unit_info {
        //日本語名
        String ja_name;
        //特性
        String feature;
        //耐久力
        uint16 durability;
        //攻撃力
        uint16 attack_power;
        //移動速度
        uint16 speed;
        //攻撃時にセットするクールダウン
        uint16 cooldown;
        //攻撃範囲の始まり
        uint16 attack_range_begin;
        //攻撃範囲の終わり
        uint16 attack_range_end;
        //コスト
        uint16 cost;
        //通常時テクスチャー
        Texture friend_texture;
        //攻撃時テクスチャー
        Texture friend_attack_texture;
        //通常時テクスチャー
        Texture enemy_texture;
        //攻撃時テクスチャー
        Texture enemy_attack_texture;
        //通常時テクスチャの形のマスクのレンダーテクスチャ
        RenderTexture friend_normal_mask_render_texture{ texture_size, texture_size };
        RenderTexture enemy_normal_mask_render_texture{ texture_size, texture_size };
        //攻撃時テクスチャの形のマスクのレンダーテクスチャ
        RenderTexture friend_attack_mask_render_texture{ texture_size, texture_size };
        RenderTexture enemy_attack_mask_render_texture{ texture_size, texture_size };
        //総合的な攻撃の強さ
        uint16 general_attack_power;
        //通常時テクスチャをtexture_sizeにリサイズして返す関数
        TextureRegion get_normal_texture(bool is_friend){
            return is_friend ? this->friend_texture.resized(texture_size) : this->enemy_texture.resized(texture_size);
        }
        //攻撃時テクスチャをtexture_sizeにリサイズして返す関数
        TextureRegion get_attack_texture(bool is_friend){
            return is_friend ? this->friend_attack_texture.resized(texture_size) : this->enemy_attack_texture.resized(texture_size);
        }
        //コンストラクター
        game_unit_info(String type){
            this->ja_name = GetStringValue(type, U"ja_name");
            this->feature = GetStringValue(type, U"feature");
            this->durability = get_config_value<uint16>(type, U"durability");
            this->attack_power = get_config_value<uint16>(type, U"attack_power");
            this->speed = get_config_value<uint16>(type, U"speed");
            this->cooldown = get_config_value<uint16>(type, U"cooldown");
            this->attack_range_begin = get_config_value<uint16>(type, U"attack_range_begin");
            this->attack_range_end = get_config_value<uint16>(type, U"attack_range_end");
            this->cost = get_config_value<uint16>(type, U"cost");
            this->friend_texture = Texture{ Resource(U"resource/texture/" + type + U"/friend.png"), TextureDesc::Mipped };
            this->friend_attack_texture = Texture{ Resource(U"resource/texture/" + type + U"/friend_attack.png"), TextureDesc::Mipped };
            this->enemy_texture = Texture{ Resource(U"resource/texture/" + type + U"/enemy.png"), TextureDesc::Mipped };
            this->enemy_attack_texture = Texture{ Resource(U"resource/texture/" + type + U"/enemy_attack.png"), TextureDesc::Mipped };
            //攻撃の総合的な強さ
            this->general_attack_power = get_config_value<double>(type, U"attack_power") / get_config_value<double>(type, U"cooldown") * (get_config_value<double>(type, U"attack_range_end") - get_config_value<double>(type, U"attack_range_begin"));
            // レンダーテクスチャをクリア
            friend_normal_mask_render_texture.clear(ColorF{ 0.0, 1.0 });
            friend_attack_mask_render_texture.clear(ColorF{ 0.0, 1.0 });
            enemy_normal_mask_render_texture.clear(ColorF{ 0.0, 1.0 });
            enemy_attack_mask_render_texture.clear(ColorF{ 0.0, 1.0 });
            {
                // レンダーターゲットをnormal_mask_render_texture.clearに設定
                const ScopedRenderTarget2D target{ friend_normal_mask_render_texture };
                Texture{ Resource(U"resource/texture/" + type + U"/friend_silhouette.png"), TextureDesc::Mipped }.resized(texture_size).drawAt(friend_normal_mask_render_texture.size() / 2);
            }
            {
                // レンダーターゲットをattack_mask_render_textureに設定
                const ScopedRenderTarget2D target{ friend_attack_mask_render_texture };
                Texture{ Resource(U"resource/texture/" + type + U"/friend_attack_silhouette.png"), TextureDesc::Mipped }.resized(texture_size).drawAt(friend_attack_mask_render_texture.size() / 2);
            }
            {
                // レンダーターゲットをnormal_mask_render_texture.clearに設定
                const ScopedRenderTarget2D target{ enemy_normal_mask_render_texture };
                Texture{ Resource(U"resource/texture/" + type + U"/enemy_silhouette.png"), TextureDesc::Mipped }.resized(texture_size).drawAt(enemy_normal_mask_render_texture.size() / 2);
            }
            {
                // レンダーターゲットをattack_mask_render_textureに設定
                const ScopedRenderTarget2D target{ enemy_attack_mask_render_texture };
                Texture{ Resource(U"resource/texture/" + type + U"/enemy_attack_silhouette.png"), TextureDesc::Mipped }.resized(texture_size).drawAt(enemy_attack_mask_render_texture.size() / 2);
            }
            
        };
    };
    //ゲーム終了時のエフェクト
    struct FinishEffect : IEffect
    {
        //ゲーム終了時のエフェクトのフォント
        Font finish_font{120};
        bool is_player_won;
        
        // このコンストラクタ引数が、Effect::add<FinishEffect>() の引数になる
        explicit FinishEffect(const bool input)
        : is_player_won{input}{}
        
        bool update(double t) override
        {
            //描画
            finish_font(is_player_won ? U"You Win!!" : U"You Lose...").drawAt(Scene::Center(),is_player_won ? ColorF{0.9, 0.9, 0} :  ColorF{0, 0.1, 0.9});
            // 3 秒未満なら継続
            return (t < 3.0);
        }
    };
    //game_unit_infoの連装配列
    unordered_map<string, game_unit_info> game_unit_info_list = {};
    //game_unitからgame_unit_infoを取得する関数
    game_unit_info get_game_unit_info(game_unit& target){
        return game_unit_info_list.at(target.type.narrow());
    }
    // 蓄積された時間（秒）
    double action_accumulator = 0.0;
    double income_accumulator = 0.0;
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
    //勝敗を記録する変数
    uint8 winner = 0;
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
    //勝敗決定後の待機時間
    float waiting_time = 0;
    //敵AIの変数
    //敵AIの攻撃のモード
    uint8 AIMode = 0;
    //敵AIの進行度
    uint16 AIStep = 0;
    //敵AIが召喚するターゲット
    String AITarget = U"";
    //AIの貯金
    uint16 AISavingMoney = 0;
    //「1ティック前に」資金力ボタンが押せたか
    bool WasMoneyButtonAvilable = false;
    void AISaveMoney(){
        if (AISavingMoney < (AIStep + 1) * 25 * Math::Pow(2, enemy.money_level) ) {
            AISavingMoney += enemy.money;
            enemy.money = 0;
        }
        return;
    }
public:
    // コンストラクタ（必ず実装）
    GameScene(const InitData& init)
    : IScene{ init }
    {
        Scene::SetBackground(Palette::Lightskyblue);
        mask_shader = HLSL{ Resource(U"resource/shader/hlsl/multi_texture_mask.hlsl"), U"PS" }
        | GLSL{ Resource(U"resource/shader/glsl/multi_texture_mask.frag"), {{U"PSConstants2D", 0}} };
        if (not mask_shader) {
            throw Error{U"Error! Failed to load a shader file."};
        }
        //召喚ボタン・ユニットの情報の登録
        game_unit_info_list.emplace("castle", game_unit_info(U"castle"));
        game_unit_info_list.emplace("pencil_lead", game_unit_info(U"pencil_lead"));
        for( unsigned long int i=0; i <  ConfigJson[U"all_types"].size(); i++) {
            String type = ConfigJson[U"all_types"][i].getString();
            summon_button_list.push_back(summon_button(ConfigJson[U"all_types"][i].getString(), i));
            game_unit_info_list.emplace(ConfigJson[U"all_types"][i].getString().narrow(), game_unit_info(type));
        };
        //城を造る
        game_unit_list.push_back(game_unit(U"castle", true, 40));
        game_unit_list.push_back(game_unit(U"castle", false, Scene::Width() - 40));
        BGM.play();
    }
    
    // 更新関数（オプション）
    void update() override
    {
        //蓄積された秒数の記録
        action_accumulator += Scene::DeltaTime();
        income_accumulator += Scene::DeltaTime();
        //game_unitの描画・クリック時の処理
        for (unsigned long int i = 0; i < game_unit_list.size(); i++) {
            //自分の前に重なっているgame_unitのカウント数
            int overlapped_unit_count = 0;
            for (long int j = 0; j < distance(game_unit_list.begin(), game_unit_list.begin() + i); j++){
                if (game_unit_list[j].x < game_unit_list[i].x + texture_size/4*3 and game_unit_list[j].x > game_unit_list[i].x - texture_size/4*3 and game_unit_list[j].type.narrow() != "castle" and game_unit_list[j].type.narrow() != "pencil_lead") {
                    //カウント
                    overlapped_unit_count++;
                }
            }
            //座標
            Vec2 pos;
            if (game_unit_list[i].type == U"pencil_lead"){
                pos = { game_unit_list[i].x , Scene::Center().y };
            } else {
                pos = { game_unit_list[i].x , Scene::Center().y - overlapped_unit_count * texture_size / 8 };
            }
            //耐久値の割合
            float damage_proportion = (float)game_unit_list[i].durability / get_game_unit_info(game_unit_list[i]).durability;
            //シルエット
            RenderTexture silhouette;
            //攻撃しているか
            if (game_unit_list[i].cooldown > game_unit_info_list.at(game_unit_list[i].type.narrow()).cooldown / 5 * 3){
                //攻撃テクスチャーで描画
                get_game_unit_info(game_unit_list[i]).get_attack_texture(game_unit_list[i].is_friend).drawAt(pos);
                silhouette = (game_unit_list[i].is_friend ? get_game_unit_info(game_unit_list[i]).friend_attack_mask_render_texture : get_game_unit_info(game_unit_list[i]).enemy_attack_mask_render_texture);
            }
            else {
                //通常テクスチャーで描画
                get_game_unit_info(game_unit_list[i]).get_normal_texture(game_unit_list[i].is_friend).drawAt(pos);
                silhouette = (game_unit_list[i].is_friend ? get_game_unit_info(game_unit_list[i]).friend_normal_mask_render_texture : get_game_unit_info(game_unit_list[i]).enemy_normal_mask_render_texture);
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
            if (game_unit_list[i].is_friend and game_unit_list[i].type.narrow() != "pencil_lead"){
                if (game_unit_list[i].collision_detection.leftClicked() and game_unit_list[i].is_friend) {
                    //左クリック時に（味方なら）固定
                    game_unit_list[i].is_fixed = true;
                } else if(game_unit_list[i].collision_detection.rightClicked() and game_unit_list[i].is_friend){
                    //右クリック時に（味方なら）固定解除
                    game_unit_list[i].is_fixed = false;
                }
            }
            //当たり判定の図形の移動
            game_unit_list[i].collision_detection.setPos(Arg::center(game_unit_list[i].x, Scene::Center().y - overlapped_unit_count * texture_size / 8));
        }
        //敵を召喚する簡易ボタン
#ifdef DEBUG
        if (KeyShift.pressed()) {
                if (SimpleGUI::Button(U"敵鉛筆召喚", Vec2{ 0, 100 }))
            {
                summon_game_unit(U"pencil",false);
            }
            if (SimpleGUI::Button(U"敵消しゴム召喚", Vec2{ 150, 100 }))
            {
                summon_game_unit(U"eraser",false);
            }
            if (SimpleGUI::Button(U"敵三角定規召喚", Vec2{ 350, 100 }))
            {
                summon_game_unit(U"triangle",false);
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
            summon_button_list[i].background.draw(get_config_value<int>(summon_button_list[i].type, U"cost") > player.money ? ColorF{ 0.4, 0.4, 0.4 } : summon_button_list[i].background.mouseOver() ? ColorF{ 0.5, 0.5, 0.5 } : ColorF{ 0.7, 0.7, 0.7 }).drawFrame(3, 0, ColorF{ 0.3, 0.3, 0.3 });
            //画像の描画(* 0.4 = * 0.8 / 2)
            summon_button_list[i].texture.resized(texture_size * 0.8).drawAt(summon_button_list[i].x,Scene::Size().y - texture_size * 0.4);
            //ラベルの描画
            sumbtn_label_font(Format(game_unit_info_list.at(summon_button_list[i].type.narrow()).cost) + U"円").drawAt(summon_button_list[i].x,Scene::Size().y - texture_size * 0.875);
            if (summon_button_list[i].background.mouseOver()) {
                MouseOveredButton = summon_button_list[i].type;
            }
            //ボタンがクリックされたら
            if (summon_button_list[i].background.leftClicked()) {
                //条件式を評価することでgame_unitを召喚する
                if (summon_game_unit(summon_button_list[i].type, true)){
                    SummonSound.playOneShot();
                }
            }
        }
        
        //資金力強化ボタン
        money_button.draw(120 * Math::Pow(2, player.money_level) > player.money ? static_cast<HSV>(Palette::Darkgoldenrod) : money_button.mouseOver() ? static_cast<HSV>(Palette::Goldenrod) : HSV{ 40, 0.8, 1.0 }).drawFrame(3, 0, Palette::Goldenrod);
        money_label_font( U"資金力強化に\n投資する\n" + Format(120 * Math::Pow(2, player.money_level)) + U"円" ).drawAt(Scene::Width() - texture_size / 2, Scene::Height() - texture_size * 0.5);
        if (money_button.leftClicked() and player.money >= 120 * Math::Pow(2, player.money_level)) {
            player.money -=  120 * Math::Pow(2, player.money_level);
            player.money_level ++;
            SummonSound.playOneShot();
        }
        if (player.money >= 120 * Math::Pow(2, player.money_level)) {
            if (not WasMoneyButtonAvilable) {
                MoneyAvailable.playOneShot();
                WasMoneyButtonAvilable = true;
                
            }
        } else {
            WasMoneyButtonAvilable = false;
        }
        
        //資金残高表記
        money_info_label_font(U"手持ちのお金: " + Format(player.money) + U"円").draw(0, Scene::Height() - texture_size - 40);
        
        //説明欄
        if (not (MouseOveredButton == U"")) {
            Rect{0, 0, Scene::Width(), 150}.draw(ColorF(Palette::Dimgray, 0.8)).drawFrame(2, 0, Palette::Black);
            DescriptionFont(GetStringValue(MouseOveredButton, U"description")).draw(texture_size + 30, 10);
            game_unit_info_list.at(MouseOveredButton.narrow()).get_normal_texture(true).draw(14, 2);
            NameFont(GetStringValue(MouseOveredButton, U"ja_name")).draw(2, texture_size + 2);
        }
              
        //耐久力バーの描画
        if (winner == 0) {
            // hard coding
            float friend_castle_damage_proportion = (float)game_unit_list[0].durability / 1200;
            float enemy_castle_damage_proportion = (float)game_unit_list[1].durability / 1200;
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
        
        //game_unitの行動処理（0.1秒刻み）
        while ( 0.1 <= action_accumulator) {
            //デバック情報の削除
            ClearPrint();
            //game_unitの行動処理
            for (unsigned long int i = 0; i < game_unit_list.size(); i++) {
                //クールダウンが終わったかの判定
                if (game_unit_list[i].cooldown == 0 and (game_unit_list[i].knock_back == 0 or (game_unit_list[i].type.narrow() == "castle" or game_unit_list[i].type.narrow() == "pencil_sharpener" ))){
                    //攻撃したかのフラグ
                    bool has_attacked = false;
                    game_unit* target = nullptr;
                    //攻撃するgame_unitの検索
                    for (unsigned long int j = 0; j < game_unit_list.size(); j++) {
                        //相手が敵で、攻撃範囲内にいるかを判別する
                        if (( game_unit_list[i].is_friend
                             ? not game_unit_list[j].is_friend and game_unit_list[j].x >= game_unit_list[i].x + get_game_unit_info(game_unit_list[i]).attack_range_begin and game_unit_list[j].x <= game_unit_list[i].x + get_game_unit_info(game_unit_list[i]).attack_range_end
                             : game_unit_list[j].is_friend and game_unit_list[j].x >= game_unit_list[i].x - get_game_unit_info(game_unit_list[i]).attack_range_end and game_unit_list[j].x <= game_unit_list[i].x - get_game_unit_info(game_unit_list[i]).attack_range_begin ) and game_unit_list[j].type.narrow() != "pencil_lead"){
                            if (target == nullptr){
                                target = &game_unit_list[j];
                            } else if ( game_unit_list[i].is_friend ? game_unit_list[j].x < target->x : game_unit_list[j].x > target->x ){
                                target = &game_unit_list[j];
                            }
                        }
                    }
                    //ターゲットが見つかったのなら
                    if (target != nullptr){
                        //攻撃処理
                        if (get_game_unit_info(game_unit_list[i]).feature == U"shot") {
                            //シャー芯を召喚
                            game_unit_list.push_back(game_unit(U"pencil_lead", game_unit_list[i].is_friend, game_unit_list[i].x + texture_size / 2));
                        } else {
                            target->durability -= get_game_unit_info(game_unit_list[i]).attack_power;
                            //攻撃時の、種類ごとのユニークな処理
                            if (game_unit_list[i].type == U"eraser") {
                                game_unit_list[i].durability -= 5;
                            } else if (game_unit_list[i].type == U"pencil_lead"){
                                game_unit_list[i].durability = 0;
                                //ノックバック
                                target->knock_back = 10;
                            } else {
                                //ノックバック
                                target->knock_back = 1;
                            }
                        }
                        //クールダウン
                        game_unit_list[i].cooldown = get_game_unit_info(game_unit_list[i]).cooldown;
                        //攻撃したフラグを立てる
                        has_attacked = true;
                    }
                    //攻撃したかの判定
                    if (not has_attacked) {
                        if (not game_unit_list[i].is_fixed){
                            //移動
                            //味方かの判定
                            if (game_unit_list[i].is_friend){
                                //正の方向に移動
                                game_unit_list[i].x += get_game_unit_info(game_unit_list[i]).speed;
                            } else{
                                //負の方向に移動
                                game_unit_list[i].x -= get_game_unit_info(game_unit_list[i]).speed;
                            }
                        }
                    } else {
                        //攻撃の効果音
                        HitPop.playOneShot();
                    }
                    
                } else {
                    if (game_unit_list[i].knock_back == 0){
                        //クールダウンのカウントを減らす
                        game_unit_list[i].cooldown --;
                    } else {
                        //ノックバックのカウントを減らし、後ろに弾き飛ばす
                        game_unit_list[i].knock_back --;
                        uint16 x_after_knockback = game_unit_list[i].x + (game_unit_list[i].is_friend ? -1 : 1);
                        if (game_unit_list[i].type.narrow() != "castle" and game_unit_list[i].type.narrow() != "pencil_sharpener") {
                            game_unit_list[i].x = x_after_knockback;
                        }
                    }
                }
                //破壊判定
                for (long int i = game_unit_list.size() - 1; i >= 0;  i--) {
                    if (game_unit_list[i].durability <= 0){
                        //城が破壊された場合は、勝敗を記録し、終了エフェクトを表示する
                        if (game_unit_list[i].type == U"castle") {
                            winner = game_unit_list ? 1 : 2;
                            effect.add<FinishEffect>(!game_unit_list[i].is_friend);
                        }
                        //削除
                        game_unit_list.erase(game_unit_list.begin() + i);
                        
                    }
                }
            }
            //AI
            if (winner == 0){
                //勢力の計算
                double friend_power = 0, enemy_power = 0;
                for (unsigned long int i = 0; i < game_unit_list.size(); i++){
                    if (game_unit_list[i].type.narrow() != "castle" and game_unit_list[i].type.narrow() != "pencil_lead"){
                        //強さは攻撃の強さと耐久力の合計
                        (game_unit_list[i].is_friend ? friend_power : enemy_power) += (get_game_unit_info(game_unit_list[i]).general_attack_power + game_unit_list[i].durability);
                    }
                }
                float PowerRatio;
                //0除算の防止
                if (friend_power == 0) {
                    PowerRatio = enemy_power == 0 ? 1 : 2;
                } else {
                    PowerRatio = enemy_power / friend_power;
                }
                //下準備
                if (AIMode == 0){
                    AIMode = round(Random()) + 1;
                    Logger << AIMode;
                }
                //召喚するgame_unitの決定
                if (AIStep != 3) {
                    //勢力の割合に応じた分岐
                    if (PowerRatio < 0.5) {
                        switch (enemy.money_level) {
                            case 0:
                            case 1:
                                AITarget = U"eraser";
                                break;
                            case 2:
                            case 3:
                                AITarget = U"triangle";
                                break;
                            case 4:
                            default:
                                if (AIStep == 0){
                                    AITarget = U"pencil_sharpener";
                                } else {
                                    AITarget = U"mechanical_pencil";
                                }
                                break;
                        }
                    } else if (PowerRatio < 1.25){
                        switch (enemy.money_level) {
                            case 0:
                            case 1:
                                AISaveMoney();
                                AITarget = U"pencil";
                                break;
                            case 2:
                            case 3:
                                AISaveMoney();
                                AITarget = U"triangle";
                                break;
                            case 4:
                            default:
                                AISaveMoney();
                                if (AIMode == 1) {
                                    //遠距離の召喚
                                    AITarget = (AIStep == 0) ? U"mechanical_pencil" : U"triangle";
                                } else {
                                    AITarget = U"triangle";
                                }
                                break;
                            }
                } else {
                        //攻めプロセス
                        switch (enemy.money_level) {
                            case 0:
                            case 1:
                                AISaveMoney();
                                break;
                            case 2:
                            case 3:
                                AISaveMoney();
                                AITarget=U"triangle";
                                break;
                            case 4:
                            default:
                                AISaveMoney();
                                if (AIStep == 0){
                                    AITarget=U"pencil_sharpener";
                                } else {
                                    AITarget=U"mechanical_pencil";
                                }
                                break;
                        }
                    }
                }
                if (AIStep == 3) {
                    enemy.money += AISavingMoney;
                    AISavingMoney = 0;
                    if (120 * Math::Pow(2, enemy.money_level) <= enemy.money) {
                        enemy.money -= 120 * Math::Pow(2, enemy.money_level);
                        enemy.money_level ++;
                        AIStep = 0;
                        AIMode = 0;
                    }
                } else /*召喚を試みる*/if (AITarget.narrow() != "" and summon_game_unit(AITarget, false)) {
                    AIStep ++;
                    AITarget = U"";
                }
#ifdef DEBUG
                if (KeyShift.pressed()) {
                    Print << U"Mode: " << AIMode;
                    Print << U"Target: " << (AITarget == U"" ? U"資金力強化" : AITarget);
                    Print << U"power" + Format(friend_power) + U"/" + Format(enemy_power);
                    Print << U"money: " + Format(player.money) + U" / " + Format(enemy.money);
                }
#endif
            }
            //蓄積された秒数を減らす
            action_accumulator -= 0.1;
        }
        //各陣営の収入処理
        while (0.5 <= income_accumulator) {
            //難易度に合わせて資金を増加させる
            switch (difficult_level) {
                case 1:
                    player.money += 10 * Math::Pow(1.5, player.money_level);
                    enemy.money += 7 * Math::Pow(1.5, enemy.money_level);
                    break;
                default:
                case 2:
                    player.money += 10 * Math::Pow(1.5, player.money_level);
                    enemy.money += 12 * Math::Pow(1.5, enemy.money_level);
                    break;
                case 3:
                    player.money += 7 * Math::Pow(1.5, player.money_level);
                    enemy.money += 12 * Math::Pow(1.5, enemy.money_level);
                    break;
            }
            income_accumulator -= 0.5;
            
        }
        waiting_time += winner != 0 ? Scene::DeltaTime() : 0;
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
