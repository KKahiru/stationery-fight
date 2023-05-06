# include "define.hpp"

# include <cmath>
# include <fstream>

namespace Math
{
	double Pow(double x, double y)
	{
		return std::pow(x, y);
	}
	uint16 Pow(uint16 x, uint16 y)
	{
		return std::pow(x, y);
	}
	int Pow(int x, uint16 y)
	{
		return std::pow(x, y);
	}
	double Pow(double x, uint16 y)
	{
		return std::pow(x, y);
	}
}
// 乱数生成器の設定
namespace RandomGen{
	std::random_device rd;
	std::mt19937 mt(rd());	
	std::uniform_real_distribution<double> dist(0.0, 1.0);
}

double Random()
{
	return RandomGen::dist(RandomGen::mt);
}

json LoadJSON(StringView path)
{
	std::ifstream f(path);
	return json::parse(f);
}

json LoadJSONWithComments(StringView path)
{
	std::ifstream f(path);
	return json::parse(f,
					   /* callback */ nullptr,
					   /* allow_exceptions */ true,
					   /* ignore_comments */ true);
}

String Widen(std::string_view s)
{
	return String(s.begin(), s.end());
}

std::string Narrow(StringView s)
{
	return std::string(s.begin(), s.end());
}
