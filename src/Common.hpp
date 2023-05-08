//
//  Common.hpp
//  Stationery Fight
//
//  Created by KKahiru on 2023/04/16.
//

# pragma once

# include <Siv3D.hpp> // OpenSiv3D v0.6.8

# include "logic.hpp"
# include "player.hpp"


using namespace stfi;

// 描画時のテクスチャサイズ
constexpr int texture_size = 120;
// コンフィグ
inline JSON ConfigJson;

struct GameData
{
	// 難易度
	int DifficultyLevel = 2;
};

// シーンマネージャーをAppに割り当てる
using App = SceneManager<String, GameData>;
