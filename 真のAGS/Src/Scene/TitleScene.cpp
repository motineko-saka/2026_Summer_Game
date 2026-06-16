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

TitleScene::TitleScene(void)
{
}

TitleScene::~TitleScene(void)
{
	Release();
}

void TitleScene::Init(void)
{
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;

	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::TITLE_PUSH_SPACE).handleId_;
}

void TitleScene::Load(void)
{
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
	// 2D描画
	DrawRotaGraph(IMG_TITLE_POS_X, IMG_TITLE_POS_Y, 1.0f, 0.0f, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_SPACE_POS_X, IMG_PUSH_SPACE_POS_Y, 1.0f, 0.0f, imgPushSpace_, true);
}

void TitleScene::Release(void)
{

}