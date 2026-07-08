#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;
	static std::string PATH_MOV = Application::PATH_MOV;

	Resource* res;

	// タイトル画像
	res = new RES(RES_T::IMG, PATH_IMG + "Title.png");
	resourcesMap_.emplace(SRC::TITLE_IMG, res);

	// 
	res = new RES(RES_T::IMG, PATH_IMG + "PushSpace.png");
	resourcesMap_.emplace(SRC::TITLE_PUSH_SPACE, res);

	// 
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/PitfallPlanet/PitfallPlanet.mv1");
	resourcesMap_.emplace(SRC::PIT_FALL_PLANET, res);

	// 
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/SpherePlanet/SpherePlanet.mv1");
	resourcesMap_.emplace(SRC::SPHERE_PLANET, res);

	// プレイヤーモデル
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/Remy.mv1");
	resourcesMap_.emplace(SRC::PLAYER, res);

	// ステージモデル
	//チュートリアルステージ
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/NOMOTOAREN 1.mv1");
	resourcesMap_.emplace(SRC::TUTORIAL_STAGE, res);

	// メインステージ
	//res = new RES(RES_T::MODEL, PATH_MDL + "Stage/Stage.mv1");
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/NomotoPark.mv1");
	resourcesMap_.emplace(SRC::MAIN_STAGE, res);

	// スカイドーム
	res = new RES(RES_T::MODEL, PATH_MDL + "SkyDome/Skydome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, res);

	// プレイヤー影
	res = new RES(RES_T::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, res);

	// ネズミ
	res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/Rat/Rat.mv1");
	resourcesMap_.emplace(SRC::ENEMY_RAT, res);

	// ロボット
	res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/Robot/Robot.mv1");
	resourcesMap_.emplace(SRC::ENEMY_ROBOT, res);

	// 視野
	res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/Robot/Cone.mv1");
	resourcesMap_.emplace(SRC::VIEW_RANGE, res);

	// 樽
	res = new RES(RES_T::MODEL, PATH_MDL + "Object/barrel.mv1");
	resourcesMap_.emplace(SRC::BARREL, res);

	// 四角
	res = new RES(RES_T::MODEL, PATH_MDL + "Object/mokuba.mv1");
	resourcesMap_.emplace(SRC::CUBE, res);

	// 壁
	res = new RES(RES_T::MODEL, PATH_MDL + "Cube.mv1");
	resourcesMap_.emplace(SRC::WALL, res);

	// 木箱
	res = new RES(RES_T::MODEL, PATH_MDL + "Object/wooden_box.mv1");
	resourcesMap_.emplace(SRC::WOODBOX, res);

	// ボタン
	res = new RES(RES_T::MODEL, PATH_MDL + "Cube.mv1");
	resourcesMap_.emplace(SRC::BUTTON, res);

	// 歯車
	res = new RES(RES_T::MODEL, PATH_MDL + "Object/Gear.mv1");
	resourcesMap_.emplace(SRC::GEAR, res);

	// タイトルムービー
	res = new RES(RES_T::MOVIE, PATH_MOV + "Title.mp4");
	resourcesMap_.emplace(SRC::TITLE_MOV, res);

	// 本棚画像
	res = new RES(RES_T::IMG, PATH_IMG + "hondanaTitle.png");
	resourcesMap_.emplace(SRC::HONDANA_IMG, res);

	// UIゲームスタート
	res = new RES(RES_T::IMG, PATH_IMG + "GameStart.png");
	resourcesMap_.emplace(SRC::UI_GAMESTART, res);

	// UIチュートリアル
	res = new RES(RES_T::IMG, PATH_IMG + "Tyutoriar.png");
	resourcesMap_.emplace(SRC::UI_TUTORIAL, res);

	// UI終了
	res = new RES(RES_T::IMG, PATH_IMG + "End.png");
	resourcesMap_.emplace(SRC::UI_EXIT, res);

	// 宝箱
	res = new RES(RES_T::MODEL, PATH_MDL + "Object/Box.mv1");
	resourcesMap_.emplace(SRC::Chest, res);

	// 絵の具
	res = new RES(RES_T::IMG, PATH_IMG + "Enogukunn/kyara.png");
	resourcesMap_.emplace(SRC::ENOGU, res);

	// 絵の具2
	res = new RES(RES_T::IMG, PATH_IMG + "Enogukunn/ai.png");
	resourcesMap_.emplace(SRC::ENOGU2, res);
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	const Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{
	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;
}