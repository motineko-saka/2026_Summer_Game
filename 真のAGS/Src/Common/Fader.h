#pragma once

class Fader
{
public:

	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 5.0f;

	// 不透明度の最大値
	static constexpr float MAX_ALPHA = 255.0f;

	// 状態
	enum class STATE
	{
		NONE,
		FADE_OUT,	// 徐々に暗転
		FADE_IN		// 徐々に明転
	};

	Fader(void) = default;
	~Fader(void) = default;

	// 状態の取得
	STATE GetState(void) const { return state_; }

	// フェード処理が終了しているか
	bool IsEnd(void) const { return isEnd_; }

	// 指定フェードを開始する
	void SetFade(STATE state);

	// 初期化（現状処理なし）
	void Init(void) {}

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

private:

	// 状態
	STATE state_ = STATE::NONE;

	// 不透明度
	float alpha_ = 0.0f;

	// 状態(STATE)を保ったまま終了判定を行うため、
	// Update->Draw->Update の1フレーム判定用
	bool isPreEnd_ = true;

	// フェード処理の終了判定
	bool isEnd_ = true;
};