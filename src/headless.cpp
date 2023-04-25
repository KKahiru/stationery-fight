# include <iostream>
# include <limits>
# include <string.h>

# include "logic.hpp"

void LogSeparate(const std::string_view s)
{
	std::cout << std::endl;
	std::cout << "=====" << s << "====" << std::endl;
}

void LogCampInfo(const stfi::CampInfo& info)
{
	std::cout << "money: " << info.money << ", ";
	std::cout << "profitLevel: " << info.profitLevel << ", ";
	std::cout << "gameUnitPower: " << info.gameUnitPower << ", ";
	std::cout << std::endl;
}

inline void demo()
{
	json ConfigJson = LoadJSON(U"resource/config.json");
	stfi::GameState state(ConfigJson);
	stfi::WeakAI weakAI;

	Array<String> TypeList;
	TypeList.push_back(U"Upgrade profit level");
	for (const auto& i : state.GameUnitTypeList)
	{
		TypeList.push_back(i.first);
	}

	int profitTick = 0;

	while (1)
	{
		state.actionProcess();
		if (state.winner == 1)
		{
			std::cerr << "You win!" << std::endl;
			return;
		}
		else if (state.winner == 2)
		{
			std::cerr << "You lost..." << std::endl;
			return;
		}
		
		if (profitTick >= 2)
		{
			profitTick = 0;
			state.profitProcess(2);
			// ログ出力
			LogSeparate("UnitCount");
			std::cout << state.GameUnitList.size() << std::endl;
			LogSeparate("Stage");
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
			
			std::cout << std::endl;
			LogSeparate("FriendCamp");
			LogCampInfo(state.FriendCamp);
			LogSeparate("EnemyCamp");
			LogCampInfo(state.EnemyCamp);
			LogSeparate("Units");
			{
				size_t count = 0;
				for (const auto& i : TypeList)
				{
					std::cout << "[" <<  count << "]: " << Narrow(i);
					std::cout << " (";
					if (count == 0)
					{
						const uint16 cost = 120 * Math::Pow(2, state.FriendCamp.profitLevel);
						std::cout << cost;
					}
					else
					{
						std::cout << state.GameUnitTypeList[i].cost;
					}
					std::cout << "), ";
					
					count++;
				}
			}
			std::cout << std::endl;
			
			weakAI.judge(state);

			// 入力を求めて処理する
			{
				int choice;
				std::string input;
				std::getline(std::cin, input);
				try
				{
					choice = std::stoi(input);
				}
				catch (const std::invalid_argument& e)
				{
					choice = std::numeric_limits<int>::max();
				}
				catch (const std::out_of_range& e)
				{
					choice = std::numeric_limits<int>::max();
				}
				if (choice == 0)
				{
					bool result = state.upgradeProfitLevel(true);
					if (result)
					{
						std::cout << "Upgraded profit level.";
					}
					else
					{
						std::cout << "You don't have enough money to upgrade profit level!";
					}
				}
				else if (choice < TypeList.size())
				{
					String name = TypeList[choice];
					bool result = state.summonGameUnit(state.GameUnitTypeList[name], true);
					if (result)
					{
						std::cout << "Summoned " << Narrow(name) << ".";
					}
					else
					{
						std::cout << "You don't have enough money to summon " << Narrow(name) << "!";
					}
				}
				else
				{
					std::cout << "Skipped.";
				}
				std::cout << std::endl;
			}
		}
		else
		{
			profitTick ++;
		}
	}
}

int main(int argc, char *argv[])
{
	// コマンドライン引数がdemoだったらデモを起動
	if (argc > 1 && !strcmp(argv[1], "demo"))
	{
		demo();
	}
}