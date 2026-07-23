#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "GameClearScene.h"
#include "TitleScene.h"
#include "../Manager/ResourceManager.h"
#include "../Application.h"
#include "../Audio/AudioManager.h"


GameClearScene::GameClearScene(void)
{
}

GameClearScene::~GameClearScene(void)
{
	Release();
}

void GameClearScene::Init(void)
{
	// ゲームクリアー画像の読み込み
	bgImage_ = resMng_.Load(ResourceManager::SRC::GAME_CLEAR).handleId_;

	// ゲームクリアーSEの再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_GAME_CLEAR);
}

void GameClearScene::Load(void)
{
	// AudioManagerのインスタンスを作成して初期化し、ゲームクリアーシーンのサウンドをロードする
	AudioManager::GetInstance()->CreateInstance();
	AudioManager::GetInstance()->Init();
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_CLEAR);
}

void GameClearScene::LoadEnd(void)
{
	Init();
}

void GameClearScene::Update(void)
{
	// シーン遷移
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE) ||
		InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		// ゲームクリアシーンからタイトルシーンに遷移する際に、AudioManagerのインスタンスを削除してリセットする
		AudioManager::GetInstance()->StopSE();
		AudioManager::GetInstance()->DeleteAll();
		AudioManager::DeleteInstance();

		// タイトルシーンに遷移
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
