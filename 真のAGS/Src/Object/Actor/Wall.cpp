#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Resource.h"
#include "../../Manager/SceneManager.h"
#include "../Collider/ColliderModel.h"
#include "../../Renderer/ModelMaterial.h"
#include "../../Renderer/ModelRenderer.h"
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
	time_ += (1.0f * SceneManager::GetInstance()->GetDeltaTime() * 0.5f);
	ActorBase::Update();
}

void Wall::Draw(void)
{
	vertexRenderer_->Draw();
	//ActorBase::Draw();
}

void Wall::Release(void)
{
	ActorBase::Release();
}

void Wall::InitLoad(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::GATE));

	vertexMaterial_ = std::make_unique<ModelMaterial>(
		"GateVS.cso", 1,
		"GatePS.cso", 4);

	auto dir = GetLightDirection();
	vertexMaterial_->AddConstBufPS({ dir.x, dir.y, dir.z, 0.0f });

	auto cameraPos = GetCameraPosition();
	vertexMaterial_->AddConstBufPS({ cameraPos.x, cameraPos.y, cameraPos.z, time_ });

	vertexMaterial_->AddConstBufPS({ 1.0f, 1.0f, 1.0f, 1.0f });
	vertexMaterial_->AddConstBufPS({ 0.0f, 0.0f, 0.0f, 1.0f });

	vertexMaterial_->SetTextureAddress(ModelMaterial::TEXADDRESS::WRAP);

	vertexRenderer_ = std::make_unique<ModelRenderer>(transform_.modelId, *vertexMaterial_);
}

void Wall::InitTransform(void)
{
	transform_.scl = STAGE_DEFAULT_SCALE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();

	// 回転を先に適用してから位置を設定
	if(isRot_)
	{
		transform_.quaRot = Quaternion::AngleAxis(DX_PI_F / 2.0f, AsoUtility::AXIS_Y);
	}

	transform_.pos = tempPos_;
	transform_.Update();
}

void Wall::InitCollider(void)
{
	// DxLib側のコリジョンセットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダー
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
