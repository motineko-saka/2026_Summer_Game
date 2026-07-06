#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/StageManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/SkyDome.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/GameObject/ObjectBase.h"
#include "../Object/Actor/Wall.h"
#include "../Object/LightPillar.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Audio/AudioManager.h"
#include "GameScene.h"
#include "GameClearScene.h"
#include "PauseScene.h"
#include "TitleScene.h"

GameScene::GameScene(void)
	:
	stageManager_(nullptr),
	skyDome_(nullptr),
	screenHandle1_(-1),
	screenHandle2_(-1),
	screenWidth_(0),
	screenHeight_(0),
	pinID_(-1),
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

	ansVec_ = ANSWER_VECTOR;

	// 分割画面用のスクリーン作成(左右画面)
	int halfWidth = screenWidth_ / 2;
	screenHandle1_ = MakeScreen(halfWidth, screenHeight_, true);
	screenHandle2_ = MakeScreen(halfWidth, screenHeight_, true);

	isPause_ = false;

	lightPillar_ = std::make_unique<LightPillar>();

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

		players_[i].isPlayerHitObject_ = false;
	}

	// ステージ
	stageManager_ = std::make_unique<StageManager>(SceneManager::SCENE::MAIN);
	stageManager_->InitStage();

	// スカイドーム(プレイヤー1用)
	skyDome_ = std::make_unique<SkyDome>(players_[0].player_->GetTransform());
	skyDome_->Init();

	CreateWall(*stageManager_);

	// エネミー管理
	//enemyManager_ = new EnemyManager(player1_);
	//enemyManager_->Init();

	// オブジェクト作成（複数）
	objects_.reserve(5);

	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::DEFAULT));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 1260.0f, 0.0f, -50.5f });
	objects_.back()->SetPosition({ 1260.0f, 0.0f, -50.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });


	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::WBOX));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 1260.0f, 0.0f, -50.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });

	objects_.push_back(new ObjectBase(GameScene::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[2], ObjectBase::OBJECT_TYPE::AKEG));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -1260.0f, 0.0f, -50.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });

	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[3], ObjectBase::OBJECT_TYPE::BUTTON));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 1000.0f, 0.0f, -50.5f });
	//objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });
	objects_.back()->SetPosition({ 0.0f, 80.0f, -50.0f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });

	objects_.push_back(new ObjectBase(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -900.0f, 0.0f, 0.5f });
	//objects_.back()->SetPosition({500.0f, -720.0f, -50.5f });
	//objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });
	//objects_.back()->SetPosition({ 0.0f, 80.0f, -50.0f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });


#pragma region コライダ登録

	// ステージの各コライダをプレイヤー／カメラ／オブジェクトに登録
	for (const auto& stage : stageManager_->GetStage())
	{
		const ColliderBase* stageCollider =
			stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		for (auto player : players_)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			player.player_->AddHitCollider(stageCollider);
			player.camera_->AddHitCollider(stageCollider);
		}

		// ステージモデルのコライダーを全オブジェクトに登録
		for (auto* obj : objects_)
		{
			obj->AddHitCollider(stageCollider);
		}

		if (stageCollider == nullptr) DrawFormatString(100, 100, 0xffffff, "stageCollider is null\n");
	}

	// 踏むButtonのindexをとる
	std::vector<int> pushButtonIndex = {};

	for (int i = 0; i < objects_.size(); i++)
	{
		auto& obj = objects_[i];

		if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::PRESS_BUTTON) continue;

		pushButtonIndex.push_back(i);
	}

	// 各オブジェクトの衝突コライダをプレイヤーに登録
	for (int i = 0; i < objects_.size(); i++)
	{
		auto& obj = objects_[i];

		const auto* objCaps = obj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));

		if (!objCaps) continue;

		for (int i = 0; i < players_.size(); i++)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			players_[i].player_->AddHitCollider(objCaps);
		}

		for (auto index : pushButtonIndex)
		{
			if (i != index)
			{
				objects_[index]->AddHitCollider(objCaps);
			}
		}
	}

	for (auto& wall : walls_)
	{
		const ColliderBase* wallCollider =
			wall->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		for (int i = 0; i < players_.size(); i++)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			players_[i].player_->AddHitCollider(wallCollider);
		}
	}

#pragma endregion

	// 衝突フラグの初期化
	ansVec_ = ANSWER_VECTOR;

	// 初期アクティブ状態（プレイヤー1 を操作）
	activePlayer_ = Player::PLAYER_NO::PLAYER1;
	for (int i = 0; i < players_.size(); i++)
	{
		bool isActive = i == 0 ? true : false;
		players_[i].player_->SetActive(isActive);
		players_[i].camera_->SetControlEnabled(isActive);
	}

	// シャドウマップ作成
	shadowMapHandle_ = MakeShadowMap(8192, 8192);

	// ライトの方向 
	SetShadowMapLightDirection(shadowMapHandle_, VGet(0.3f, -0.7f, 0.8f));

	// 影を描画する空間範囲
	SetShadowMapDrawArea(shadowMapHandle_,
		VGet(-5000.0f, -1.0f, -5000.0f),
		VGet(5000.0f, 5000.0f, 5000.0f));

	// シーンのサウンドを読み込み、BGM を再生
	if (AudioManager::GetInstance())
	{
		AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME);
	}
}

