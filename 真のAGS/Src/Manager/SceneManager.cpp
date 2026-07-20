#include "SceneManager.h"

#include <DxLib.h>
#include <chrono>

#include "../Loading/Loading.h"
#include "../Scene/TitleScene.h"
#include "../Scene/TutorialScene.h"
#include "../Scene/GameScene.h"
#include "../Manager/Camera.h"
#include "../Scene/SceneTransition.h"

SceneManager* SceneManager::instance_ = nullptr;

// コンストラクタ
SceneManager::SceneManager(void)
{
	isGameEnd_ = false;

	deltaTime_ = 1.0f / 60.0f;

}

// デストラクタ
SceneManager::~SceneManager(void)
{
}

// 初期化
void SceneManager::Init(void)
{
	// ロード画面生成
	Loading::CreateInstance();
	Loading::GetInstance()->Init();
	Loading::GetInstance()->Load();

	// 最初はタイトル画面から
	ChangeScene(std::make_shared<TitleScene>());
	//ChangeScene(std::make_shared<TutorialScene>());
	//ChangeScene(std::make_shared<GameScene>());

		// メインスクリーン
	mainScreen_ = MakeScreen(
		Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	sceneTransition_ = std::make_unique<SceneTransition>();
}


// 更新
void SceneManager::Update(void)
{
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// シーンがなければ終了
	if (scenes_.empty())
		return;

	if (isTransition_)
	{
		sceneTransition_->Update();

		if (!sceneTransition_->IsPlay())
		{
			if (sceneTransition_->GetState() == SceneTransition::STATE::CLOSE)
			{
				// シーン変更
				ChangeScene(nextScene_);

				// 開く演出
				sceneTransition_->Open();
			}
			else if (sceneTransition_->GetState() == SceneTransition::STATE::OPEN)
			{
				isTransition_ = false;
			}
		}
		return;
	}

	// ロード中
	if (Loading::GetInstance()->IsLoading())
	{
		// ロード更新
		Loading::GetInstance()->Update();

		// ロードの更新が終了していたら
		if (Loading::GetInstance()->IsLoading() == false)
		{
			// ロード後の初期化
			scenes_.back()->LoadEnd();
		}
	}
	// 通常の更新処理
	else
	{
		// 現在のシーンの更新
		scenes_.back()->Update();
	}
}

// 描画
void SceneManager::Draw(void)
{
	//SetDrawScreen(mainScreen_);
	//// 画面を初期化
	//ClearDrawScreen();

	// ロード中ならロード画面を描画
	if (Loading::GetInstance()->IsLoading())
	{
		// ロードの描画
		Loading::GetInstance()->Draw();
	}
	// 通常の更新
	else
	{
		// 積まれているもの全てを描画する
		for (auto& scene : scenes_)
		{
			// シーンの描画
			scene->Draw();
		}
	}
	
	if (isTransition_)
	{
		sceneTransition_->Draw();
	}

	//// 背面スクリーンにメインスクリーンを描画
	//SetDrawScreen(DX_SCREEN_BACK);
	//DrawGraph(0, 0, mainScreen_, true);
}

// 解放
void SceneManager::Release(void)
{
	//全てのシーンの解放・削除
	for (auto& scene : scenes_)
	{
		scene->Release();
	}
	scenes_.clear();

	// ロード画面の削除
	Loading::GetInstance()->Release();
	Loading::GetInstance()->DeleteInstance();
}

// 状態遷移関数
void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	// シーンが空か？
	if (scenes_.empty())
	{
		//空なので新しく入れる
		scenes_.push_back(scene);
	}
	else
	{
		//末尾のものを新しい物に入れ替える
		scenes_.back() = scene;
	}

	SetMouseDispFlag(false);

	// 読み込み(非同期)
	Loading::GetInstance()->StartAsyncLoad();
	scenes_.back()->Load();
	Loading::GetInstance()->EndAsyncLoad();
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	//新しく積むのでもともと入っている奴はまだ削除されない
	scene->Init();
	scenes_.push_back(scene);
}

void SceneManager::PopScene(void)
{
	//積んであるものを消して、もともとあったものを末尾にする
	if (scenes_.size() > 1)
	{
		SetMouseDispFlag(false);
		scenes_.pop_back();
	}
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	// 全て解放
	scenes_.clear();

	// 新しく積む
	scenes_.push_back(scene);
}

float SceneManager::GetDeltaTime(void) const
{
	return deltaTime_;
	//return 1 / 60.0f;
}

int SceneManager::GetMainScreen(void) const
{
	return mainScreen_;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 0.016f;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::ChangeSceneTransition(std::shared_ptr<SceneBase> scene)
{
	if (isTransition_)
		return;

	nextScene_ = scene;

	isTransition_ = true;

	sceneTransition_->Start();
}