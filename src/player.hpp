# ifndef player_hpp
# define player_hpp

# include "logic.hpp"

namespace stfi
{
// プレイヤーのインターフェース
class IPlayer
{
public:
	virtual ~IPlayer();
	// 判断する関数
	virtual String judge(const GameState& state) = 0;

};

# ifdef HEADLESS
class ConsolePlayer : public IPlayer
{
	bool isFriend;
public:
	ConsolePlayer(bool isFriend);
	String judge(const GameState& state);
};
# endif

class WeakAI : public IPlayer
{
	//敵AIの攻撃のモード
	uint8 AIMode = 0;
	//敵AIの進行度
	uint16 AIStep = 0;
	//敵AIが召喚するターゲット
	String AITarget = U"";
	// レベルアップのために貯めている資金
	uint16 savedMoney = 0;
	// 必要量まで資金を貯める関数
	void saveMoney(const CampInfo& info);
public:
	// 判断する関数
	String judge(const GameState& state);
};

}

# endif /* player_hpp */