void GameScene::Load(void)
{
}

void GameScene::LoadEnd(void)
{
	Init();
}

void GameScene::CheckCollisions(void)
{
	for (auto player : players_)
	{
		player.isPlayerHitObject_ = false;
	}

	std::vector<ObjectBase*> newObjects;  // 新規オブジェクト用

	for (auto& obj : objects_)
	{
		if (obj == nullptr) continue;

		// ボタンタイプの場合は専用処理
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{
			ButtonProcess(*obj, newObjects);
			
			continue;
		}
	}

	// ループ終了後に追加
	MakeNewObject(newObjects);
}

const void GameScene::ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects)
{
	VECTOR objectPos = obj.GetTransform().pos;

	bool isNearButton = false;

	for (auto& player : players_)
	{
		// プレイヤーとの距離チェック
		VECTOR playerPos = player.player_->GetTransform().pos;
		float distance1 = VSize(VSub(playerPos, objectPos));
		if (distance1 < 180.0f)
		{
			isNearButton = true;
			// ボタンが押されたときの処理（例：ゲームクリア、ドアが開くなど）
		}
	}

	// ボタンの近くにいて、スペースキーか左ボタンが押されたら
	if (isNearButton &&
		(InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT)))
	{
		obj.SetButtomPushed(true);
		// 直接追加せず、一時リストに格納
		ObjectBase* newObj = new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::AKEG);
		newObjects.push_back(newObj);
	}
}

const void GameScene::MakeNewObject(std::vector<ObjectBase*>& newObjects)
{
	for (auto& newObj : newObjects)
	{
		newObj->Init();
		newObj->SetPosition({ 0.0f, 200.0f, -0.5f });
		newObj->SetScale({ 3.0, 3.0, 3.0 });
		for (const auto& stage : stageManager_->GetStage())
		{
			// ステージモデルのコライダーをオブジェクトに登録
			const ColliderBase* stageCollider =
				stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

			newObj->AddHitCollider(stageCollider);
		}

		// オブジェクトの衝突コライダをプレイヤーに登録
		const ColliderBase* objCaps = newObj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
		if (!objCaps) return;

		for (auto player : players_)
		{
			player.player_->AddHitCollider(objCaps);
		}

		//player1_->AddHitCollider(objCaps);
		//player2_->AddHitCollider(objCaps);

		objects_.push_back(newObj);
	}
}

void GameScene::Update(void)
{
	isPause_ = false;
	// ポーズ画面を積む
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
		isPause_ = true;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_P))
	{
		// BGM停止とシーン用サウンドの削除
		if (AudioManager::GetInstance())
		{
			AudioManager::GetInstance()->StopBGM();
			AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
		}

		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_Q))
	{
		// BGM停止とシーン用サウンドの削除
		if (AudioManager::GetInstance())
		{
			AudioManager::GetInstance()->StopBGM();
			AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
		}

		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClearScene>());
		return;
	}

	// プレイヤー選択切替（TAB か 右クリック)
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_TAB) ||
		InputManager::GetInstance()->IsTrgMouseRight())
	{
		for (int i = 0; i < players_.size(); i++)
		{
			bool isNo = activePlayer_ == static_cast<Player::PLAYER_NO>(i) ? false : true;
			players_[i].player_->SetActive(isNo);
			players_[i].camera_->SetControlEnabled(isNo);
		}
		activePlayer_ = (activePlayer_ == Player::PLAYER_NO::PLAYER1) ?
			Player::PLAYER_NO::PLAYER2 : Player::PLAYER_NO::PLAYER1;
	}

	//// 右クリックでもプレイヤー切替
	//if (InputManager::GetInstance()->IsTrgMouseRight())
	//{
	//	if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
	//	{
	//		activePlayer_ = Player::PLAYER_NO::PLAYER2;
	//		player1_->SetActive(false);
	//		player2_->SetActive(true);
	//		camera1_->SetControlEnabled(false);
	//		camera2_->SetControlEnabled(true);
	//	}
	//	else
	//	{
	//		activePlayer_ = Player::PLAYER_NO::PLAYER1;
	//		player1_->SetActive(true);
	//		player2_->SetActive(false);
	//		camera1_->SetControlEnabled(true);
	//		camera2_->SetControlEnabled(false);
	//	}
	//}

	stageManager_->Update();
	skyDome_->Update();
	for (int i = 0; i < players_.size(); i++)
	{
		players_[i].player_->Update();
		players_[i].camera_->Update();
	}

	//enemyManager_->Update();
	// プレイヤー1用のカメラ設定
	

	//wall_->Update();
	lightPillar_->Update();

	// 衝突判定チェック(Objectの更新前に実行)
	CheckCollisions();

	// 全オブジェクトの更新
	for (auto& obj : objects_)
	{
		if (obj) obj->Update();
	}

	// 踏む
	for (auto& obj : objects_)
	{
		if (obj && obj->GetType() == ObjectBase::OBJECT_TYPE::PRESS_BUTTON)
		{
			if (obj->IsPushButtonPressed())
			{
				// ボタンが踏まれたときの処理
				obj->PushButton();
			}
		}
	}

	for (int i = 0; i < objects_.size(); i++)
	{
		auto& obj = objects_[i];
		if (obj->IsGrabbed())
		{
			DrawSphere3D(ANSWER_VECTOR_LENGTH[i], 20.0f, 180, 0xffffff, 0xffffff, true);
			
		}
	}
	DrawSphere3D(ANSWER_VECTOR_LENGTH[0], 80.0f, 16, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);
	//DrawSphere3D(ANSWER_VECTOR_LENGTH[1], 20.0f, 180, 0xffffff, 0xffffff, true);

	// 答えの場所に全てのオブジェクトがあるか判定
	bool isAnswer = true;

	for (auto& obj : objects_)
	{
		if (!obj->IsAnswerPosition())
		{
			isAnswer = false;
		}
	}

	if (isAnswer)
	{
		
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClearScene>());
	}
}

