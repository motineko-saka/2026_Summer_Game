#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System 
{
	static const std::unordered_map<SoundID, std::string> Table = 
	{
		{ SoundID::SE_MENU_DECISION, "Data/Sound/SE/Menu_Decision_SE.mp3" },
		{ SoundID::SE_MENU_CHOICE, "Data/Sound/SE/Menu_Choice_SE.mp3" },
		{ SoundID::SE_MENU_OPEN, "Data/Sound/SE/Menu_Open_SE.mp3" },
	};
}

// タイトル画面用
namespace SoundTable_Title
{
	static const std::unordered_map<SoundID, std::string> Table =
	{

	};
}

// ゲーム画面用
namespace SoundTable_Game 
{
	static const std::unordered_map<SoundID, std::string> Table = 
	{
		{SoundID::BGM_GAME, "Data/Sound/BGM/GameScene_BGM.mp3"},
		{SoundID::SE_WALK, "Data/Sound/SE/Walk_SE.mp3"},
		{SoundID::SE_RUN, "Data/Sound/SE/Run_SE.mp3"},
		{SoundID::SE_HAVE_IRON, "Data/Sound/SE/Have_Iron_SE.mp3"},
		{SoundID::SE_HAVE_WOOD, "Data/Sound/SE/Have_Wood_SE.mp3"},
		{SoundID::SE_PUT_WOOD, "Data/Sound/SE/Put_Wood_SE.mp3"},
		{SoundID::SE_BROKE, "Data/Sound/SE/Broke_SE.mp3"},
		{SoundID::SE_WARNING, "Data/Sound/SE/Warning_SE.mp3"},
		{SoundID::SE_SUCCESS, "Data/Sound/SE/Success_SE.mp3"},
	};
}

namespace SoundTable_GameClear
{
	static const std::unordered_map<SoundID, std::string> Table =
	{
		{SoundID::SE_GAME_CLEAR, "Data/Sound/SE/Game_Clear_SE.mp3"}
	};
}

namespace SoundTable_GameOver
{
	static const std::unordered_map<SoundID, std::string> Table =
	{
		{SoundID::SE_GAME_OVER, "Data/Sound/SE/Game_Over_SE.mp3"}
	};

}

namespace SoundTable_Tutorial
{
	static const std::unordered_map < SoundID, std::string> Table =
	{
		{SoundID::BGM_TUTORIAL, "Data/Sound/BGM/GameScene_BGM.mp3"},
	};
}