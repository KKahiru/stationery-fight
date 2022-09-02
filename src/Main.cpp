# include <Siv3D.hpp>
#include <iostream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

const int texture_size = 120;
//jsonファイルを開いてパーズする変数
inline json open_json_file(String file_path){
    //jsonファイルを開く
    TextReader json_file{ file_path };
    //jsonファイルを開けたか判定
    if (json_file.isOpen()){
        cout << "Opened "<< file_path <<"." << endl;
    } else{
        throw Error{U"Failed to open \'" + file_path + U"'."};
    }
    const string json_law = json_file.readAll().narrow();
    //jsonファイルの中身を読み込んでパーズする
    cout << json_file.readAll().narrow() << endl;
    json json_data = json::parse(json_law);
    return json_data;
}
//コンフィグ
json config;
//コンフィグの値を取得する関数
template <class Type>
Type get_config_value(string type, string json_path){
    json result = config.at(json::json_pointer("/ability_config/" + type + "/" + json_path)).get<Type>();
    return result;
}
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
    game_unit(String type, bool is_player_camp);
    //当たり判定
    Rect collision_detection{120,120};
};
//コンストラクターの定義
game_unit::game_unit(String type, bool is_friend){
    string type_ss = type.narrow();
    if (is_friend){
        this->x = 40;
    } else{
        this->x = Scene::Size().x - 40;
    }
    this->type = type;
    this->is_friend = is_friend;
    this->durability = get_config_value<uint16>(type_ss, "durability");
}
//プレイヤーの情報を格納する構造体
struct player_info{
    uint16 money;
    uint16 game_unit_power;
};
//プレイヤーと敵の情報生成
player_info player;
player_info enemy;
//ステージ上のgame_unitのリスト
Array<game_unit> game_unit_list;
//game_unitを召喚する関数
bool summon_game_unit(String type, bool is_friend){
    string type_ss = type.narrow();
    if ((is_friend ? player.money : enemy.money) >= get_config_value<uint16>(type_ss, "cost")){
        game_unit_list.push_back(game_unit(type,is_friend));
        (is_friend ? player.money : enemy.money) -= get_config_value<uint16>(type_ss, "cost");
    } else {
        return false;
    }
    return true;
}
//召喚ボタンの情報を格納する構造体
struct summon_button{
    summon_button(String type, uint16 count);
    String type;
    Texture texture;
    uint16 x;
    Rect background;
    
};
//コンストラクターの定義
summon_button::summon_button(String type, uint16 count){
    //種類の登録
    this->type = type;
    //ボタンのX座標
    this->x = texture_size * (count + 0.5) - 3 * count;
    this->texture = Texture{ Resource(U"resource/texture/" + type + U"/friend.png"), TextureDesc::Mipped };
    //ボタンの背景
    this->background = Rect{texture_size * count - 3 * count,Scene::Size().y - texture_size,texture_size,texture_size};
}
//game_unitの情報を格納する構造体
struct game_unit_info {
    game_unit_info(String type);
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
};
game_unit_info::game_unit_info(String type){
    string type_ss = type.narrow();
    this->ja_name = Unicode::Widen(get_config_value<string>(type_ss, "ja_name"));
    this->feature = Unicode::Widen(get_config_value<string>(type_ss, "feature"));
    this->durability = get_config_value<uint16>(type_ss, "durability");
    this->attack_power = get_config_value<uint16>(type_ss, "attack_power");
    this->speed = get_config_value<uint16>(type_ss, "speed");
    this->cooldown = get_config_value<uint16>(type_ss, "cooldown");
    this->attack_range_begin = get_config_value<uint16>(type_ss, "attack_range_begin");
    this->attack_range_end = get_config_value<uint16>(type_ss, "attack_range_end");
    this->cost = get_config_value<uint16>(type_ss, "cost");
    this->friend_texture = Texture{ Resource(U"resource/texture/" + type + U"/friend.png"), TextureDesc::Mipped };
    this->friend_attack_texture = Texture{ Resource(U"resource/texture/" + type + U"/friend_attack.png"), TextureDesc::Mipped };
    this->enemy_texture = Texture{ Resource(U"resource/texture/" + type + U"/enemy.png"), TextureDesc::Mipped };
    this->enemy_attack_texture = Texture{ Resource(U"resource/texture/" + type + U"/enemy_attack.png"), TextureDesc::Mipped };
    //攻撃の総合的な強さ
    this->general_attack_power = get_config_value<double>(type_ss, "attack_power") / get_config_value<double>(type_ss, "cooldown") * (get_config_value<double>(type_ss, "attack_range_end") - get_config_value<double>(type_ss, "attack_range_begin"));
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
//game_unit_infoの連装配列
unordered_map<string, game_unit_info> game_unit_info_list = {};
//game_unitからgame_unit_infoを取得する関数
game_unit_info get_game_unit_info(game_unit& target){
    return game_unit_info_list.at(target.type.narrow());
}
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
void Main(){
    //ウィンドウサイズの設定
    Window::Resize(1067,600);
    Scene::SetResizeMode(ResizeMode::Keep);
    Window::SetStyle(WindowStyle::Sizable);
    //背景を白くする
    Scene::SetBackground(Palette::White);
    //一旦描画
    System::Update();
    config = open_json_file(Resource(U"resource/config.json"));
    //攻撃音
    const Audio hit_pop_1{Resource(U"resource/sound/hit_pop_1.ogg")};
    // 蓄積された時間（秒）
    double action_accumulator = 0.0;
    double income_accumulator = 0.0;
    //ひび割れ
    TextureRegion crack[3] = { Texture{ Resource(U"resource/texture/crack/crack-1.png"), TextureDesc::Mipped }.resized(texture_size),
        Texture{ Resource(U"resource/texture/crack/crack-2.png"), TextureDesc::Mipped }.resized(texture_size),
        Texture{ Resource(U"resource/texture/crack/crack-3.png"), TextureDesc::Mipped }.resized(texture_size) };
    //エフェクト
    Effect effect;
    //勝敗を記録する変数
    uint8 winner = 0;
    //マスク用のシェーダー
    const PixelShader mask_shader = HLSL{ Resource(U"resource/shader/hlsl/multi_texture_mask.hlsl"), U"PS" }
    | GLSL{ Resource(U"resource/shader/glsl/multi_texture_mask.frag"), {{U"PSConstants2D", 0}} };
    if (not mask_shader) {
        throw Error{U"Error! Failed to load a shader file."};
    }
    //難易度
    int difficult_level = 2;
    //召喚ボタンのリスト
    Array<summon_button> summon_button_list;
    //召喚ボタンのラベルのフォント
    Font sumbtn_label_font{(int32)(texture_size * 0.15)};
    //プレイヤーと敵の状態設定（仮）
    switch (difficult_level) {
        case 1:
            player.money = 2000;
            enemy.money = 900;
            break;
        default:
        case 2:
            player.money = 1000;
            enemy.money = 1000;
            break;
        case 3:
            player.money = 900;
            enemy.money = 2000;
    }
    
    //召喚ボタン・ユニットの情報の登録
    game_unit_info_list.emplace("castle", game_unit_info(U"castle"));
    game_unit_info_list.emplace("pencil_lead", game_unit_info(U"pencil_lead"));
    for( int i=0; auto& el : config.at("all_types")) {
        String type = Unicode::Widen(el.get<string>());
        summon_button_list.push_back(summon_button(Unicode::Widen(el.get<string>()),i));
        game_unit_info_list.emplace(el.get<string>(), game_unit_info(type));
        i++;
    };
    //城を造る
    game_unit_list.push_back(game_unit(U"castle",true));
    game_unit_list.push_back(game_unit(U"castle",false));
    //毎フレームごとの処理
    while (System::Update()) {
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
        //召喚ボタンの描画
        for (unsigned long int i = 0; i < summon_button_list.size(); i++){
            //背景と枠線の描画
            summon_button_list[i].background.draw(get_config_value<int>(summon_button_list[i].type.narrow(), "cost") > player.money ? ColorF{ 0.4, 0.4, 0.4 } : summon_button_list[i].background.mouseOver() ? ColorF{ 0.5, 0.5, 0.5 } : ColorF{ 0.7, 0.7, 0.7 }).drawFrame(3, 0, ColorF{ 0.3, 0.3, 0.3 });
            //画像の描画(* 0.4 = * 0.8 / 2)
            summon_button_list[i].texture.resized(texture_size * 0.8).drawAt(summon_button_list[i].x,Scene::Size().y - texture_size * 0.4);
            //ラベルの描画
            sumbtn_label_font(game_unit_info_list.at(summon_button_list[i].type.narrow()).ja_name + U"|" + Format(game_unit_info_list.at(summon_button_list[i].type.narrow()).cost)).drawAt(summon_button_list[i].x,Scene::Size().y - texture_size * 0.875);
            //ボタンがクリックされたら
            if (summon_button_list[i].background.leftClicked()) {
                //game_unitを召喚する
                summon_game_unit(summon_button_list[i].type, true);
            }
        }
        //game_unitの行動処理（0.1秒刻み）
        while ( 0.1 <= action_accumulator) {
            //デバック情報の削除
            ClearPrint();
            //game_unitの行動処理
            for (unsigned long int i = 0; i < game_unit_list.size(); i++) {
                //クールダウンが終わったかの判定
                if (game_unit_list[i].cooldown == 0 and game_unit_list[i].knock_back == 0){
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
                        game_unit pencil_lead = *new game_unit(U"pencil_lead", game_unit_list[i].is_friend);
                        pencil_lead.x = game_unit_list[i].x + texture_size / 2;
                        game_unit_list.push_back(pencil_lead);
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
                        hit_pop_1.playOneShot();
                    }
                    
                } else {
                    if (game_unit_list[i].knock_back == 0){
                        //クールダウンのカウントを減らす
                        game_unit_list[i].cooldown --;
                    } else {
                        //ノックバックのカウントを減らし、後ろに弾き飛ばす
                        game_unit_list[i].knock_back --;
                        if (game_unit_list[i].type.narrow() != "castle") {
                            game_unit_list[i].x += (game_unit_list[i].is_friend ? -1 : 1);
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
                double friend_power = 0, enemy_power = 0;
                for (unsigned long int i = 0; i < game_unit_list.size(); i++){
                    if (game_unit_list[i].type.narrow() != "castle" and game_unit_list[i].type.narrow() != "pencil_lead"){
                        //強さは攻撃の強さと耐久力の合計
                        (game_unit_list[i].is_friend ? friend_power : enemy_power) += (get_game_unit_info(game_unit_list[i]).general_attack_power + game_unit_list[i].durability);
                    }
                }
                
                Print << U"power" + Format(friend_power) + U"/" + Format(enemy_power);
                if (friend_power > enemy_power) {
                    summon_game_unit(U"pencil", false);
                }
                Print << U"money: " + Format(player.money) + U" / " + Format(enemy.money);
            }
            //蓄積された秒数を減らす
            action_accumulator -= 0.1;
        }
        //各陣営の収入処理
        while (1.0 <= income_accumulator) {
            //難易度に合わせて資金を増加させる
            switch (difficult_level) {
                case 1:
                    player.money += 30;
                    enemy.money += 10;
                    break;
                default:
                case 2:
                    player.money += 20;
                    enemy.money += 20;
                    break;
                case 3:
                    player.money += 10;
                    enemy.money += 30;
                    break;
            }
            income_accumulator -= 1.0;
            
        }
      effect.update();
    }
}
