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
}

void Button::InitTransform(void)
{
}

void Button::InitCollider(void)
{
}

void Button::InitAnimation(void)
{
}

void Button::InitPost(void)
{
}

void Button::ObjectUpdateProcess(void)
{
	// É{É^ÉìÇÃèàóù
}
