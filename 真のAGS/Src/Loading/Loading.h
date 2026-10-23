#pragma once

#include "../Common/Vector2.h"

class Loading
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) instance_ = new Loading(); }
	static Loading* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { delete instance_; instance_ = nullptr; }

	void Init(void);			// 初期化
	void Load(void);			// 読み込み
	void Update(void);			// 更新
	void Draw(void) const;		// 描画
	void Release(void);			// 解放

	void StartAsyncLoad(void);	// 非同期ロードの開始
	void EndAsyncLoad(void);	// 非同期ロードの終了

	// ロード中かどうか
	bool IsLoading(void) const { return isLoading_; }

private:
	// 外部から生成できないようにする
	Loading(void) = default;
	~Loading(void) = default;

	// コピー禁止
	Loading(const Loading&) = delete;
	Loading& operator=(const Loading&) = delete;
	Loading(Loading&&) = delete;
	Loading& operator=(Loading&&) = delete;

	// ロード終了処理
	void FinishLoad(void);

	// 静的インスタンス
	static Loading* instance_;

	// 最低でもロード画面を表示する時間の計測用(フレーム)
	int loadTimer_ = 0;

	// 画像ハンドル
	int handle_ = -1;

	// 描画座標
	Vector2 pos_;

	// ロード中の判定用
	bool isLoading_ = false;
};