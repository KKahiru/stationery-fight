# include "player.hpp"

# include <iostream>
# include <string>

namespace stfi
{
IPlayer::~IPlayer()
{}

# ifdef HEADLESS
ConsolePlayer::ConsolePlayer(bool isFriend)
	: isFriend(isFriend)
{}
String ConsolePlayer::judge(const GameState& state)
{
	const CampInfo& camp = isFriend ? state.FriendCamp : state.EnemyCamp;

	for (size_t i = 0; i < state.OptionList.size(); i++)
	{
		const String item = state.OptionList[i];
		const uint16 cost = item == U"upgrade" ? 120 * Math::Pow(2, camp.profitLevel) : state.GameUnitTypeList.at(item).cost;
		std::cout << "[" << i << "] " << Narrow(item) << " (" << cost << "), ";
	}
	std::cout << std::endl;
	size_t choice;
	std::string input;
	std::getline(std::cin, input);
	try
	{
		choice = std::stoi(input);
	}
	catch (const std::invalid_argument& e)
	{
		std::cout << "Skip." << std::endl;
		return U"skip";
	}
	catch (const std::out_of_range& e)
	{
		std::cout << "Skip." << std::endl;
		return U"skip";
	}
	if (choice >= state.OptionList.size())
	{
		std::cout << "Skip." << std::endl;
		return U"skip";
	}
	String target = state.OptionList[choice];
	if (state.GameUnitTypeList.at(target).cost <= camp.money)
	{
		std::cout << "Summon " << Narrow(target) << "." << std::endl;
	}
	else
	{
		std::cout << "There is not enough money to summon " << Narrow(target) << "." << std::endl;
	}

	return target;
}
# endif

void WeakAI::saveMoney(const CampInfo& info)
{
	if (savedMoney < (AIStep + 1) * 25 * Math::Pow(2, info.profitLevel) )
	{
		savedMoney += info.money;
	}
	return;
}

String WeakAI::judge(const GameState& state)
{
		//勢力の計算
	double friendPower = 0, enemyPower = 0;
	for (unsigned long int i = 0; i < state.GameUnitList.size(); i++)
	{
		if (state.GameUnitList[i].type != U"castle" and state.GameUnitList[i].type != U"pencil_lead")
		{
			//強さは攻撃の強さと耐久力の合計
			(state.GameUnitList[i].isFriend ? friendPower : enemyPower) += (state.getGameUnitType(state.GameUnitList[i]).generalPower + state.GameUnitList[i].durability);
		}
	}
	float powerRatio;
	//0除算の防止
	if (friendPower == 0) {
		powerRatio = enemyPower == 0 ? 1 : 2;
	} else {
		powerRatio = enemyPower / friendPower;
	}
	//下準備
	if (AIMode == 0){
		AIMode = round(Random()) + 1;
	}
	//召喚するユニットの決定
	if (AIStep != 3) {
		//勢力の割合に応じた分岐
		if (powerRatio < 0.5) {
			switch (state.EnemyCamp.profitLevel) {
				case 0:
				case 1:
					AITarget = U"eraser";
					break;
				case 2:
				case 3:
					AITarget = U"triangle";
					break;
				case 4:
				default:
					if (AIStep == 0){
						AITarget = U"pencil_sharpener";
					} else {
						AITarget = U"mechanical_pencil";
					}
					break;
			}
		} else if (powerRatio < 1.25){
			switch (state.EnemyCamp.profitLevel) {
				case 0:
				case 1:
					saveMoney(state.EnemyCamp);
					AITarget = U"pencil";
					break;
				case 2:
				case 3:

					AITarget = U"triangle";
					break;
				case 4:
				default:
					saveMoney(state.EnemyCamp);
					if (AIMode == 1) {
						//遠距離の召喚
						AITarget = (AIStep == 0) ? U"mechanical_pencil" : U"triangle";
					} else {
						AITarget = U"triangle";
					}
					break;
			}
		} else {
			//攻めプロセス
			switch (state.EnemyCamp.profitLevel) {
				case 0:
				case 1:
					saveMoney(state.EnemyCamp);
					break;
				case 2:
				case 3:
					saveMoney(state.EnemyCamp);
					AITarget=U"triangle";
					break;
				case 4:
				default:
					saveMoney(state.EnemyCamp);
					if (AIStep == 0){
						AITarget=U"pencil_sharpener";
					} else {
						AITarget=U"mechanical_pencil";
					}
					break;
			}
		}
	}
	#if defined(DEBUG) && !defined(HEADLESS)
	if (KeyShift.pressed()) {
		Print << U"Mode: " << AIMode;
		Print << U"Target: " << (AITarget == U"" ? U"資金力強化" : AITarget);
		Print << U"power" + Format(friendPower) + U"/" + Format(enemyPower);
		Print << U"money: " + Format(state.FriendCamp.money) + U" / " + Format(state.FriendCamp.money);
	}
	#endif
	if (AIStep == 3) {
		if (120 * Math::Pow(2, state.EnemyCamp.profitLevel) <= state.EnemyCamp.money)
		{
			savedMoney = 0;
			AIStep = 0;
			AIMode = 0;
			return U"upgrade";
		}
	} else /*召喚を試みる*/if (not (AITarget == U"") and state.GameUnitTypeList.at(AITarget).cost <= state.EnemyCamp.money)
	{
		String target = AITarget;
		AIStep ++;
		AITarget = U"";
		return target;
	}
	return U"";
}

}
