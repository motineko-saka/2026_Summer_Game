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
#include "../Object/Actor/Charactor/GameObject/ObjectBase.h"
#include "../Object/Actor/Wall.h"
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
	Release();
}

void GameScene::Init(void)
{
	// 画面サイズの取得
	GetScreenState(&screenWidth_, &screenHeight_, nullptr);

	ansVec_ = ANSWER_VECTOR;

	// 分割画面用のスクリーン作成(左右画面)
	int halfWidth = screenWidth_ / 2;
	screenHandle1_ = MakeScreen(halfWidth, screenHeight_, true);
	screenHandle2_ = MakeScreen(halfWidth, screenHeight_, true);

	players_.resize(2);

	for (int i = 0; i < 2; i++)
	{
		players_[i].camera_ = new Camera();
		players_[i].camera_->Init();

		// プレイヤー番号を設定
		Player::PLAYER_NO pno = (i == 0) ? Player::PLAYER_NO::PLAYER1 : Player::PLAYER_NO::PLAYER2;
		players_[i].player_ = new Player(pno, *players_[i].camera_);
		players_[i].player_->Init();

		players_[i].camera_->SetFollow(&players_[i].player_->GetTransform());
		players_[i].camera_->ChangeMode(Camera::MODE::FOLLOW);
	}

	// メンバ変数に紐付け
	player1_ = players_[0].player_;
	player2_ = players_[1].player_;
	camera1_ = players_[0].camera_;
	camera2_ = players_[1].camera_;

	// ステージ
	stageManager_ = new StageManager();
	stageManager_->InitStage();

	// スカイドーム(プレイヤー1用)
	skyDome_ = new SkyDome(player1_->GetTransform());
	skyDome_->Init();

	wall_ = std::make_unique<Wall>();
	wall_->Init();

	// エネミー管理
	//enemyManager_ = new EnemyManager(player1_);
	//enemyManager_->Init();

	// オブジェクト作成（複数）
	objects_.reserve(4);

	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::DEFAULT));
	objects_.back()->Init();

	objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });

	objects_.back()->SetPosition({ 1260.0f, -500.0f, -50.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });


	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::WBOX));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });

	objects_.push_back(new ObjectBase(GameScene::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[2], ObjectBase::OBJECT_TYPE::AKEG));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -1260.0f, -720.0f, -50.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });

	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[3], ObjectBase::OBJECT_TYPE::BUTTOM));
	objects_.back()->Init();

	objects_.back()->SetPosition({ 1000.0f, -720.0f, -50.5f });
	//objects_.back()->SetPosition({ 0.0f, 80.0f, -50.0f });

	objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });
	objects_.back()->SetPosition({ 0.0f, 80.0f, -50.0f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });


	// ステージの各コライダをプレイヤー／カメラ／オブジェクトに登録
	for (const auto& stage : stageManager_->GetStage())
	{
		const ColliderBase* stageCollider =
			stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		// ステージモデルのコライダーをプレイヤー1に登録
		player1_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーをプレイヤー2に登録
		player2_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーをエネミーに登録
		//enemyManager_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーをカメラに登録
		camera1_->AddHitCollider(stageCollider);
		camera2_->AddHitCollider(stageCollider);

		// ステージモデルのコライダーを全オブジェクトに登録
		for (auto* obj : objects_)
		{
			obj->AddHitCollider(stageCollider);
		}

		if (stageCollider == nullptr) DrawFormatString(100, 100, 0xffffff, "stageCollider is null\n");
	}

	// 各オブジェクトの衝突コライダをプレイヤーに登録
	for (auto* obj : objects_)
	{
		const ColliderBase* objCaps = obj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
		if (objCaps) player1_->AddHitCollider(objCaps);
		if (objCaps) player2_->AddHitCollider(objCaps);
	}

	const ColliderBase* wallCollider =
		wall_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	player1_->AddHitCollider(wallCollider);
	player2_->AddHitCollider(wallCollider);

	// 衝突フラグの初期化
	isPlayer1HitObject_ = false;
	isPlayer2HitObject_ = false;
	ansVec_ = ANSWER_VECTOR;

	// 初期アクティブ状態（プレイヤー1 を操作）
	activePlayer_ = Player::PLAYER_NO::PLAYER1;
	player1_->SetActive(true);
	player2_->SetActive(false);
	camera1_->SetControlEnabled(true);
	camera2_->SetControlEnabled(false);
}

