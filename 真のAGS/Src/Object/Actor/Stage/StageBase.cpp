#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../Collider/ColliderModel.h"
#include "StageBase.h"
#include "Stage.h"

StageBase::StageBase()
	:
	ActorBase()
{
}

StageBase::~StageBase(void)
{
}

void StageBase::Update(void)
{
	ActorBase::Update();
}

void StageBase::Draw(void)
{
	ActorBase::Draw();
}

void StageBase::Release(void)
{
	ActorBase::Release();
}

void StageBase::InitTransform(void)
{
}

void StageBase::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::STAGE, &transform_);

	// 除外フレーム設定
	for (const std::string& name : EXCLUDE_FRAME_NAMES)
	{
		colModel->AddExcludeFrameIds(name);
	}

	// 許可フレーム設定
	for (const std::string& name : TARGET_FRAME_NAMES)
	{
		colModel->AddTargetFrameIds(name);
	}


	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void StageBase::InitAnimation(void)
{
}

void StageBase::InitPost(void)
{
}
