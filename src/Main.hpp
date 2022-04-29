#ifndef INCLUDE_STATIONERY_FIGHT_HPP
#define INCLUDE_STATIONERY_FIGHT_HPP
#include <iostream>
#include "json.hpp"
#include <fstream> //ファイルシステム
#include <cstdlib> //abortの定義
#include <Siv3D.hpp> //Bool型


namespace stfi{
const int texture_size = 80;

nlohmann::json get_json_value(std::string file_path, std::string json_path){
    //config.jsonを開く
    std::ifstream json_file(file_path, std::ios::in);
    if (json_file.is_open()){
        std::cout << "Opened "<< file_path <<"." << std::endl;
    } else{
        std::cerr << "Error! Could not open " << file_path <<"." << std::endl;
        abort();
    }
    //config.jsonの中身をしまう変数
    std::string law_json;
    //config.jsonの中身を取り出す
    while (!json_file.eof()) {
        std::string temp_s;
        std::getline(json_file, temp_s);
        
        if (law_json.length() > 0){
            law_json = law_json +"\n" + temp_s;
        }else{
            law_json = temp_s;
        }
    }
    nlohmann::json json_data = nlohmann::json::parse(law_json);
    nlohmann::json result = json_data.at(nlohmann::json::json_pointer(json_path));
    return result;
}

class game_unit{
public:
    //味方かのbool値
    bool is_friend;
    //game_unitのタイプ
    std::string type;
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
    game_unit(std::string type, bool is_player_camp);
    //通常時テクスチャー
    TextureRegion texture;
    //攻撃時テクスチャー
    TextureRegion attack_texture;
    //移動
    void go();
    //相手を攻撃
    void attack(game_unit *target);
};

//コンストラクターの定義
game_unit::game_unit(std::string type, bool is_friend){
    if (is_friend){
        this->x = 0;
    } else{
        this->x = 800;
    }
    this->type = type;
    this->is_friend = is_friend;
    this->durability = get_json_value("resource/config.json","/" + type + "/durability").get<uint16>();
    this->attack_power = get_json_value("resource/config.json","/" + type + "/attack_power").get<uint16>();
    this->speed = get_json_value("resource/config.json","/" + type + "/speed").get<uint16>();
    this->reset_cooldown = get_json_value("resource/config.json","/" + type + "/cooldown").get<uint16>();
    if (is_friend){
        this->texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_friend.png") }.resized(texture_size);
        this->attack_texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_friend_attack.png") }.resized(texture_size);
    } else{
        this->texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_enemy.png") }.resized(texture_size);
        this->attack_texture = Texture{ Resource(U"resource/texture/" + Unicode::Widen(type) + U"_enemy_attack.png") }.resized(texture_size);
    }
    this->attack_range_begin = get_json_value("resource/config.json","/" + type + "/attack_range_begin").get<uint16>();
    this->attack_range_end = get_json_value("resource/config.json","/" + type + "/attack_range_end").get<uint16>();
    
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

}

#endif
