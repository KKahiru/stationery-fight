# include <iostream>
# include <thread>
# include <string.h>

# include "logic.hpp"
# include "player.hpp"

std::thread* launchDeepAI(int port)
{
# if defined(__MACH__) || defined(__linux__)
	json setting = LoadJSONWithComments(U"python_setting.jsonc");
	std::string shell = setting["shell"].get<std::string>();
	std::string before_launch_command = setting["before_launch_command"].get<std::string>();
	std::string python = setting["python"].get<std::string>();
	
	std::cout << "Generating new thread" << std::endl;
	return new std::thread([shell, before_launch_command, python, port]()
	{
		std::system((shell + " -c \"" + before_launch_command + python + " python/main.py " + std::to_string(port) + "\" &> port" + std::to_string(port) + ".log").c_str());
	});
# else
	std::cerr << "Sorry, the AI is not available on your platform." << std::endl;
	return nullptr;
# endif
}

void logStage(stfi::GameState& state)
{
	String Stage[10];
	size_t maxLength = 0;
	for (const auto& i : state.GameUnitList)
	{
		const size_t j = size_t((i.pos - 0.001) * 10);
		Stage[j] += i.type.substr(0, 1);
		const size_t length = Stage[j].length();
		if (maxLength < length)
		{
			maxLength = length;
		}
	}
	// オーバーフローしたら終了
	for (size_t i = maxLength - 1; i < maxLength; i--)
	{
		for (const auto& s : Stage)
		{
			if (s.length() > i)
			{
				std::cout << Narrow(s.substr(i, 1));
			}
			else
			{
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
				
}

void play(stfi::IPlayer* friendPlayer, stfi::IPlayer* enemyPlayer, const int difficulty, const json& configJSON)
{
	stfi::GameState state(configJSON);
	while (true)
	{
		// 2回行動処理をしてから収入・選択の処理をする
		for (size_t i = 0; i < 2; i++)
		{
			state.actionProcess();
			if (state.winner != 0)
			{
				break;
			}
			
		}
		state.profitProcess(difficulty);
		// プレイヤーの判断
		String friendChoice, enemyChoice;
		try
		{
			friendChoice = friendPlayer->judge(state);
			enemyChoice = enemyPlayer->judge(state);
		}
		// AIがリセットを要求した時の処理
		catch (const stfi::AIResetException& e)
		{
			return;
		}

		if (state.winner == 0)
		{
			if (friendChoice == U"upgrade")
			{
				state.upgradeProfitLevel(true);
			}
			else if (friendChoice != U"skip")
			{
				state.summonGameUnit(state.GameUnitTypeList[friendChoice], true);
			}
			if (enemyChoice == U"upgrade")
			{
				state.upgradeProfitLevel(false);
			}
			else if (enemyChoice != U"skip")
			{
				state.summonGameUnit(state.GameUnitTypeList[enemyChoice], false);
			}
		}
		else
		{
			// 勝敗情報
			if (state.winner == 1)
			{
				std::cout << "The friend camp won!" << std::endl;
			}
			else
			{
				std::cout << "The friend camp lost..." << std::endl;
			}
			break;
		}
	}
}

int main(int argc, const char* argv[])
{
	// サブコマンドがない場合は終了
	if (argc < 2)
	{
		std::cerr << "No subcommand specified." << std::endl;
		return 1;
	}
	json configJSON = LoadJSON(U"resource/config.json");

	// available_typesに含まれるユニットタイプと資金力強化を含む選択肢リスト
	Array<String> optionList(configJSON["available_types"].size() + 1);
	{
		auto& availableTypesJSON = configJSON["available_types"];
		optionList[0] = U"upgrade";
		for (size_t i = 0; i < availableTypesJSON.size();i++)
		{
			optionList[i + 1] = Widen(availableTypesJSON[i].get<std::string>());
		}
	}

	// サブコマンドがdemoだったらデモを起動
	if (!strcmp(argv[1], "demo"))
	{
		stfi::IPlayer* friendPlayer = new stfi::ConsolePlayer(true);
		stfi::IPlayer* enemyPlayer = new stfi::WeakAI();
		play(friendPlayer, enemyPlayer, 1, configJSON);
		delete friendPlayer;
		delete enemyPlayer;
	}
	else if (!strcmp(argv[1], "train"))
	{
		std::thread* friendThread = launchDeepAI(1);
		if (friendThread == nullptr)
		{
			return 1;
		}
		stfi::IPlayer *friendPlayer, *enemyPlayer;
		while (true)
		{
			// 初期化
			try
			{
				friendPlayer = new stfi::DeepAI(1, optionList, true);
				enemyPlayer = new stfi::WeakAI();
			}
			catch (const stfi::AIResetException& e)
			{}
			catch (const stfi::AIFinishException& e)
			{
				std::cout << "Training has finished." << std::endl;
				return 0;
			}
			play(friendPlayer, enemyPlayer, 1, configJSON);
			delete friendPlayer;
			delete enemyPlayer;
		}
		if (friendThread->joinable())
		{
			friendThread->join();
		}
		delete friendThread;
	}
}
