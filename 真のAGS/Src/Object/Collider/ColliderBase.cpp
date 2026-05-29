#include "../Common/Transform.h"
#include "ColliderBase.h"

ColliderBase::ColliderBase(SHAPE shape, TAG tag, const Transform* follow)
	:
	shape_(shape),
	tag_(tag),
	follow_(follow),
	originalFollow_(follow),
	isValid_(true)
{
}

ColliderBase::~ColliderBase(void)
{
}

void ColliderBase::Draw(void)
{
	int color = COLOR_INVALID;
	if (isValid_)
	{
		color = COLOR_VALID;
	}
	DrawDebug(color);
}

void ColliderBase::SetFollow(Transform* follow)
{
	follow_ = follow;
}

VECTOR ColliderBase::GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, int maxTryCnt, float pushDistance) const
{
	return VECTOR();
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// ローカル座標を回転・平行移動してワールド座標にする
	VECTOR localRotPos = follow_->quaRot.PosAxis(localPos);
	return VAdd(follow_->pos, localRotPos);
}