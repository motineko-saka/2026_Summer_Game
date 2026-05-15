#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/EnemyManager.h"
#include "../Manager/StageManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/SkyDome.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/EnemyRat.h"
#include "../Object/Actor/Object.h"
#include "../Object/Collider/ColliderBase.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	stageManager_(nullptr),
	skyDome_(nullptr),
	player1_(nullptr),
	player2_(nullptr),
	camera1_(nullptr),
	camera2_(nullptr),
	enemyManager_(nullptr),
	screenHandle1_(-1),
	screenHandle2_(-1),
	screenWidth_(0),
	screenHeight_(0),
	SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// 画面サイズの取得
	GetScreenState(&screenWidth_, &screenHeight_, nullptr);

	// 分割画面用のスクリーン作成(左右画面)
	int halfWidth = screenWidth_ / 2;
	screenHandle1_ = MakeScreen(halfWidth, screenHeight_, TRUE);
	screenHandle2_ = MakeScreen(halfWidth, screenHeight_, TRUE);

	// プレイヤー1
	player1_ = new Player(Player::PLAYER_NO::PLAYER1);
	player1_->Init();

	// プレイヤー2(プレイヤー1を複製)
	player2_ = new Player(Player::PLAYER_NO::PLAYER2);
	player2_->Init();

	// ステージ
	stageManager_ = new StageManager();
	stageManager_->InitStage();

	// スカイドーム(プレイヤー1用)
	skyDome_ = new SkyDome(player1_->GetTransform());
	skyDome_->Init();

	// エネミー管理
	enemyManager_ = new EnemyManager(player1_);
	enemyManager_->Init();

	// オブジェクト作成
	object_ = new Object(GameScene::WORLD::LEFT);
	object_->Init();

<<<<<<< HEAD
	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	// ステージモデルのコライダーをプレイヤー1に登録
	player1_->AddHitCollider(stageCollider);

	// ステージモデルのコライダーをプレイヤー2に登録
	player2_->AddHitCollider(stageCollider);

	// ステージモデルのコライダーをエネミーに登録
	enemyManager_->AddHitCollider(stageCollider);

	// オブジェクトのモデルコライダーをプレイヤーに登録
	const ColliderBase* objectCollider =
		object_->GetOwnCollider(static_cast<int>(Object::COLLIDER_TYPE::MODEL));
	player1_->AddHitCollider(objectCollider);
	player2_->AddHitCollider(objectCollider);

	// プレイヤー1のコライダーをエネミーに登録
	enemyManager_->AddHitCollider(player1_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE)));

=======
>>>>>>> 299a2a4da06e82a39494e4e37574630b5067dc01
	// カメラ1の作成(プレイヤー1用)
	camera1_ = new Camera();
	camera1_->Init();
	camera1_->SetFollow(&player1_->GetTransform());
	camera1_->ChangeMode(Camera::MODE::FOLLOW);

	// カメラ2の作成(プレイヤー2用)
	camera2_ = new Camera();
	camera2_->Init();
	camera2_->SetFollow(&player2_->GetTransform());
	camera2_->ChangeMode(Camera::MODE::FOLLOW);

	for(const auto& stage : stageManager_->GetStage())
	{
		const ColliderBase* stageCollider =
			stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		// ステージモデルのコライダーをプレイヤー1に登録
		player1_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーをプレイヤー2に登録
		player2_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーをエネミーに登録
		enemyManager_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーをカメラに登録
		camera1_->AddHitCollider(stageCollider);
		camera2_->AddHitCollider(stageCollider);
	}

	// プレイヤー1のコライダーをエネミーに登録
	enemyManager_->AddHitCollider(player1_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE)));
}

void GameScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	stageManager_->Update();
	skyDome_->Update();
	player1_->Update();
	player2_->Update();
	enemyManager_->Update();
	camera1_->Update();
	camera2_->Update();
	object_->Update();
}

void GameScene::DrawPlayer1Screen(void)
{
	// プレイヤー1用のカメラ設定
	camera1_->SetBeforeDraw();

	// 3D描画
	stageManager_->Draw();
	skyDome_->Draw();
	player1_->Draw();
	player2_->Draw(); // プレイヤー2も描画(同じ世界にいる場合)
	object_->SetWorld(WORLD::LEFT);
	object_->Draw();
	enemyManager_->Draw();
}

void GameScene::DrawPlayer2Screen(void)
{
	// プレイヤー2用のカメラ設定
	camera2_->SetBeforeDraw();

	// 3D描画
	stageManager_->Draw();
	skyDome_->Draw();
	player1_->Draw(); // プレイヤー1も描画(同じ世界にいる場合)
	player2_->Draw();
	object_->SetWorld(WORLD::RIGHT);
	object_->Draw();
	enemyManager_->Draw();
}

void GameScene::Draw(void)
{
	int halfWidth = screenWidth_ / 2;

	// プレイヤー1の画面を描画(左側)
	SetDrawScreen(screenHandle1_);
	ClearDrawScreen();
	DrawPlayer1Screen();

	// プレイヤー2の画面を描画(右側)
	SetDrawScreen(screenHandle2_);
	ClearDrawScreen();
	DrawPlayer2Screen();

	// メイン画面に転送
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	// 左半分にプレイヤー1の画面
	DrawExtendGraph(0, 0, halfWidth, screenHeight_, screenHandle1_, true);

	// 右半分にプレイヤー2の画面
	DrawExtendGraph(halfWidth, 0, screenWidth_, screenHeight_, screenHandle2_, true);

	// デバッグ表示
	DrawFormatString(0, 0, GetColor(255, 255, 255), "P1角度:(%.1f, %.1f, %.1f)",
		player1_->GetTransform().quaRot.ToEuler().x,
		player1_->GetTransform().quaRot.ToEuler().y,
		player1_->GetTransform().quaRot.ToEuler().z);

	DrawFormatString(halfWidth, 0, GetColor(255, 255, 255), "P2角度:(%.1f, %.1f, %.1f)",
		player2_->GetTransform().quaRot.ToEuler().x,
		player2_->GetTransform().quaRot.ToEuler().y,
		player2_->GetTransform().quaRot.ToEuler().z);
}

void GameScene::Release(void)
{
	stageManager_->Release();
	delete stageManager_;

	skyDome_->Release();
	delete skyDome_;

	player1_->Release();
	delete player1_;

	player2_->Release();
	delete player2_;

	object_->Release();
	delete object_;

	enemyManager_->Release();
	delete enemyManager_;

	camera1_->Release();
	delete camera1_;

	camera2_->Release();
	delete camera2_;

	// スクリーンハンドルの削除
	if (screenHandle1_ != -1)
	{
		DeleteGraph(screenHandle1_);
	}
	if (screenHandle2_ != -1)
	{
		DeleteGraph(screenHandle2_);
	}
}