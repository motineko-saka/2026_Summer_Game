#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Application.h"
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
	bgImage_ = LoadGraph("Data/Image/GameClear.png");
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
	//DrawGraph(0, 0, bgImage_,true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2,
		0.0f, 1.0f, bgImage_, true);
}

void GameClearScene::Release(void)
{
	DeleteGraph(bgImage_);
}
