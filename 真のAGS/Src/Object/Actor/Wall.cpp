#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Resource.h"
#include "../../Manager/SceneManager.h"
#include "../Collider/ColliderModel.h"
#include "Wall.h"


Wall::Wall(void)
{
}

Wall::~Wall(void)
{
}

void Wall::Update(void)
{
	ActorBase::Update();
}

void Wall::Draw(void)
{
	ActorBase::Draw();
}

void Wall::Release(void)
{
	ActorBase::Release();
}

void Wall::InitLoad(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::WALL));
}

void Wall::InitTransform(void)
{
	transform_.scl = STAGE_DEFAULT_SCALE;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = STAGE_DEFAULT_POS;
	transform_.Update();
}

void Wall::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::STAGE, &transform_);

	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void Wall::InitAnimation(void)
{
	
}

void Wall::InitPost(void)
{
}
