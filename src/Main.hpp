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
    bool is_player_camp;
    std::string type;
    int16 durability;
    void set_data(std::string type, bool is_player_camp);
    uint16 cooldown = 0;
    void go();
    void attack(game_unit *target);
};

void game_unit::set_data(std::string type, bool is_player_camp){
    if (is_player_camp){
        this->x = 0;
    } else{
        this->x = 800;
    }
    this->type = type;
    this->is_player_camp = is_player_camp;
    this->durability = get_json_value("resource/config.json","/" + type + "/durability").get<std::uint16_t>();

}
void game_unit::go(){
    //移動
    if (this->is_player_camp){
        this->x += get_json_value("resource/config.json","/" + this->type + "/speed").get<std::int16_t>();
    } else{
        this->x -= get_json_value("resource/config.json","/" + this->type + "/speed").get<std::int16_t>();
    }
}

void game_unit::attack(game_unit *target){
    //攻撃処理
    target->durability -= get_json_value("resource/config.json","/" + this->type + "/attack_power").get<std::int16_t>();
    //クールダウン
    this->cooldown = get_json_value("resource/config.json","/" + this->type + "/cooldown").get<std::int16_t>();
}

}

#endif
