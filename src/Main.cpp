# include "Common.hpp"
# include "Scenes/Title.hpp"
# include "Scenes/DifficultySetting.hpp"
# include "Scenes/Game.hpp"
# include "Scenes/RuleExplanation.hpp"

void Main()
{
	//ウィンドウサイズの設定
	Window::Resize(DisplayResolution::HD_1280x720);
	//ウィンドウタイトルの設定
	Window::SetTitle(U"Stationery Fight!");
	// シーンマネージャーを作成
	App manager;
	// シーンの登録
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");
	manager.add<RuleExplanation>(U"RuleExplanation");
	manager.add<DifficultySetting>(U"DifficultySetting");
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetStyle(WindowStyle::Sizable);
	// オーディオアセットの登録
	// 選択音
	AudioAsset::Register(U"Choose", Resource(U"resource/sound/choose.ogg"));
	// 攻撃音
	AudioAsset::Register(U"Hit", Resource(U"resource/sound/hit.ogg"));
	// 召喚音
	AudioAsset::Register(U"Summon", Resource(U"resource/sound/summon.ogg"));
	// 資金力強化ボタンが押せるようになった時の音
	AudioAsset::Register(U"MoneyAvailable", Resource(U"resource/sound/money_available.mp3"));
	// BGM
	AudioAsset::Register(U"BGM", Audio::Stream, Resource(U"resource/sound/bgm.mp3"), Loop::Yes);
	//コンフィグのロード
	ConfigJson = JSON::Load(Resource(U"resource/config.json"));
	if (not ConfigJson)
	{
		throw Error{U"Error! Failed to load a config file."};
	}
	//毎フレームごとの処理
	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
