#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/SkyDome.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "PauseScene.h"
#include "TutorialScene.h"
#include "../Application.h"
#include "GameClearScene.h"

TitleScene::TitleScene(void)
{
}

TitleScene::~TitleScene(void)
{
	Release();
}

void TitleScene::Init(void)
{
	// ムービーを再生状態にします
	PlayMovieToGraph(movTitle_);

	// UI 選択初期化
	uiSelect_ = 0;
}

void TitleScene::Load(void)
{
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;

	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::TITLE_PUSH_SPACE).handleId_;

	imgHondana_ = resMng_.Load(ResourceManager::SRC::HONDANA_IMG).handleId_;

	// 動画読み込み
	movTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_MOV).handleId_;

	imgUIStart_ = resMng_.Load(ResourceManager::SRC::UI_GAMESTART).handleId_;
	imgUITutorial_ = resMng_.Load(ResourceManager::SRC::UI_TUTORIAL).handleId_;
	imgUIExit_ = resMng_.Load(ResourceManager::SRC::UI_EXIT).handleId_;
}

void TitleScene::LoadEnd(void)
{
	Init();
}

void TitleScene::Update(void)
{
	// ポーズ画面に遷移
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_Q))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClearScene>());
	}

	// 十字キー / パッドで選択移動
	// 左に移動
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_LEFT)
		|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT))
	{
		uiSelect_ = (std::min)(uiSelect_ + 1, 2);
	}

	// 右に移動
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RIGHT)
		|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		uiSelect_ = (std::max)(uiSelect_ - 1, 0);
	}

	// 決定
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE) || InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RETURN)
		|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		switch (uiSelect_)
		{
		case 0: // Start
			//SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
			break;
		case 1: // Tutorial
			SceneManager::GetInstance()->ChangeScene(std::make_shared<TutorialScene>());
			break;
		case 2: // Exit
			PostQuitMessage(0);
			break;
		default:
			break;
		}
	}
}

void TitleScene::Draw(void)
{
	int screenX = Application::SCREEN_SIZE_X;
	int screenY = Application::SCREEN_SIZE_Y;

	DrawRotaGraph(screenX / 2, screenY / 2, 1.85f, 0.0f, imgHondana_, true);

	unsigned int white = GetColor(255, 255, 255);

	// UI の X 位置（Start:右, Tutorial:中央, Exit:左）
	const float posStartX = screenX * 0.55f;
	const float posTutorialX = screenX * 0.45f;
	const float posExitX = screenX * 0.35f;
	const float posY = screenY / 2;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              //野本アレンを倒すために俺は生まれてきたんだ

	// 選択中は拡大して分かりやすくする
	const float baseScale = 0.8f;
	const float selScale = 0.9f;

	float scaleStart = (uiSelect_ == 0) ? selScale : baseScale;
	float scaleTutorial = (uiSelect_ == 1) ? selScale : baseScale;
	float scaleExit = (uiSelect_ == 2) ? selScale : baseScale;

	// 描画（選択中は少し上にずらす）
	DrawRotaGraph(static_cast<int>(posStartX), static_cast<int>(posY - (uiSelect_ == 0 ? 10 : 0)), scaleStart, 0.0f, imgUIStart_, true);
	DrawRotaGraph(static_cast<int>(posTutorialX), static_cast<int>(posY - (uiSelect_ == 1 ? 10 : 0)), scaleTutorial, 0.0f, imgUITutorial_, true);
	DrawRotaGraph(static_cast<int>(posExitX), static_cast<int>(posY - (uiSelect_ == 2 ? 10 : 0)), scaleExit, 0.0f, imgUIExit_, true);

	// --------------------------------------------
	//DrawFormatString(0, 0, GetColor(0, 0, 0), "ゲームスタート");

	//DrawExtendGraph(0, 0, screenX, screenY, movTitle_, FALSE);

	// 2D描画（ムービーの上にUIを重ねる）
	DrawRotaGraph(IMG_TITLE_POS_X, IMG_TITLE_POS_Y, 0.45f, 0.0f, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_SPACE_POS_X, IMG_PUSH_SPACE_POS_Y, 1.0f, 0.0f, imgPushSpace_, true);
}

void TitleScene::Release(void)
{

}