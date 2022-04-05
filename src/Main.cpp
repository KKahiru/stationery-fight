# include <Siv3D.hpp>
#include <iostream>
#include <vector>
#include <fstream> //ファイルシステム
#include <cstdlib> //quick_exitの定義
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

class game_unit{
public:
    uint16 x;
    bool is_player_camp;
    string type;
    int16 durability;
    void set_data(int x, string type, bool is_player_camp, int health);
    uint16 cooldown = 0;
};

void game_unit::set_data(int x, string type, bool is_player_camp, int health)
{
    this->x = x;
    this->type = type;
    this->is_player_camp = is_player_camp;
    this->durability = health;
    
}

void Main(){
    Scene::SetBackground(Palette::White);
    const Audio hit_pop_1{U"resource/sound/hit_pop_1.ogg"};
    // 蓄積された時間（秒）
    double accumulator = 0.0;
    //ステージ上のgame_unitのリスト
    vector<game_unit> game_units;
    //config.jsonを開く
    ifstream config_file("resource/config.json", ios::in);
    if (config_file.is_open()){
        cout << "Opened resource/config.json." << endl;
    } else{
        cerr << "Error!Could not open resource/config.json." << endl;
        abort();
    }
    //config.jsonの中身をしまう変数
    string law_config_json;
    //config.jsonの中身を取り出す
    while (!config_file.eof()) {
        string temp_s;
        getline(config_file, temp_s);
        
        if (law_config_json.length() > 0){
            law_config_json = law_config_json +"\n" + temp_s;
        }else{
            law_config_json = temp_s;
        }
    }
    //cout << law_config_json << endl;
    json config_json = json::parse(law_config_json);
    while (System::Update())
    {
        accumulator += Scene::DeltaTime();
        /*for (auto i : step(9))
         {
         const double theta = (i * 60_deg + t * 30_deg);
         const Vec2 pos = OffsetCircular{ Scene::Center(), 100, theta };
         Circle{ pos , 20 }.draw(ColorF{ 0.25 });
         }*/
        if (SimpleGUI::Button(U"味方召喚", Vec2{ 100, 100 }))
        {
            game_unit new_game_unit;
            new_game_unit.set_data(0,"pencil",true,100);
            game_units.push_back(new_game_unit);
            
        }
        if (SimpleGUI::Button(U"敵召喚", Vec2{ 300, 100 }))
        {
            game_unit new_game_unit;
            new_game_unit.set_data(800,"pencil",false,100);
            game_units.push_back(new_game_unit);
        }
        //game_unitの描画
        for (unsigned long int i = 0; i < game_units.size(); i++) {
            if (game_units[i].is_player_camp) {
                Circle{ game_units[i].x , 300 , 20 }.draw(ColorF{ 0,0,1 });
            } else {
                Circle{ game_units[i].x , 300 , 20 }.draw(ColorF{ 1,0,0 });
            }
            
        }
        //game_unitの行動処理
        while ( 0.1 <= accumulator) {
            for (unsigned long int i = 0; i < game_units.size(); i++) {
                //情報を残すコード
                cout << "No." << i << "| X:" << game_units[i].x << "| type:" << game_units[i].type << "| isPlayerCamp:" << game_units[i].is_player_camp  << "| health:" << game_units[i].durability <<endl;
                //クールダウンが終わったかの判定
                if (game_units[i].cooldown == 0){
                    bool has_attacked = false;
                    //攻撃するgame_unitの検索
                    for (unsigned long int i2 = 0; i2 < game_units.size(); i2++) {
                        //相手が敵かどうかを判別する長い長い条件文
                        if ( (game_units[i].is_player_camp and !game_units[i2].is_player_camp and game_units[i2].x>=game_units[i].x and game_units[i2].x <= game_units[i].x + 100) or (!game_units[i].is_player_camp and game_units[i2].is_player_camp and game_units[i2].x>=game_units[i].x - 100 and game_units[i2].x <= game_units[i].x ) ){
                            
                            //攻撃処理
                            game_units[i2].durability -= config_json.at(json::json_pointer("/" + game_units[i].type + "/attack_power")).get<std::int16_t>();
                            //攻撃したフラグを立てる
                            has_attacked = true;
                            //攻撃の効果音
                            hit_pop_1.playOneShot();
                            //クールダウン
                            game_units[i].cooldown = config_json.at(json::json_pointer("/" + game_units[i].type + "/cooldown")).get<std::int16_t>();
                        }
                    }
                    //攻撃したかの判定
                    if (!has_attacked) {
                        //移動
                        if (game_units[i].is_player_camp){
                            game_units[i].x += config_json.at(json::json_pointer("/" + game_units[i].type + "/speed")).get<std::int16_t>();
                        } else{
                            game_units[i].x -= config_json.at(json::json_pointer("/" + game_units[i].type + "/speed")).get<std::int16_t>();
                        }
                    }
                } else {
                    game_units[i].cooldown --;
                }
            }
            //破壊判定
            for (long int i = game_units.size() - 1; i >= 0;  i--) {
                if (game_units[i].durability <= 0){
                    game_units.erase(game_units.begin() + i);
                }
            }
            //cout << "-----" << endl;
            accumulator -= 0.1;
        }
    }
}
