#include "Button.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "../../../../Object/Actor/Charactor/Player.h"


Button::Button(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	: 
	ObjectBase(world, ansVec, type, true)
{
}

void Button::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BUTTON));
}

void Button::ObjectUpdateProcess()
{
}
