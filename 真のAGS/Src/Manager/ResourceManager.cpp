#include <DxLib.h>
#include <sstream>
#include <Windows.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

namespace
{
	using SRC = ResourceManager::SRC;
	using RES_T = Resource::TYPE;

	// リソースの登録情報
	struct ResourceDef
	{
		SRC src;
		RES_T type;
		const char* path;	// 種類ごとのフォルダからの相対パス
	};

	const ResourceDef RESOURCE_DEFS[] =
	{
		// タイトル
		{ SRC::TITLE_IMG,        RES_T::IMG,   "Title.png" },
		{ SRC::TITLE_PUSH_SPACE, RES_T::IMG,   "PushSpace.png" },
		{ SRC::TITLE_MOV,        RES_T::MOVIE, "Title.mp4" },
		{ SRC::HONDANA_IMG,      RES_T::IMG,   "bookshelf_title.png" },

		// UI
		{ SRC::UI_GAMESTART, RES_T::IMG, "game_start.png" },
		{ SRC::UI_TUTORIAL,  RES_T::IMG, "tutorial.png" },
		{ SRC::UI_EXIT,      RES_T::IMG, "exit.png" },
		{ SRC::GAME_CLEAR,   RES_T::IMG, "GameClear.png" },
		{ SRC::GAME_OVER,    RES_T::IMG, "GameOver.png" },

		// ステージ
		{ SRC::PIT_FALL_PLANET, RES_T::MODEL, "stage/PitfallPlanet/PitfallPlanet.mv1" },
		{ SRC::SPHERE_PLANET,   RES_T::MODEL, "stage/SpherePlanet/SpherePlanet.mv1" },
		{ SRC::TUTORIAL_STAGE,  RES_T::MODEL, "stage/tutorial_stage.mv1" },
		{ SRC::MAIN_STAGE,      RES_T::MODEL, "stage/main_stage.mv1" },
		{ SRC::SKY_DOME,        RES_T::MODEL, "SkyDome/Skydome.mv1" },

		// プレイヤー
		{ SRC::PLAYER,        RES_T::MODEL, "Player/boy.mv1" },
		{ SRC::PLAYER_SHADOW, RES_T::IMG,   "Shadow.png" },

		// 敵
		{ SRC::ENEMY_RAT,   RES_T::MODEL, "Enemy/Rat/Rat.mv1" },
		{ SRC::ENEMY_ROBOT, RES_T::MODEL, "Enemy/Robot/Robot.mv1" },
		{ SRC::VIEW_RANGE,  RES_T::MODEL, "Enemy/Robot/Cone.mv1" },

		// オブジェクト
		{ SRC::BARREL,       RES_T::MODEL, "object/barrel.mv1" },
		{ SRC::CUBE,         RES_T::MODEL, "object/rocking_horse.mv1" },
		{ SRC::WALL,         RES_T::MODEL, "Cube.mv1" },
		{ SRC::WOODBOX,      RES_T::MODEL, "object/wooden_box.mv1" },
		{ SRC::BUTTON,       RES_T::MODEL, "Cube.mv1" },
		{ SRC::PRESS_BUTTON, RES_T::MODEL, "object/rocking_horse.mv1" },
		{ SRC::GEAR,         RES_T::MODEL, "object/Gear.mv1" },
		{ SRC::ROCK,         RES_T::MODEL, "object/tree.mv1" },
		{ SRC::AXE,          RES_T::MODEL, "object/Axe.mv1" },
		{ SRC::GATE,         RES_T::MODEL, "object/Gate/GateMist.mv1" },
		{ SRC::Chest,        RES_T::MODEL, "object/Chest.mv1" },
		{ SRC::OPENCHEST,    RES_T::MODEL, "object/ChestOpen.mv1" },

		// 絵の具
		{ SRC::ENOGU1, RES_T::IMG, "paint/wave_hand.png" },
		{ SRC::ENOGU2, RES_T::IMG, "paint/check.png" },
		{ SRC::ENOGU3, RES_T::IMG, "paint/point.png" },
		{ SRC::ENOGU4, RES_T::IMG, "paint/question.png" },
		{ SRC::ENOGU5, RES_T::IMG, "paint/ここに名前.png" },	// ← 2.png の新しい名前
		{ SRC::ENOGU6, RES_T::IMG, "paint/hold_barrel.png" },
		{ SRC::ENOGU7, RES_T::IMG, "paint/happy.png" },
		{ SRC::ENOGU8, RES_T::IMG, "paint/clear.png" },

		// ヒント
		{ SRC::HINTO,  RES_T::IMG, "paint/hint_button.png" },
		{ SRC::HINTO2, RES_T::IMG, "paint/hint_chest.png" },
	};

