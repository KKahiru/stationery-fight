# include "Common.hpp"
# include "Scenes/Title.hpp"
# include "Scenes/DifficultySetting.hpp"
# include "Scenes/Game.hpp"
# include "Scenes/RuleException.hpp"

void Main()
{
	//ウィンドウサイズの設定
	Window::Resize(960,540);
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
	//オーディオアセットの登録
	AudioAsset::Register(U"choose", Resource(U"resource/sound/choose.ogg"));
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
