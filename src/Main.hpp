#ifndef INCLUDE_STATIONERY_FIGHT_HPP
#define INCLUDE_STATIONERY_FIGHT_HPP
#include <iostream>
#include "json.hpp"
#include <fstream> //ファイルシステム
#include <Siv3D/FormatBool.hpp> //Bool型
#include <Siv3D/FormatInt.hpp> //整数型

namespace stfi{
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
    uint16 x;
    bool is_friend;
    std::string type;
    int16 durability;
    uint16 attack_power;
    uint16 speed;
    uint16 reset_cooldown;
    game_unit(std::string type, bool is_player_camp);
    uint16 cooldown = 0;
    void go();
    void attack(game_unit *target);
};

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

}
void game_unit::go(){
    //移動
    if (this->is_friend){
        this->x += this->speed;
    } else{
        this->x -= this->speed;
    }
}

void game_unit::attack(game_unit *target){
    //攻撃処理
    target->durability -= this->attack_power;
    //クールダウン
    this->cooldown = this->reset_cooldown;
}

}

#endif
