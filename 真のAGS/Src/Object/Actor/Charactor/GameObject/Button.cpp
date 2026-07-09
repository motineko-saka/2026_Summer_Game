#include "../../../../Manager/ResourceManager.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Common/Quaternion.h"
#include "Button.h"

Button::Button(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type)
	: 
	ObjectBase(world, ansVec, type)
{
}

void Button::InitLoad(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BUTTON));
	
	isHoldable_ = true;
}

void Button::ObjectUpdateProcess(void)
{
	
}
