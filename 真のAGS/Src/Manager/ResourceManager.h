#pragma once
#include <map>
#include <memory>
#include <set>
#include <string>
#include "Resource.h"

class ResourceManager
{
public:

	// リソース名
	enum class SRC
	{
		TITLE_IMG,
		TITLE_PUSH_SPACE,
		PIT_FALL_PLANET,
		SPHERE_PLANET,
		PLAYER,
		TUTORIAL_STAGE,
		MAIN_STAGE,
		SKY_DOME,
		PLAYER_SHADOW,
		ENEMY_RAT,
		ENEMY_ROBOT,
		VIEW_RANGE,
		BARREL,
		WALL,
		WOODBOX,
		BUTTON,
		PRESS_BUTTON,
		CUBE,
		GEAR,
		ROCK,
		AXE,
		GATE,
		TITLE_MOV,
		HONDANA_IMG,
		UI_GAMESTART,
		UI_TUTORIAL,
		UI_EXIT,
		Chest,
		OPENCHEST,
		ENOGU1,
		ENOGU2,
		ENOGU3,
		ENOGU4,
		ENOGU5,
		ENOGU6,
		ENOGU7,
		ENOGU8,
		GAME_CLEAR,
		GAME_OVER,
		HINTO,
		HINTO2,
	};

	// 明示的にインスタンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化（リソースの登録）
	void Init(void);

	// 解放（シーン切替時に一旦解放）
	void Release(void);

	// リソースの完全破棄（インスタンスも削除される）
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード（モデル用）
	int LoadModelDuplicate(SRC src);

	// デバッグ出力
	void DebugDump(void) const;

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, std::unique_ptr<Resource>> resourcesMap_;

	// 読み込み済みリソース
	std::set<SRC> loadedSrcs_;

	// 未登録時に返すダミー
	Resource dummy_;

	ResourceManager(void) = default;
	~ResourceManager(void) = default;

	// コピー・ムーブ禁止
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

	// 内部ロード
	Resource& LoadInternal(SRC src);
};