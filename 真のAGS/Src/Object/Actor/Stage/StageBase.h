#pragma once

#include <string>
#include <array>
#include "../ActorBase.h"

class StageBase : public ActorBase
{
public:
	struct Vector3 {
		float x, y, z;
		Vector3() :x(0.0f), y(0.0f), z(0.0f) {}
		Vector3(float inx, float iny, float inz) : x(inx), y(iny), z(inz) {};
		VECTOR GetVECTOR()const { return VGet(x, y, z); }
	};

	struct BoundingBox {
		Vector3 minPos;//12
		Vector3 maxPos;//12
	};

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		MODEL,
		CAPSULE,
		MAX,
		VIEW_RANGE,
	};

	enum class STAGE_TYPE
	{
		MAIN_STAGE = 0,
		ANSWER_STAGE,
	};

	static constexpr int STAGE_CUT_NUM = 2;

	StageBase();
	virtual ~StageBase(void);

	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void DrawAtOffset(const VECTOR& offset);

	void DrawDebug(void);

	const BoundingBox GetBoundingBox(void) const { return bb_; }

	std::array<VECTOR, STAGE_CUT_NUM> GetStageCutPos()const { return stageCutPos_; }

protected:

	// リソースロード
	void InitLoad(void)override = 0;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void)override;

	STAGE_TYPE stageType_;

	BoundingBox bb_;

	std::array<VECTOR, STAGE_CUT_NUM> stageCutPos_;

	// 除外フレーム名称
	const std::vector<std::string> EXCLUDE_FRAME_NAMES =
	{
		/*"Grass",*/"Plant","Leaves"
	};

private:
	static constexpr VECTOR STAGE_DEFAULT_POS = { 0.0f, 0.0f, 0.0f };

	static constexpr VECTOR STAGE_DEFAULT_SCALE = { 1.0f,1.0f,1.0f };

	// 対象フレーム
	const std::vector<std::string> TARGET_FRAME_NAMES =
	{
		"Ground","Grass","Plant","Leaves"
	};

};

