#include "SceneManager.h"

#include <DxLib.h>

#include "../Application.h"
#include "../Loading/Loading.h"
#include "../Scene/TitleScene.h"
#include "../Scene/SceneTransition.h"

SceneManager* SceneManager::instance_ = nullptr;

// unique_ptr<SceneTransition> の破棄に完全型が必要なため cpp で定義
SceneManager::SceneManager(void) = default;
SceneManager::~SceneManager(void) = default;

// ---------------------------------------------------------------
// 初期化・解放
// ---------------------------------------------------------------
void SceneManager::Init(void)
{
	// ロード画面
	Loading::CreateInstance();
	Loading::GetInstance()->Init();
	Loading::GetInstance()->Load();

	// メインスクリーン（シーンの読み込みより先に用意する）
	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TRUE);

	// シーン切替演出
	sceneTransition_ = std::make_unique<SceneTransition>();

	// 最初はタイトル画面
	ChangeScene(std::make_shared<TitleScene>());

	ResetDeltaTime();
}

void SceneManager::Release(void)
{

	// 全てのシーンの解放
	ReleaseAllScenes();
	nextScene_.reset();

	sceneTransition_.reset();

	if (mainScreen_ != -1)
	{
		DeleteGraph(mainScreen_);
		mainScreen_ = -1;
	}

	// ロード画面の削除
	if (Loading::GetInstance() != nullptr)
	{
		Loading::GetInstance()->Release();
		Loading::DeleteInstance();
	}
}

// ---------------------------------------------------------------
// 更新・描画
// ---------------------------------------------------------------
void SceneManager::Update(void)
{
	UpdateDeltaTime();

	if (scenes_.empty()) return;

	// 切替演出中
	if (isTransition_)
	{
		UpdateTransition();
		return;
	}

	Loading* loading = Loading::GetInstance();

	// ロード中
	if (loading->IsLoading())
	{
		loading->Update();

		// ロードが終わったらシーン側の後処理
		if (!loading->IsLoading())
		{
			scenes_.back()->LoadEnd();
		}
		return;
	}

	// 通常の更新
	const std::shared_ptr<SceneBase> current = scenes_.back();
	current->Update();
}

void SceneManager::Draw(void)
{
	if (Loading::GetInstance()->IsLoading())
	{
		Loading::GetInstance()->Draw();
	}
	else
	{
		// 積まれているシーンを下から順に描画
		for (auto& scene : scenes_)
		{
			scene->Draw();
		}
	}

	if (isTransition_)
	{
		sceneTransition_->Draw();
	}
}

void SceneManager::UpdateTransition(void)
{
	sceneTransition_->Update();
	if (sceneTransition_->IsPlay()) return;

	const auto state = sceneTransition_->GetState();

	if (state == SceneTransition::STATE::CLOSE)
	{
		// 閉じ終わったらシーンを切り替えて開く
		ChangeScene(nextScene_);
		nextScene_.reset();
		sceneTransition_->Open();
	}
	else if (state == SceneTransition::STATE::OPEN)
	{
		isTransition_ = false;
	}
}

void SceneManager::ReleaseAllScenes(void)
{
	// 上に積まれているものから順に解放する
	for (auto it = scenes_.rbegin(); it != scenes_.rend(); ++it)
	{
		(*it)->Release();
	}
	scenes_.clear();
}

void SceneManager::UpdateDeltaTime(void)
{
	const auto now = Clock::now();
	deltaTime_ = std::chrono::duration<float>(now - preTime_).count();
	preTime_ = now;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 1.0f / DEFAULT_FPS;
	preTime_ = Clock::now();
}

// ---------------------------------------------------------------
// シーン操作
// ---------------------------------------------------------------
void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	// ポーズ画面などのオーバーレイも含めて差し替える
	ReleaseAllScenes();
	scenes_.push_back(scene);

	SetMouseDispFlag(FALSE);

	// 非同期で読み込む
	Loading* loading = Loading::GetInstance();
	loading->StartAsyncLoad();
	scenes_.back()->Load();
	loading->EndAsyncLoad();
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	// 元のシーンは残したまま上に積む
	scene->Init();
	scenes_.push_back(scene);
}

void SceneManager::PopScene(void)
{
	if (scenes_.size() <= 1) return;

	SetMouseDispFlag(FALSE);
	scenes_.back()->Release();
	scenes_.pop_back();
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	ReleaseAllScenes();
	scenes_.push_back(scene);
}

void SceneManager::ChangeSceneTransition(std::shared_ptr<SceneBase> scene)
{
	if (isTransition_) return;

	nextScene_ = scene;
	isTransition_ = true;
	sceneTransition_->Start();
}