#include "Object.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Resource.h"
#include "../../Manager/SceneManager.h"
#include "../../Application.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "../../Manager/InputManager.h"

Object::Object(GameScene::WORLD world)
{
	viewWorld_ = world;
	world_ = world;
}

Object::~Object()
{
}

void Object::Update(void)
{
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_C))
	{
		viewWorld_ = (viewWorld_ == GameScene::WORLD::LEFT) ? GameScene::WORLD::RIGHT : GameScene::WORLD::LEFT;
	}
}

void Object::Draw(void)
{
	if (viewWorld_ == world_)
	{
		ActorBase::Draw();
	}
}

void Object::Release(void)
{
	ActorBase::Release();
}

void Object::InitLoad(void)
{
	transform_.SetModel(resMng_.Load(ResourceManager::SRC::CUBE).handleId_);
}

void Object::InitTransform(void)
{
	// モデルの位置
	transform_.scl = { 1.0f, 1.0f, 1.0f };
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = { 0.0f, 80.0f, -10.0f };
	transform_.Update();
}

void Object::InitCollider(void)
{
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::OBJECT, &transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void Object::InitAnimation(void)
{
}

void Object::InitPost(void)
{
}
