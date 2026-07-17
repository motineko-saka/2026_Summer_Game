#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "GameClearScene.h"
#include "TitleScene.h"
#include "../Manager/ResourceManager.h"
#include "../Application.h"


GameClearScene::GameClearScene(void)
{
}

GameClearScene::~GameClearScene(void)
{
	Release();
}

void GameClearScene::Init(void)
{
	bgImage_ = resMng_.Load(ResourceManager::SRC::GAME_CLEAR).handleId_;
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
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE) ||
		InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
	}
}

void GameClearScene::Draw(void)
{
	int screenX = Application::SCREEN_SIZE_X;
	int screenY = Application::SCREEN_SIZE_Y;

	DrawFormatString(100, 200, 0xffffff, "Game Clear");

	DrawRotaGraph(screenX / 2, screenY / 2, 1.02f, 0.0f, bgImage_, true);
}

void GameClearScene::Release(void)
{
}
