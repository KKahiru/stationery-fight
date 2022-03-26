# include <Siv3D.hpp>
#include <iostream>
#include <vector>
using namespace std;

class game_unit{
public:
    uint16 x;
    bool is_player_camp;
    string type;
    uint16 durability;
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
    const Audio hit_pop_1{U"resource/hit_pop_1.ogg"};
    // 蓄積された時間（秒）
    double accumulator = 0.0;
    vector<game_unit> game_units;
    
    /*for (int i = 0; i < 5; i++){
     game_units.push_back(*new game_unit);
     game_units[i].x = i*50;
     game_units[i].is_player_camp = 1;
     game_units[i].type = "pencil";
     game_units[i].health = 100;
     }*/
    
    while (System::Update())
    {
        //const double t = Scene::Time();
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
                bool has_attacked = false;
                //攻撃するgame_unitの検索
                for (unsigned long int i2 = 0; i2 < game_units.size(); i2++) {
                    if ( (game_units[i].is_player_camp and !game_units[i2].is_player_camp and game_units[i2].x>=game_units[i].x and game_units[i2].x <= game_units[i].x + 100) or (!game_units[i].is_player_camp and game_units[i2].is_player_camp and game_units[i2].x>=game_units[i].x - 100 and game_units[i2].x <= game_units[i].x ) ){
                        //攻撃処理
                        game_units[i2].durability -= 1;
                        has_attacked = true;
                        hit_pop_1.playOneShot();
                        game_units[i].cooldown = 100;
                    }
                }
                
                if (!has_attacked) {
                    //移動
                    if (game_units[i].is_player_camp){
                        game_units[i].x += 3;
                    } else{
                        game_units[i].x -= 3;
                    }
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
