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
        throw range_error("Failed to open \'" + file_path.narrow() + "\'.");
    }
    const string json_law = json_file.readAll().narrow();
    //jsonファイルの中身を読み込んでパーズする
    cout << json_file.readAll().narrow() << endl;
    json json_data = json::parse(json_law);
    return json_data;
}
//コンフィグ
const json config = open_json_file(U"resource/config.json");
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
    string type;
    //攻撃力
    uint16 attack_power;
    //移動速度
    uint16 speed;
    //攻撃時にセットするクールダウン
    uint16 reset_cooldown;
    //攻撃範囲の始まり
    uint16 attack_range_begin;
    //攻撃範囲の終わり
    uint16 attack_range_end;
    //x座標
    uint16 x;
    //耐久値
    int16 durability;
    //クールダウン
    uint16 cooldown = 0;
    //コンストラクター
    game_unit(string type, bool is_player_camp);
    //通常時テクスチャー
    Texture texture;
    //攻撃時テクスチャー
    Texture attack_texture;
    //移動
    void go();
    //相手を攻撃
    void attack(game_unit *target);
};
//コンストラクターの定義
game_unit::game_unit(string type, bool is_friend){
    if (is_friend){
        this->x = 40;
    } else{
        this->x = Scene::Size().x - 40;
    }
    this->type = type;
    this->is_friend = is_friend;
    this->durability = get_config_value<uint16>(type, "durability");
    this->attack_power = get_config_value<uint16>(type, "attack_power");
    this->speed = get_config_value<uint16>(type, "speed");
    this->reset_cooldown = get_config_value<uint16>(type, "cooldown");
    if (is_friend){
        this->texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_friend.png"), TextureDesc::Mipped };
        this->attack_texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_friend_attack.png"), TextureDesc::Mipped };
    } else{
        this->texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_enemy.png"), TextureDesc::Mipped };
        this->attack_texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_enemy_attack.png"), TextureDesc::Mipped };
    }
    this->attack_range_begin = get_config_value<uint16>(type, "attack_range_begin");
    this->attack_range_end = get_config_value<uint16>(type, "attack_range_end");
    
}
//移動する関数の定義
void game_unit::go(){
    //味方かの判定
    if (this->is_friend){
        //正の方向に移動
        this->x += this->speed;
    } else{
        //負の方向に移動
        this->x -= this->speed;
    }
}
//相手を攻撃する関数の定義
void game_unit::attack(game_unit *target){
    //攻撃処理
    target->durability -= this->attack_power;
    //クールダウン
    this->cooldown = this->reset_cooldown;
}

struct summon_button{
    summon_button(string type,uint16 count);
    string type;
    Texture texture;
    uint16 x;
    Rect background;
    
};
summon_button::summon_button(string type,uint16 count){
    this->type = type;
    this->x = texture_size * (count + 0.5);
    this->texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_friend.png"), TextureDesc::Mipped };
    this->background = Rect{texture_size * count,Scene::Size().y - texture_size,texture_size,texture_size};
}

