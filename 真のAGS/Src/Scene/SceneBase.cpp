#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "SceneBase.h"

SceneBase::SceneBase(void)
	:
	resMng_(ResourceManager::GetInstance())
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Load(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}

void SceneBase::Release(void)
{
}

