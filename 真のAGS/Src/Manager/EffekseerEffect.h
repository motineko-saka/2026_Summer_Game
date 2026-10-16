#pragma once
#include <array>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

class EffekseerEffect
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) instance_ = new EffekseerEffect(); }
	static EffekseerEffect* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { delete instance_; instance_ = nullptr; }

	enum class ComboStep
	{
		First,		// 1段目
		Second,		// 2段目
		Third,		// 3段目
		Finish		// フィニッシュ
	};

	// エフェクトの種類
	enum class EFFECT
	{
		SLASH,		// 斬撃（読み込みのみ）
		TUTORIAL,	// チュートリアル
		FIRE,		// コンボ1
		WATER,		// コンボ2
		ICE,		// コンボ3
		EXPLOSION,	// コンボ4
		COMBO_LAST,	// コンボ5
		METEOR,		// コンボ6
		MAX
	};

	// 初期化（Effekseerの初期化とエフェクトの読み込み）
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放（エフェクトを削除してEffekseerを終了）
	void Release(void);

	// チュートリアルエフェクトを止める
	void Delete(void);

	// 指定エフェクトを再生
	void Play(EFFECT type, const VECTOR& pos, float rotY);

	// 指定エフェクトを停止
	void Stop(EFFECT type);

	// 以下は互換用（中身は Play を呼ぶだけ）
	void PlayTutorialEffect(const VECTOR& pos, float rotY) { Play(EFFECT::TUTORIAL, pos, rotY); }
	void PlayComboEffect(const VECTOR& pos, float rotY) { Play(EFFECT::FIRE, pos, rotY); }
	void PlayComboEffect2(const VECTOR& pos, float rotY) { Play(EFFECT::WATER, pos, rotY); }
	void PlayComboEffect3(const VECTOR& pos, float rotY) { Play(EFFECT::ICE, pos, rotY); }
	void PlayComboEffect4(const VECTOR& pos, float rotY) { Play(EFFECT::EXPLOSION, pos, rotY); }
	void PlayComboEffect5(const VECTOR& pos, float rotY) { Play(EFFECT::COMBO_LAST, pos, rotY); }
	void PlayComboEffect6(const VECTOR& pos, float rotY) { Play(EFFECT::METEOR, pos, rotY); }

private:
	EffekseerEffect(void);
	~EffekseerEffect(void) = default;

	// コピー・ムーブ禁止
	EffekseerEffect(const EffekseerEffect&) = delete;
	EffekseerEffect& operator=(const EffekseerEffect&) = delete;
	EffekseerEffect(EffekseerEffect&&) = delete;
	EffekseerEffect& operator=(EffekseerEffect&&) = delete;

	static constexpr int EFFECT_NUM = static_cast<int>(EFFECT::MAX);

	// Effekseerで同時に表示できる最大パーティクル数
	static constexpr int MAX_PARTICLE = 8000;

	// 静的インスタンス
	static EffekseerEffect* instance_;

	// 読み込んだエフェクトのリソースID
	std::array<int, EFFECT_NUM> resourceIds_;

	// 再生中のハンドル（最後に再生したもの）
	std::array<int, EFFECT_NUM> playHandles_;
};