	// 種類ごとの基準フォルダ
	std::string GetBasePath(RES_T type)
	{
		switch (type)
		{
		case RES_T::IMG:
		case RES_T::IMGS:		return Application::PATH_IMAGE;
		case RES_T::MODEL:		return Application::PATH_MODEL;
		case RES_T::EFFEKSEER:	return Application::PATH_EFFECT;
		case RES_T::MOVIE:		return Application::PATH_MOV;
		default:				return "";
		}
	}
}

// ---------------------------------------------------------------
// シングルトン
// ---------------------------------------------------------------
void ResourceManager::CreateInstance(void)
{
	if (instance_ != nullptr) return;

	instance_ = new ResourceManager();
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

// ---------------------------------------------------------------
// 登録・解放
// ---------------------------------------------------------------
void ResourceManager::Init(void)
{
	for (const auto& def : RESOURCE_DEFS)
	{
		resourcesMap_.emplace(
			def.src,
			std::make_unique<Resource>(def.type, GetBasePath(def.type) + def.path));
	}
}

void ResourceManager::Release(void)
{
	for (SRC src : loadedSrcs_)
	{
		auto it = resourcesMap_.find(src);
		if (it != resourcesMap_.end())
		{
			it->second->Release();
		}
	}
	loadedSrcs_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();

	for (auto& [src, res] : resourcesMap_)
	{
		res->Release();
	}
	resourcesMap_.clear();

	// 自分自身を削除する
	if (instance_ == this) instance_ = nullptr;
	delete this;
}

// ---------------------------------------------------------------
// ロード
// ---------------------------------------------------------------
const Resource& ResourceManager::Load(SRC src)
{
	return LoadInternal(src);
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = LoadInternal(src);

	// モデル以外、または読み込み失敗時は複製しない
	if (res.type_ != Resource::TYPE::MODEL || res.handleId_ == -1)
	{
		return -1;
	}

	const int duplicateId = MV1DuplicateModel(res.handleId_);
	if (duplicateId != -1)
	{
		res.duplicateModelIds_.push_back(duplicateId);
	}
	return duplicateId;
}

Resource& ResourceManager::LoadInternal(SRC src)
{
	auto it = resourcesMap_.find(src);
	if (it == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	Resource& res = *it->second;

	// 初回のみ読み込む
	if (loadedSrcs_.insert(src).second)
	{
		res.Load();
	}

	return res;
}

// ---------------------------------------------------------------
// デバッグ
// ---------------------------------------------------------------
void ResourceManager::DebugDump(void) const
{
	std::ostringstream oss;
	oss << "ResourceManager Dump: resourcesMap_=" << resourcesMap_.size()
		<< " loaded=" << loadedSrcs_.size() << "\n";

	for (const auto& [src, res] : resourcesMap_)
	{
		oss << " SRC=" << static_cast<int>(src)
			<< " path=" << res->path_
			<< " type=" << static_cast<int>(res->type_)
			<< " handle=" << res->handleId_
			<< " dupCount=" << res->duplicateModelIds_.size()
			<< "\n";
	}

	OutputDebugStringA(oss.str().c_str());
}