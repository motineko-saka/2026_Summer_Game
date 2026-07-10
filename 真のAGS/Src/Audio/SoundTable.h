#pragma once
#include <string>
#include <unordered_map>

// サウンドID
enum class SoundID
{
	BGM_TITLE,
	BGM_GAME,
	BGM_TUTORIAL,

	SE_MENU_DECISION,
	SE_MENU_CHOICE,
	SE_MENU_OPEN,

	SE_WALK,
	SE_RUN,
	SE_HAVE_IRON,
	SE_HAVE_WOOD,
	SE_PUT_WOOD,
	SE_BROKE,
	SE_WARNING,
	SE_SUCCESS,

	SE_GAME_CLEAR,

	SE_GAME_OVER,

	SE_TUTORIAL_WALK,

};

// 読み込むシーン
enum class LoadScene
{
	SYSTEM,		// システム
	TITLE,		// タイトル
	GAME,		// ゲーム
	GAME_CLEAR,	// ゲームクリア
	GAME_OVER,	// ゲームオーバー
	GAME_TUTORIAL,
};

// サウンド属性
struct SoundData
{
	std::string path;	// サウンドのパス
};

// サウンドテーブル（名称被りや拡張性の為にクラス化)
class SoundTable
{
public:
	// サウンドテーブルのマップ
	// サウンドID から サウンドパスを取得
	static const std::unordered_map<SoundID, SoundData> Table;
};
