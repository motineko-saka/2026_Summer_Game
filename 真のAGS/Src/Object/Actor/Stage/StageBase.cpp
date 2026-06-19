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
	if(stageType_ != STAGE_TYPE::ANSWER_STAGE)
	{
		ActorBase::Draw();
	}
}

void StageBase::Release(void)
{
	ActorBase::Release();
}

void StageBase::DrawAtOffset(const VECTOR& offset)
{
	// 元の位置を保持
	const VECTOR prevPos = transform_.pos;

	// オフセット位置へ移動してマトリクス更新
	transform_.pos = VAdd(transform_.pos, offset);
	transform_.Update();

	// 実際の描画
	if (stageType_ != STAGE_TYPE::ANSWER_STAGE)
	{
		ActorBase::Draw();
	}

	// 元の位置に戻してマトリクス更新
	transform_.pos = prevPos;
	transform_.Update();
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
