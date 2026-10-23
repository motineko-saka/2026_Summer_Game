#pragma once

#include <chrono>
#include <list>
#include <memory>

class SceneBase;
class SceneTransition;

class SceneManager
{
public:
	static constexpr float DEFAULT_FPS = 60.0f;

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) instance_ = new SceneManager(); }
	static SceneManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { delete instance_; instance_ = nullptr; }

	enum SCENE
	{
		TUTORIAL = 0,
		MAIN,
	};

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// シーンを切り替える（積まれているシーンは全て破棄して読み込み直す）
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	// シーンを新しく積む（ポーズ画面など）
	void PushScene(std::shared_ptr<SceneBase> scene);

	// 最後に積んだシーンを取り除く
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ（読み込みは行わない）
	void JumpScene(std::shared_ptr<SceneBase> scene);

	// トランジション演出付きでシーンを切り替える
	void ChangeSceneTransition(std::shared_ptr<SceneBase> scene);

	// ゲーム終了
	void GameEnd(void) { isGameEnd_ = true; }
	bool GetGameEnd(void) const { return isGameEnd_; }

	// デルタタイムの取得
	float GetDeltaTime(void) const { return deltaTime_; }

	// メインスクリーンの取得
	int GetMainScreen(void) const { return mainScreen_; }

private:
	using Clock = std::chrono::steady_clock;

	SceneManager(void);
	~SceneManager(void);

	// コピー・ムーブ禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	// 静的インスタンス
	static SceneManager* instance_;

	// シーン切替演出
	std::unique_ptr<SceneTransition> sceneTransition_;

	// 演出後に切り替えるシーン
	std::shared_ptr<SceneBase> nextScene_;

	// 演出中か
	bool isTransition_ = false;

	// back が最新のシーン（基本は1つ、ポーズなどが積み重なる）
	std::list<std::shared_ptr<SceneBase>> scenes_;

	// ゲーム終了
	bool isGameEnd_ = false;

	// デルタタイム計測用
	Clock::time_point preTime_;
	float deltaTime_ = 1.0f / DEFAULT_FPS;

	// メインスクリーン
	int mainScreen_ = -1;

	// デルタタイムを更新する
	void UpdateDeltaTime(void);

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// 切替演出の更新
	void UpdateTransition(void);

	// 積まれている全シーンをReleaseしてから取り除く
	void ReleaseAllScenes(void);
};