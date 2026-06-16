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
}

void TitleScene::Load(void)
{
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;

	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::TITLE_PUSH_SPACE).handleId_;

	imgHondana_ = resMng_.Load(ResourceManager::SRC::HONDANA_IMG).handleId_;

	// 動画読み込み
	movTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_MOV).handleId_;
}

void TitleScene::LoadEnd(void)
{
	Init();
}

void TitleScene::Update(void)
{


	// ポーズ画面を積む
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
	}

	// シーン遷移
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TutorialScene>());
	}
}

void TitleScene::Draw(void)
{
	int screenX = Application::SCREEN_SIZE_X;
	int screenY = Application::SCREEN_SIZE_Y;

	DrawGraph(screenX, screenY, imgHondana_, true);

	DrawExtendGraph(0, 0, screenX, screenY, movTitle_, FALSE);

	// 2D描画（ムービーの上にUIを重ねる）
	DrawRotaGraph(IMG_TITLE_POS_X, IMG_TITLE_POS_Y, 1.0f, 0.0f, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_SPACE_POS_X, IMG_PUSH_SPACE_POS_Y, 1.0f, 0.0f, imgPushSpace_, true);

	// ウエイトをかけます、あまり速く描画すると画面がちらつくからです
	WaitTimer(17);
}

void TitleScene::Release(void)
{
}