void GameScene::Draw(void)
{
	ShadowMap_DrawSetup(shadowMapHandle_);

	// 影を落とすモデルだけ描く
	players_[0].player_->Draw();
	players_[1].player_->Draw();
	stageManager_->Draw();

	ShadowMap_DrawEnd();

	SetUseShadowMap(0, shadowMapHandle_);

	int halfWidth = screenWidth_ / 2;

	for (int i = 0; i < players_.size(); i++)
	{
		auto& screenHandle_ = Player::PLAYER_NO::PLAYER1 == static_cast<Player::PLAYER_NO>(i) ?
			screenHandle1_ : screenHandle2_;
		SetDrawScreen(screenHandle_);
		ClearDrawScreen();
	
		// プレイヤー1用のカメラ設定
		players_[i].camera_->SetBeforeDraw();
	
		// 3D描画
		skyDome_->Draw();
		stageManager_->Draw();
		lightPillar_->Draw();
		
		for (int j = 0; j < players_.size(); j++)
		{
			players_[j].player_->Draw();
		}
	
		// 答えの描画
		for (int i = 0; i < objects_.size(); i++)
		{
			auto& obj = objects_[i];
	
			if (!obj->IsGrabbed()) continue;
			// 持っている
			// 答えの場所に描画
			DrawSphere3D(ANSWER_VECTOR_LENGTH[i], 80.0f, 16, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);
	
			MV1SetPosition(pinID_, ANSWER_VECTOR_LENGTH[i]);
			MV1DrawModel(pinID_);
		}
	
		//for (auto& wall : walls_)
		//{
		//	wall->Draw();
		//}
	
		// 全オブジェクトを順に描画（それぞれの viewWorld を設定）
		for (auto* obj : objects_)
		{
			if (obj == nullptr) continue;
			//obj->SetViewWorld(WORLD::LEFT);
			obj->Draw();
		}
	}

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

#ifdef _DEBUG

	for (int i = 0; i < players_.size(); i++)
	{
		int w = halfWidth * i;
		DrawFormatString(halfWidth, 0, GetColor(255, 255, 255), "P%d角度:(%.1f, %.1f, %.1f)",
			i + 1,
			players_[i].player_->GetTransform().quaRot.ToEuler().x,
			players_[i].player_->GetTransform().quaRot.ToEuler().y,
			players_[i].player_->GetTransform().quaRot.ToEuler().z);
	}

	for (auto& player : players_)
	{
		// 衝突判定結果の表示(プレイヤー2側)
		if (player.isPlayerHitObject_)
		{
			DrawFormatString(halfWidth, 40, GetColor(255, 0, 0), "P2: オブジェクトと衝突中!");
		}
		else
		{
			DrawFormatString(halfWidth, 40, GetColor(0, 255, 0), "P2: 衝突なし");
		}
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

	VECTOR min, max;

	for(auto& stage : stageManager_->GetStage())
	{
		auto bb = stage->GetBoundingBox();
		
		float centerX = (bb.minPos.x + bb.maxPos.x) * 0.5f;
		float centerZ = (bb.minPos.z + bb.maxPos.z) * 0.5f;

		// 真ん中
		DrawSphere3D(VGet(centerX, 0, centerZ), 10.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);

		// 奥手前（Z固定、Xはステージ中央）
		DrawSphere3D(VGet(centerX, 0, bb.minPos.z), 10.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
		DrawSphere3D(VGet(centerX, 0, bb.maxPos.z), 10.0f, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);

		// 左右（X固定、Zはステージ中央）
		DrawSphere3D(VGet(bb.minPos.x, 0, centerZ), 10.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
		DrawSphere3D(VGet(bb.maxPos.x, 0, centerZ), 10.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
	}
	
#endif // _DEBUG
#pragma endregion
}

void GameScene::Release(void)
{
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

	for (auto& player : players_)
	{
		player.player_->Release();
		player.camera_->Release();
	}
	players_.clear();

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