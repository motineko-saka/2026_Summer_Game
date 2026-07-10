#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include <DxLib.h>
#include "GameOverScene.h"
#include "TitleScene.h"
#include "../Manager/ResourceManager.h"
#include "../Application.h"

GameOverScene::GameOverScene(void)
{
}

GameOverScene::~GameOverScene(void)
{
}

void GameOverScene::Init(void)
{
	bgImage_ = resMng_.Load(ResourceManager::SRC::GAME_OVER).handleId_;
}

void GameOverScene::Load(void)
{
}

void GameOverScene::LoadEnd(void)
{
	Init();
}

void GameOverScene::Update(void)
{
	// ƒV[ƒ“‘JˆÚ
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
	}
}

void GameOverScene::Draw(void)
{
	int screenX = Application::SCREEN_SIZE_X;
	int screenY = Application::SCREEN_SIZE_Y;

	DrawFormatString(100, 200, 0xffffff, "Game Clear");

	DrawRotaGraph(screenX / 2, screenY / 2, 1.02f, 0.0f, bgImage_, true);
}

void GameOverScene::Release(void)
{
}
