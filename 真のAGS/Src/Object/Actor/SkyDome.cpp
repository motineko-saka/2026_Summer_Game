#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Resource.h"
#include "../../Manager/SceneManager.h"
#include "SkyDome.h"

SkyDome::SkyDome(const Transform& transform)
	:
	state_(),
	followTransform_(transform),
	ActorBase()
{
}

SkyDome::~SkyDome(void)
{
}

void SkyDome::Update(void)
{

	switch (state_)
	{
	case SkyDome::STATE::NONE:
		UpdateNone();
		break;
	case SkyDome::STATE::STAY:
		UpdateStay();
		break;
	case SkyDome::STATE::FOLLOW:
		UpdateFollow();
		break;
	}
}

void SkyDome::Draw(void)
{
	SetUseLighting(false);
	ActorBase::Draw();
	
	//MV1DrawModel(transform_.modelId);
	SetUseLighting(true);
}

void SkyDome::Release(void)
{
	ActorBase::Release();
}

void SkyDome::InitLoad(void)
{
	transform_.SetModel(resMng_.Load(ResourceManager::SRC::SKY_DOME).handleId_);
}

void SkyDome::InitTransform(void)
{
	transform_.scl = DEFAULT_SKYDOME_SCALE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Euler(DEFAULT_ROT_LOCAL);
	transform_.pos = DEFAULT_SKYDOME_POS;
	transform_.Update();

}

void SkyDome::InitCollider(void)
{
}

void SkyDome::InitAnimation(void)
{
}

void SkyDome::InitPost(void)
{
	// Zバッファ無効(突き抜け対策)
	MV1SetUseZBuffer(transform_.modelId, false);
	MV1SetWriteZBuffer(transform_.modelId, false);
}

void SkyDome::ChangeState(STATE state)
{
	state_ = state;
	switch (state_)
	{
	case SkyDome::STATE::NONE:
		ChangeStateNone();
		break;
	case SkyDome::STATE::STAY:
		ChangeStateStay();
		break;
	case SkyDome::STATE::FOLLOW:
		ChangeStateFollow();
		break;
	}
}

void SkyDome::ChangeStateNone(void)
{
}

void SkyDome::ChangeStateStay(void)
{
}

void SkyDome::ChangeStateFollow(void)
{
	// 追従開始
	transform_.pos = followTransform_.pos;
	transform_.Update();
}

void SkyDome::UpdateNone(void)
{
}

void SkyDome::UpdateStay(void)
{
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-0.1f), AsoUtility::AXIS_Y));

	transform_.pos = followTransform_.pos;
	transform_.Update();
}

void SkyDome::UpdateFollow(void)
{
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-0.1f), AsoUtility::AXIS_Y));

	transform_.pos = followTransform_.pos;
	transform_.Update();
}
