# include <Siv3D.hpp>
#include <iostream>
#include <vector>
#include <fstream> //ファイルシステム
#include <cstdlib> //quick_exitの定義
#include "json.hpp"
#include "Main.hpp"
using namespace std;
using namespace stfi;
using json = nlohmann::json;

void Main(){
    const int texture_size = 80;
    //テクスチャーの定義
    //鉛筆
    TextureRegion texture_pencil_friend = Texture{ Resource(U"resource/texture/pencil_friend.png") }.resized(texture_size);
    TextureRegion texture_pencil_friend_attack = Texture{ Resource(U"resource/texture/pencil_friend_attack.png") }.resized(texture_size);
    TextureRegion texture_pencil_enemy = Texture{ Resource(U"resource/texture/pencil_enemy.png") }.resized(texture_size);
    TextureRegion texture_pencil_enemy_attack = Texture{ Resource(U"resource/texture/pencil_enemy_attack.png") }.resized(texture_size);
    //消しゴム
    TextureRegion texture_eraser_friend = Texture{ Resource(U"resource/texture/eraser_friend.png") }.resized(texture_size);
    TextureRegion texture_eraser_friend_attack = Texture{ Resource(U"resource/texture/eraser_friend_attack.png") }.resized(texture_size);
    TextureRegion texture_eraser_enemy = Texture{ Resource(U"resource/texture/eraser_enemy.png") }.resized(texture_size);
    TextureRegion texture_eraser_enemy_attack = Texture{ Resource(U"resource/texture/eraser_enemy_attack.png") }.resized(texture_size);
    TextureRegion texture_castle = Texture{ Resource(U"resource/texture/castle.png") }.resized(texture_size * 2);
    
    Scene::SetBackground(Palette::White);
    const Audio hit_pop_1{U"resource/sound/hit_pop_1.ogg"};
    // 蓄積された時間（秒）
    double accumulator = 0.0;
    //ステージ上のgame_unitのリスト
    vector<game_unit> game_units;
    //城を造る
    game_units.push_back(game_unit("castle",true,&texture_castle,&texture_castle));
    game_units.push_back(game_unit("castle",false,&texture_castle,&texture_castle));
    while (System::Update())
    {
        accumulator += Scene::DeltaTime();
        //仮GUI
        if (SimpleGUI::Button(U"味方鉛筆召喚", Vec2{ 0, 100 }))
        {
            game_units.push_back(game_unit("pencil",true,&texture_pencil_friend,&texture_pencil_friend_attack));
            
        }
        if (SimpleGUI::Button(U"敵鉛筆召喚", Vec2{ 200, 100 }))
        {
            game_units.push_back(game_unit("pencil",false,&texture_pencil_enemy,&texture_pencil_enemy_attack));
        }
        if (SimpleGUI::Button(U"味方消しゴム召喚", Vec2{ 400, 100 }))
        {
            game_units.push_back(game_unit("eraser",true,&texture_eraser_friend,&texture_eraser_friend_attack));
        }
        if (SimpleGUI::Button(U"敵消しゴム召喚", Vec2{ 600, 100 }))
        {
            game_units.push_back(game_unit("eraser",false,&texture_eraser_enemy,&texture_eraser_enemy_attack));
        }
        //game_unitの描画
        for (unsigned long int i = 0; i < game_units.size(); i++) {
            int overlapped_unit_count = 0;
            for (long int i2 = 0; i2 < distance(game_units.begin(), game_units.begin() + i); i2++){
                overlapped_unit_count++;
            }
            if (game_units[i].cooldown > game_units[i].reset_cooldown/5*3){
                game_units[i].attack_texture->drawAt(game_units[i].x , Scene::Center().x - overlapped_unit_count);
            } else {
                 game_units[i].texture->drawAt(game_units[i].x , Scene::Center().x - overlapped_unit_count);
            }
               
        }
        //game_unitの行動処理
        while ( 0.1 <= accumulator) {
            for (unsigned long int i = 0; i < game_units.size(); i++) {
                //情報を残すコード
                cout << "No." << i << "| X:" << game_units[i].x << "| type:" << game_units[i].type << "| isPlayerCamp:" << game_units[i].is_friend  << "| health:" << game_units[i].durability <<endl;
                //クールダウンが終わったかの判定
                if (game_units[i].cooldown == 0){
                    bool has_attacked = false;
                    //攻撃するgame_unitの検索
                    for (unsigned long int i2 = 0; i2 < game_units.size(); i2++) {
                        //相手が敵かどうかを判別する長い長い条件文
                        if ( (game_units[i].is_friend and !game_units[i2].is_friend and game_units[i2].x>=game_units[i].x and game_units[i2].x <= game_units[i].x + 100) or (!game_units[i].is_friend and game_units[i2].is_friend and game_units[i2].x>=game_units[i].x - 100 and game_units[i2].x <= game_units[i].x ) ){
                            //攻撃
                            game_units[i].attack(&game_units[i2]);
                            //攻撃時の、種類ごとのユニークな処理
                            if (game_units[i].type == "eraser") {
                                    game_units[i].durability -= 5;
                            }
                            //攻撃したフラグを立てる
                            has_attacked = true;
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
            //cout << "-----" << endl;
            accumulator -= 0.1;
        }
    }
}
