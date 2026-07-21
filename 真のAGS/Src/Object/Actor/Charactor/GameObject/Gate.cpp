#include "../../../../Application.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "../../../../Renderer/ModelMaterial.h"
#include "../../../../Renderer/ModelRenderer.h"
#include "Gate.h"

Gate::Gate(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	:
	ObjectBase(world, ansVec, type)
{
}

void Gate::Draw()
{
	auto dir = GetLightDirection();
	//vertexMaterial_->SetConstBufPS(0, { dir.x, dir.y, dir.z, dissolveTime_ });

	auto cameraPos = GetCameraPosition();
	vertexMaterial_->SetConstBufPS(1, { cameraPos.x, cameraPos.y, cameraPos.z, time_ });
	//vertexMaterial_->SetConstBufPS(1, { cameraPos.x, cameraPos.y, cameraPos.z, 0 });

	vertexRenderer_->Draw();
	//ActorBase::Draw();
}

void Gate::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::GATE));
	isHoldable_ = true;

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

void Gate::InitObjTrans(void)
{
	tag_ = ColliderBase::TAG::OBJECT;
	capsule_r = 100.0f;
}

void Gate::InitPost(void)
{
	isAnswerObject_ = false;
	isGrav = false;

	time_ = 0.0f;
	dissolveTime_ = 0.0f;
}

void Gate::ObjectUpdateProcess(void)
{
	time_ += (1.0f * SceneManager::GetInstance()->GetDeltaTime() * 0.5f);

	if (isButtomPushed_)
	{
		dissolveTime_ += (1.0f * SceneManager::GetInstance()->GetDeltaTime() * 0.5f);
	}
}
