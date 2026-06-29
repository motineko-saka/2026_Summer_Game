#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../Collider/ColliderModel.h"
#include "Stage.h"

Stage::Stage(int modelID)
	:
	stageID_(-1),
	StageBase()
{
	transform_.SetModel(modelID);
}

Stage::~Stage(void)
{
}

void Stage::InitLoad(void)
{
	//transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::TUTORIAL_STAGE));
}

void Stage::InitTransform(void)
{
	transform_.scl = STAGE_DEFAULT_SCALE;
	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	transform_.pos = STAGE_DEFAULT_POS;
	transform_.Update();

	// ç≈è¨ç≈ëÂç¿ïWéÊìæ
	Vector3 minPos = { FLT_MAX, FLT_MAX, FLT_MAX };
	Vector3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	auto offset = transform_.pos;
	auto scale = transform_.scl;

	int handle = transform_.modelId;
	int meshNum = MV1GetMeshNum(handle);

	for (int i = 0; i < meshNum; ++i)
	{
		// ç≈è¨ç¿ïW
		auto mPos = MV1GetMeshMinPosition(handle, i);
		Vector3 pos =
		{
			mPos.x * scale.x + offset.x,
			mPos.y * scale.y + offset.y,
			mPos.z * scale.z + offset.z
		};

		minPos.x = (std::min)(minPos.x, pos.x);
		minPos.y = (std::min)(minPos.y, pos.y);
		minPos.z = (std::min)(minPos.z, pos.z);

		// ç≈ëÂç¿ïW
		mPos = MV1GetMeshMaxPosition(handle, i);
		pos =
		{
			mPos.x * scale.x + offset.x,
			mPos.y * scale.y + offset.y,
			mPos.z * scale.z + offset.z
		};

		maxPos.x = (std::max)(maxPos.x, pos.x);
		maxPos.y = (std::max)(maxPos.y, pos.y);
		maxPos.z = (std::max)(maxPos.z, pos.z);
	}

	bb_.minPos = minPos;
	bb_.maxPos = maxPos;
}

void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
	stageType_ = STAGE_TYPE::MAIN_STAGE;
}