void Main(){
    //ウィンドウサイズの設定
    Window::Resize(1067,600);
    //背景を白くする
    Scene::SetBackground(Palette::White);
    //攻撃音
    const Audio hit_pop_1{Resource(U"resource/sound/hit_pop_1.ogg")};
    // 蓄積された時間（秒）
    double accumulator = 0.0;
    //ステージ上のgame_unitのリスト
    Array<game_unit> game_units;
    //召喚ボタンのリスト
    Array<summon_button> summon_buttons;
    //城を造る
    game_units.push_back(game_unit("castle",true));
    game_units.push_back(game_unit("castle",false));
    //召喚ボタンの登録
    for( int i=0; auto& el : config.at("all_types")) {
        summon_buttons.push_back(summon_button(el.get<string>(),i));
        i++;
    };
    //毎フレームごとの処理
    while (System::Update()) {
        //蓄積された秒数の記録
        accumulator += Scene::DeltaTime();
        //game_unitの描画
        for (unsigned long int i = 0; i < game_units.size(); i++) {
            //自分の前に重なっているgame_unitのカウント数
            int overlapped_unit_count = 0;
            for (long int j = 0; j < distance(game_units.begin(), game_units.begin() + i); j++){
                if (game_units[j].x < game_units[i].x + texture_size/4*3 and game_units[j].x > game_units[i].x - texture_size/4*3 and game_units[j].type != "castle") {
                    //カウント
                    overlapped_unit_count++;
                }
            }
            //攻撃しているか
            if (game_units[i].cooldown > game_units[i].reset_cooldown / 5 * 3){
                //攻撃テクスチャーで描画
                game_units[i].attack_texture.resized(texture_size).drawAt(game_units[i].x , Scene::Center().y - overlapped_unit_count * texture_size / 8);
            } else {
                //通常テクスチャーで描画
                game_units[i].texture.resized(texture_size).drawAt(game_units[i].x , Scene::Center().y - overlapped_unit_count * texture_size / 8);
            }
        }
        //敵を召喚する簡易ボタン
        if (SimpleGUI::Button(U"敵鉛筆召喚", Vec2{ 0, 100 }))
        {
            game_units.push_back(game_unit("pencil",false));
        }
        if (SimpleGUI::Button(U"敵消しゴム召喚", Vec2{ 150, 100 }))
        {
            game_units.push_back(game_unit("eraser",false));
        }
        if (SimpleGUI::Button(U"敵三角定規召喚", Vec2{ 350, 100 }))
        {
            game_units.push_back(game_unit("triangle",false));
        }
        //召喚ボタンの描画
        for (unsigned long int i = 0; i<summon_buttons.size(); i++){
            summon_buttons[i].background.draw(summon_buttons[i].background.mouseOver() ? ColorF{ 0.5, 0.5, 0.5 } : ColorF{ 0.6, 0.6, 0.6 }).drawFrame(4, 0, ColorF{ 0.1, 0.1, 0.1 });
            summon_buttons[i].texture.resized(texture_size).drawAt(summon_buttons[i].x,Scene::Size().y-texture_size/2);
            if (summon_buttons[i].background.leftClicked()) {
                game_units.push_back(game_unit(summon_buttons[i].type,true));
            }
        }
        //game_unitの行動処理（0.1秒刻み）
        while ( 0.1 <= accumulator) {
            for (unsigned long int i = 0; i < game_units.size(); i++) {
                //情報を残すコード
                //cout << "No." << i << "| X:" << game_units[i].x << "| type:" << game_units[i].type << "| isPlayerCamp:" << game_units[i].is_friend  << "| health:" << game_units[i].durability <<endl;
                //クールダウンが終わったかの判定
                if (game_units[i].cooldown == 0){
                    //攻撃したかのフラグ
                    bool has_attacked = false;
                    //攻撃するgame_unitの検索
                    for (unsigned long int i2 = 0; i2 < game_units.size(); i2++) {
                        //相手が敵で、攻撃範囲内にいるかを判別する
                        if ( game_units[i].is_friend
                            ? !game_units[i2].is_friend and game_units[i2].x >= game_units[i].x + game_units[i].attack_range_begin and game_units[i2].x <= game_units[i].x + game_units[i].attack_range_end
                            : game_units[i2].is_friend and game_units[i2].x >= game_units[i].x - game_units[i].attack_range_end and game_units[i2].x <= game_units[i].x - game_units[i].attack_range_begin ){
                            //攻撃
                            game_units[i].attack(&game_units[i2]);
                            //攻撃時の、種類ごとのユニークな処理
                            if (game_units[i].type == "eraser") {
                                game_units[i].durability -= 5;
                            }
                            //攻撃したフラグを立てる
                            has_attacked = true;
                            break;
                        }
                    }
                    //攻撃したかの判定
                    if (!has_attacked) {
                        //移動
                        game_units[i].go();
                    } else {
                        //攻撃の効果音
                        hit_pop_1.playOneShot();
                    }
                } else {
                    //クールダウンのカウントを減らす
                    game_units[i].cooldown --;
                }
            }
            //破壊判定
            for (long int i = game_units.size() - 1; i >= 0;  i--) {
                if (game_units[i].durability <= 0){
                    //削除
                    game_units.erase(game_units.begin() + i);
                }
            }
            //ログの区切り線
            //cout << "-----" << endl;
            //蓄積された秒数を減らす
            accumulator -= 0.1;
        }
    }
}
