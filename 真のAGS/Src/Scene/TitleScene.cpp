#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/SkyDome.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	imgTitle_(-1),
	imgPushSpace_(-1),
	animController_(nullptr),
	skyDome_(nullptr),
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
	Release();
}

void TitleScene::Init(void)
{
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;

	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::TITLE_PUSH_SPACE).handleId_;

	// メイン惑星
	bigPlanet_.SetModel(resMng_.Load(ResourceManager::SRC::PIT_FALL_PLANET).handleId_);
	bigPlanet_.scl = AsoUtility::VECTOR_ONE;
	bigPlanet_.quaRot = Quaternion::Identity();
	bigPlanet_.quaRotLocal = Quaternion::Identity();
	bigPlanet_.pos = AsoUtility::VECTOR_ZERO;
	bigPlanet_.Update();

	// 小惑星
	SpherePlanet_.SetModel(resMng_.Load(ResourceManager::SRC::SPHERE_PLANET).handleId_);
	SpherePlanet_.scl = SPHERE_PLANET_DEFAULT_SCALE;
	SpherePlanet_.quaRot = Quaternion::Identity();

	SpherePlanet_.quaRot = Quaternion::Euler(SPHERE_PLANET_ROT);

	SpherePlanet_.quaRotLocal = Quaternion::Identity();
	/*SpherePlanet_.quaRotLocal = Quaternion::Mult(
		SpherePlanet_.quaRotLocal, Quaternion::AngleAxis(SpherePlanet_.localPos.y, AsoUtility::AXIS_Y));
	SpherePlanet_.quaRotLocal = Quaternion::Mult(
		SpherePlanet_.quaRotLocal, Quaternion::AngleAxis(SpherePlanet_.localPos.x, AsoUtility::AXIS_X));
	SpherePlanet_.quaRotLocal = Quaternion::Mult(
		SpherePlanet_.quaRotLocal, Quaternion::AngleAxis(SpherePlanet_.localPos.z, AsoUtility::AXIS_Z));*/

	SpherePlanet_.pos = SPHERE_PLANET_DEFAULT_POS;
	SpherePlanet_.Update();

	// プレイヤー
	player_.SetModel(resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
	player_.scl = PLAYER_DEFAULT_SCALE;

	player_.quaRot = Quaternion::Identity();
	player_.quaRot = Quaternion::Mult(player_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-90.0f), AsoUtility::AXIS_Y));

	player_.quaRotLocal = Quaternion::Identity();
	player_.quaRotLocal = Quaternion::Mult(player_.quaRotLocal,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));

	player_.pos = PLAYER_DEFAULT_POS;
	player_.Update();

	animController_ = new AnimationController(player_.modelId);
	animController_->Add(static_cast<int>(ANIM_TYPE::RUN), 30.0f, Application::PATH_MODEL + "Player/Run.mv1");

	animController_->Play(static_cast<int>(ANIM_TYPE::RUN));

	skyDome_ = new SkyDome(empty_);
	skyDome_->Init();

	// 定点カメラ
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::Update(void)
{
	SpherePlanet_.quaRot = Quaternion::Mult(SpherePlanet_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-1.0f), AsoUtility::AXIS_Y));
		//;Euler(0.0f, 0.0f, AsoUtility::Deg2RadF(-1.0f)));

	SpherePlanet_.Update();

	animController_->Play(static_cast<int>(ANIM_TYPE::RUN));

	animController_->Update();

	skyDome_->Update();

	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}

}

void TitleScene::Draw(void)
{
	// モデル描画
	skyDome_->Draw();

	MV1DrawModel(bigPlanet_.modelId);

	MV1DrawModel(SpherePlanet_.modelId);

	MV1DrawModel(player_.modelId);

	DrawRotaGraph(IMG_TITLE_POS_X, IMG_TITLE_POS_Y,1.0f, 0.0f, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_SPACE_POS_X, IMG_PUSH_SPACE_POS_Y,1.0f, 0.0f, imgPushSpace_, true);

	
}

void TitleScene::Release(void)
{
	DeleteGraph(imgTitle_);
	DeleteGraph(imgPushSpace_);
	skyDome_->Release();

	delete animController_;
	delete skyDome_;

	bigPlanet_.Release();
	SpherePlanet_.Release();
	player_.Release();
}