void GameScene::CheckCollisions(void)
{
	// 各オブジェクトに対してプレイヤーとの距離判定を行う
	isPlayer1HitObject_ = false;
	isPlayer2HitObject_ = false;

	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;

		VECTOR objectPos = obj->GetTransform().pos;

		// ボタンタイプの場合は専用処理
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTOM)
		{
			bool isNearButton = false;

			// プレイヤー1との距離チェック

			VECTOR player1Pos = player1_->GetTransform().pos;
			float distance1 = VSize(VSub(player1Pos, objectPos));
			if (distance1 < 180.0f)
			{
				isNearButton = true;
				// ボタンが押されたときの処理（例：ゲームクリア、ドアが開くなど）
			}

			// プレイヤー2も同様にチェック
			VECTOR player2Pos = player2_->GetTransform().pos;
			float distance2 = VSize(VSub(player2Pos, objectPos));
			if (distance2 < 180.0f)
			{
				isNearButton = true;
			}

			// ボタンの近くにいて、スペースキーが押されたら
			if (isNearButton && InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
			{
				obj->SetButtomPushed(true);
				// ボタンが押されたときの処理（例：ゲームクリア、ドアが開くなど）
			}

			continue;

			// プレイヤー2も同様にチェック
			if (distance2 < 180.0f)
			{
				obj->SetButtomPushed(true);
			}

			continue;
		}

		// プレイヤー1との距離
		VECTOR player1Pos = player1_->GetTransform().pos;
		float distance1 = VSize(VSub(player1Pos, objectPos));
		bool hit1 = (distance1 < 180.0f);
		if (hit1)
		{
			isPlayer1HitObject_ = true;
			// 必要なら押す処理を有効化
			// VECTOR pushDir = VSub(objectPos, player1Pos); pushDir.y = 0.0f; pushDir = VNorm(pushDir); obj->Push(pushDir, 5.0f);
		}

		// プレイヤー2との距離
		VECTOR player2Pos = player2_->GetTransform().pos;
		float distance2 = VSize(VSub(player2Pos, objectPos));
		bool hit2 = (distance2 < 180.0f);
		if (hit2)
		{
			//isPlayer2HitObject_ = true;
			// プレイヤーからオブジェクトへの方向ベクトル
			//VECTOR pushDir = VSub(objectPos, player2Pos);
			//pushDir.y = 0.0f; // Y軸(垂直方向)は無視
			//pushDir = VNorm(pushDir); // 正規化

			// オブジェクトを押す(速度は適度に調整)
			//obj->Push(pushDir, 5.0f);
		}
	}

	if(InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		// ボタンオブジェクトの判定
		for (auto* obj : objects_)
		{
			if (obj == nullptr) continue;

			// ボタンタイプのオブジェクトのみ処理
			if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTOM)
			{
				VECTOR objectPos = obj->GetTransform().pos;

				// プレイヤー1との距離チェック
				VECTOR player1Pos = player1_->GetTransform().pos;
				float distance1 = VSize(VSub(player1Pos, objectPos));
				if (distance1 < 180.0f)
				{
					obj->SetButtomPushed(true);
				}

				// プレイヤー2との距離チェック
				VECTOR player2Pos = player2_->GetTransform().pos;
				float distance2 = VSize(VSub(player2Pos, objectPos));
				if (distance2 < 180.0f)
				{
					obj->SetButtomPushed(true);
				}
			}
		}
	}
}

void GameScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();

	// プレイヤー選択切替
	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
		{
			activePlayer_ = Player::PLAYER_NO::PLAYER2;
			player1_->SetActive(false);
			player2_->SetActive(true);
			camera1_->SetControlEnabled(false);
			camera2_->SetControlEnabled(true);
		}
		else
		{
			activePlayer_ = Player::PLAYER_NO::PLAYER1;
			player1_->SetActive(true);
			player2_->SetActive(false);
			camera1_->SetControlEnabled(true);
			camera2_->SetControlEnabled(false);
		}
	}

	// 右クリックでもプレイヤー切替
	if (ins.IsTrgMouseRight())
	{
		if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
		{
			activePlayer_ = Player::PLAYER_NO::PLAYER2;
			player1_->SetActive(false);
			player2_->SetActive(true);
			camera1_->SetControlEnabled(false);
			camera2_->SetControlEnabled(true);
		}
		else
		{
			activePlayer_ = Player::PLAYER_NO::PLAYER1;
			player1_->SetActive(true);
			player2_->SetActive(false);
			camera1_->SetControlEnabled(true);
			camera2_->SetControlEnabled(false);
		}
	}

	stageManager_->Update();
	skyDome_->Update();
	player1_->Update();
	player2_->Update();
	//enemyManager_->Update();
	camera1_->Update();
	camera2_->Update();
	wall_->Update();


	// 衝突判定チェック(Objectの更新前に実行)
	CheckCollisions();

	// 全オブジェクトの更新
	for (auto* obj : objects_)
	{
		if (obj) obj->Update();
	}

	// 答えの場所に全てのオブジェクトがあるか判定
	bool isAnswer = true;

	for (auto* obj : objects_)
	{
		if (!obj->IsAnswerPosition())
		{
			isAnswer = false;
		}
	}

	if (isAnswer)
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);
	}
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
	//wall_->Draw();

	// 全オブジェクトを順に描画（それぞれの viewWorld を設定）
	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;
		//obj->SetViewWorld(WORLD::LEFT);
		obj->Draw();
	}
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
	//wall_->Draw();

	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;
		//obj->SetViewWorld(WORLD::RIGHT);
		obj->Draw();
	}
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

	// 非アクティブ側を薄暗くする
	int dimAlpha = 150; 
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, dimAlpha);
	if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
	{
		// 右側を暗くする
		DrawBox(halfWidth, 0, screenWidth_, screenHeight_, GetColor(0, 0, 0), TRUE);
	}
	else
	{
		// 左側を暗くする
		DrawBox(0, 0, halfWidth, screenHeight_, GetColor(0, 0, 0), TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#pragma region デバッグ表示
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

	// オブジェクト位置表示（先頭のオブジェクト）
	if (!objects_.empty() && objects_[0] != nullptr)
	{
		DrawFormatString(halfWidth, 80, GetColor(0, 0, 0), "座標:(%.1f, %.1f, %.1f)",
			objects_[0]->GetTransform().pos.x,
			objects_[0]->GetTransform().pos.y,
			objects_[0]->GetTransform().pos.z);
	}

	int y = 120;
	for (auto& object : objects_)
	{
		DrawFormatString(halfWidth, y, GetColor(255, 255, 255), "Object情報:座標(%.1f, %.1f, %.1f) 回転(%.1f, %.1f, %.1f)\nViewWorld : %d isAnswer : %d",
			object->GetTransform().pos.x,
			object->GetTransform().pos.y,
			object->GetTransform().pos.z,
			object->GetTransform().quaRot.ToEuler().x,
			object->GetTransform().quaRot.ToEuler().y,
			object->GetTransform().quaRot.ToEuler().z,
			static_cast<int>(object->GetWorld()),
			object->IsAnswerPosition());

		y += 40;
	}
#pragma endregion
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

	// 全オブジェクト解放
	for (auto* obj : objects_)
	{
		if (obj)
		{
			obj->Release();
			delete obj;
		}
	}
	objects_.clear();

	//enemyManager_->Release();
	//delete enemyManager_;

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