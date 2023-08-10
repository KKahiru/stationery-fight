# include <iostream>
# include <string.h>

# include "logic.hpp"
# include "player.hpp"

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
}
