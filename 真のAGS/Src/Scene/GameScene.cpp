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
	isPlayer1HitObject_(false),
	isPlayer2HitObject_(false),
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

	// オブジェクトのモデルコライダーをプレイヤーに登録
	const ColliderBase* objectCollider =
		object_->GetOwnCollider(static_cast<int>(Object::COLLIDER_TYPE::MODEL));
	player1_->AddHitCollider(objectCollider);
	player2_->AddHitCollider(objectCollider);

	// プレイヤー1のコライダーをエネミーに登録
	enemyManager_->AddHitCollider(player1_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE)));

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

	for (const auto& stage : stageManager_->GetStage())
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

	// 衝突フラグの初期化
	isPlayer1HitObject_ = false;
	isPlayer2HitObject_ = false;
}

void GameScene::CheckCollisions(void)
{
	// プレイヤー1とオブジェクトの衝突判定
	auto player1Capsule = player1_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE));
	auto objectCollider = object_->GetOwnCollider(static_cast<int>(Object::COLLIDER_TYPE::CAPSULE));

	// 簡易的な距離判定でチェック
	VECTOR player1Pos = player1_->GetTransform().pos;
	VECTOR objectPos = object_->GetTransform().pos;

	float distance1 = VSize(VSub(player1Pos, objectPos));
	isPlayer1HitObject_ = (distance1 < 180.0f);

	// プレイヤー1がオブジェクトに衝突している場合、押す
	if (isPlayer1HitObject_)
	{
		// プレイヤーからオブジェクトへの方向ベクトル
		VECTOR pushDir = VSub(objectPos, player1Pos);
		pushDir.y = 0.0f; // Y軸(垂直方向)は無視
		pushDir = VNorm(pushDir); // 正規化

		// オブジェクトを押す(速度は適度に調整)
		object_->Push(pushDir, 0.3f);
	}

	// プレイヤー2とオブジェクトの衝突判定
	VECTOR player2Pos = player2_->GetTransform().pos;
	float distance2 = VSize(VSub(player2Pos, objectPos));
	isPlayer2HitObject_ = (distance2 < 180.0f);

	// プレイヤー2がオブジェクトに衝突している場合、押す
	if (isPlayer2HitObject_)
	{
		// プレイヤーからオブジェクトへの方向ベクトル
		VECTOR pushDir = VSub(objectPos, player2Pos);
		pushDir.y = 0.0f; // Y軸(垂直方向)は無視
		pushDir = VNorm(pushDir); // 正規化

		// オブジェクトを押す(速度は適度に調整)
		object_->Push(pushDir, 0.3f);
	}
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

	// 衝突判定チェック(Objectの更新前に実行)
	CheckCollisions();

	// オブジェクトの更新(押す処理を反映)
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

	// 衝突判定結果の表示(プレイヤー1側)
	if (isPlayer1HitObject_)
	{
		DrawFormatString(0, 40, GetColor(255, 0, 0), "P1: オブジェクトと衝突中!");
	}
	else
	{
		DrawFormatString(0, 40, GetColor(0, 255, 0), "P1: 衝突なし");
	}

	// 衝突判定結果の表示(プレイヤー2側)
	if (isPlayer2HitObject_)
	{
		DrawFormatString(halfWidth, 40, GetColor(255, 0, 0), "P2: オブジェクトと衝突中!");
	}
	else
	{
		DrawFormatString(halfWidth, 40, GetColor(0, 255, 0), "P2: 衝突なし");
	}
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