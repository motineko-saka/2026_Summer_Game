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

	// エフェクトの種類
	enum class EFFECT
	{
		OPEN_CHEST,
		MAX
	};

	// 初期化(エフェクトの読み込み)
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放(エフェクトの削除)
	void Release(void);

	// 指定エフェクトを再生
	void Play(EFFECT type, const VECTOR& pos, float rotY);

	// 指定エフェクトを停止
	void Stop(EFFECT type);

private:
	EffekseerEffect(void);
	~EffekseerEffect(void) = default;

	// コピー・ムーブ禁止
	EffekseerEffect(const EffekseerEffect&) = delete;
	EffekseerEffect& operator=(const EffekseerEffect&) = delete;
	EffekseerEffect(EffekseerEffect&&) = delete;
	EffekseerEffect& operator=(EffekseerEffect&&) = delete;

	static constexpr int EFFECT_NUM = static_cast<int>(EFFECT::MAX);

	// 静的インスタンス
	static EffekseerEffect* instance_;

	// 読み込んだエフェクトのリソースID
	std::array<int, EFFECT_NUM> resourceIds_;

	// 再生中のハンドル（最後に再生したもの）
	std::array<int, EFFECT_NUM> playHandles_;
};