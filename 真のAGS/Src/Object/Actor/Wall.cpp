#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Resource.h"
#include "../../Manager/SceneManager.h"
#include "../Collider/ColliderModel.h"
#include "Wall.h"

Wall::Wall(VECTOR pos, bool isRot)
	:
	tempPos_(pos),
	isRot_(isRot)
{
}

Wall::~Wall(void)
{
	Release();
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

	if(isRot_)
	{
		transform_.quaRot = Quaternion::AngleAxis(DX_PI_F / 2.0f, AsoUtility::AXIS_Y);
	}

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = tempPos_;
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
