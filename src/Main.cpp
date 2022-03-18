# include <Siv3D.hpp>
#include <iostream>
#include <vector>
using namespace std;

class game_unit{
public:
    int x;
    bool is_player_camp;
    string type;
    int health;
    void set_data(int x, string type, bool is_player_camp, int health);
};

void game_unit::set_data(int x, string type, bool is_player_camp, int health)
{
    this->x = x;
    this->type = type;
    this->is_player_camp = is_player_camp;
    this->health = health;
}

void Main(){
    Scene::SetBackground(Palette::White);
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
                Circle{ game_units[i].x , 300 , 20 }.draw(ColorF{ 0.25 });
            }
            
        }
        //game_unitの行動処理
        while ( 0.1 <= accumulator) {
            for (unsigned long int i = 0; i < game_units.size(); i++) {
                /*cout << "No." << i << "| X:" << game_units[i].x << "| isOurCamp:" << game_units[i].isOurCamp << "| type:" << game_units[i].type << endl;
                 cout << (0 <= game_units[i].x && game_units[i].x <= 1) << endl;
                 Circle{ game_units[i].x , 300 , 20 }.draw(ColorF{ 0.25 });*/
                //cout << game_units[i].health << endl;
                
                if (game_units[i].is_player_camp){
                    //移動
                    game_units[i].x += 3;
                    ///攻撃するgame_unitの検索
                    for (unsigned long int i2 = 0; i2 < game_units.size(); i2++) {
                        if (!game_units[i2].is_player_camp){
                            //攻撃処理
                            game_units[i2].health -= 1;
                        }
                    }
                } else{
                    //移動
                    game_units[i].x -= 3;
                    ///攻撃するgame_unitの検索
                    for (unsigned long int i2 = 0; i2 < game_units.size(); i2++) {
                        if (game_units[i2].is_player_camp){
                            //攻撃処理
                            game_units[i2].health -= 1;
                        }
                    }
                }
                //vector<int> suitability_game_units;
            }
            for (long int i = game_units.size() - 1; i >= 0;  i--) {
                if (game_units[i].health < 0){
                    game_units.erase(game_units.begin() + i);
                }
            }
            /*cout << accumulator << endl;*/
            //cout << "-----" << endl;
            accumulator -= 0.1;
        }
    }
}
