# pragma once

# include <zmq.hpp>

# include "logic.hpp"

namespace stfi
{
// 通信に失敗したときの例外クラス
class AICommunicationException : public std::runtime_error
{
public:
	AICommunicationException(const char* _Message);
};
// AIがリセットを要求した時の例外クラス
class AIResetException :  public std::runtime_error
{
public:
	AIResetException(const char* _Message);
};

class AIFinishException : public std::runtime_error
{
public:
	AIFinishException(const char* _Message);
};


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

class DeepAI : public IPlayer
{
	Array<String> optionList;

	bool isFriend;

	zmq::context_t ctx{ 1 };

	zmq::socket_t socket{ ctx, zmq::socket_type::pair };

public:
	DeepAI(int port, const Array<String>& optionList, bool isFriend);
	// 判断する関数
	String judge(const GameState& state);
};

}
