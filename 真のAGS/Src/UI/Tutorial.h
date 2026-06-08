#pragma once
#include <string>

class Tutorial
{
public:
	Tutorial();
	~Tutorial();

	void Init();
	void Start();            // チュートリアル開始（最初のステップへ）
	void Update();           // 毎フレーム呼ぶ
	void Draw() const;       // 描画
	bool IsActive() const { return active_; }

private:
	enum class Step
	{
		NONE = -1,
		MOVE,           // WASD
		CAMERA_ANGLE,   // 矢印
		CAMERA_STATE,   // CTRL
		SWITCH_CHAR,    // TAB
		FINISHED
	};

	Step step_;
	bool active_;
	int inputDelay_;                 // 入力デバウンス（フレーム）
	static constexpr int DELAY_MAX = 8;

	// 各ステップに表示する文字列を返す
	const char* GetTextForStep(Step s) const;
};