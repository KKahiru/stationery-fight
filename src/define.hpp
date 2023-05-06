//
//  define.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/16.
//

# pragma once

// 型宣言用
# include <inttypes.h>
# include <string>
# include <parallel_hashmap/phmap.h>
# include <vector>
# include <random>
# include <nlohmann/json.hpp>

// 数値型の定義 
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
// 文字列型の定義
using String = std::u32string;
using StringView = std::u32string_view;
// ハッシュテーブルの定義
template <typename Key, typename Value>
using HashTable = phmap::flat_hash_map<Key, Value>;
// 動的配列の定義
template <typename T>
using Array = std::vector<T>;
// JSONの読み込み用
using json = nlohmann::json;

namespace Math
{
	double Pow(double x, double y);
	uint16 Pow(uint16 x, uint16 y);
	int Pow(int x, uint16 y);
	double Pow(double x, uint16 y);
}
double Random();

json LoadJSON(StringView path);
json LoadJSONWithComments(StringView path);

String Widen(std::string_view s);
std::string Narrow(StringView s);