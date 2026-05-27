#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "GameClearScene.h"

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

void GameClearScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
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
