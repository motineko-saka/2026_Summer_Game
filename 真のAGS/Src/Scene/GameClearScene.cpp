#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "GameClearScene.h"
#include "TitleScene.h"


GameClearScene::GameClearScene(void)
{
}

GameClearScene::~GameClearScene(void)
{
	Release();
}

void GameClearScene::Init(void)
{
}

void GameClearScene::Load(void)
{
}

void GameClearScene::LoadEnd(void)
{
	Init();
}

void GameClearScene::Update(void)
{
	// ƒV[ƒ“‘JˆÚ
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
	}
}

void GameClearScene::Draw(void)
{
	DrawFormatString(100, 200, 0xffffff, "Game Clear");
}

void GameClearScene::Release(void)
{
	DeleteGraph(bgImage_);
}
