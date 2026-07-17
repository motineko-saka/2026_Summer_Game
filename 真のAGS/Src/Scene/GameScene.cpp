#include <DxLib.h>
#include <string>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/StageManager.h"
#include "../Manager/Resource.h"
#include "../Utility/AsoUtility.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/SkyDome.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/GameObject/ObjectBase.h"
#include "../Object/Actor/Charactor/GameObject/Object.h"
#include "../Object/Actor/Charactor/GameObject/Button.h"
#include "../Object/Actor/Charactor/GameObject/PressButton.h"
#include "../Object/Actor/Charactor/GameObject/Gaer.h"
#include "../Object/Actor/Charactor/GameObject/Rock.h"
#include "../Object/Actor/Charactor/GameObject/Axe.h"
#include "../Object/Actor/Charactor/GameObject/Gate.h"
#include "../Object/Actor/Wall.h"
#include "../Object/LightPillar.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Audio/AudioManager.h"
#include "GameScene.h"
#include "GameClearScene.h"
#include "PauseScene.h"
#include "TitleScene.h"
#include "GameOverScene.h"

GameScene::GameScene(void)
	:
	stageManager_(nullptr),
	skyDome_(nullptr),
	screenHandle1_(-1),
	screenHandle2_(-1),
	screenWidth_(0),
	screenHeight_(0),
	pinID_(-1),
	shadowMapHandle_(-1),
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
	screenHandle1_ = MakeScreen(halfWidth, screenHeight_, true);
	screenHandle2_ = MakeScreen(halfWidth, screenHeight_, true);

	isPause_ = false;
	isClear_ = false;

	lightPillar_ = std::make_unique<LightPillar>();

	players_.resize(PLAYER_NUM);

	for (int i = 0; i < players_.size(); i++)
	{
		players_[i].camera_ = std::make_unique<Camera>();
		players_[i].camera_->Init();

		// プレイヤー番号を設定
		Player::PLAYER_NO pno = (i == 0) ? Player::PLAYER_NO::PLAYER1 : Player::PLAYER_NO::PLAYER2;
		players_[i].player_ = std::make_unique<Player>(pno, *players_[i].camera_, true);
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
	objects_.reserve(10);

	//auto pushObject = [this](SceneBase::WORLD w, const VECTOR& ans, ObjectBase::OBJECT_TYPE type, const VECTOR& pos, const VECTOR& scl, bool placed = false) {
	//	std::unique_ptr<ObjectBase> o = std::make_unique<ObjectBase>(w, ans, type);
	//	o->Init();
	//	o->SetPosition(pos);
	//	o->SetScale(scl);
	//	if (placed) o->SetPlaced(true);
	//	objects_.push_back(std::move(o));
	//	};

	//// ボタンを左右両方に配置
	//pushObject(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::BUTTON, buttonPos_, { 0.5f, 0.5f, 0.5f }, true);
	//pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::ROCK, rockPos_, { 1.0f,1.0f,1.0f }, true);

	//pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::KINOKO, { -500.0f, 0.0f, 0.0f }, { 8.0, 8.0, 8.0 }, false);
	//pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::DEFAULT, { 1300.0f, -320.0f, 440.0f }, { 1.0f,1.0f,1.0f }, true);

	
	PushObject<Button>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[3], ObjectBase::OBJECT_TYPE::BUTTON, buttonPos_, VScale(AsoUtility::VECTOR_ONE, 0.5f));
	PushObject<Rock>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::ROCK, rockPos_, AsoUtility::VECTOR_ONE);
	PushObject<Axe>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::AXE, { -500.0f, 0.0f, 0.0f }, VScale(AsoUtility::VECTOR_ONE, 8.0f));
	PushObject<Gate>(GameScene::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::DEFAULT, { 1300.0f, -320.0f, 500.0f }, AsoUtility::VECTOR_ONE);

	/*PushObject<Gaer>(GameScene::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR, 
		{ -600.0f, 100.0f, 0.0f }, AsoUtility::VECTOR_ONE);*/
	
	//PushObject<Rock>(GameScene::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::DEFAULT, { 1300.0f, -320.0f, 500.0f }, AsoUtility::VECTOR_ONE);

	objects_.push_back(std::make_unique<Gaer>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR, *objects_[4]));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -600.0f, 100.0f, 0.0f });
	objects_.back()->SetScale(AsoUtility::VECTOR_ONE);

	//objects_.push_back(std::make_unique<Gaer>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR, objects_[4]));
	//objects_.back()->Init();
	//objects_.back()->SetPosition({ -650.0f, 110.0f, 0.0f });
	//objects_.back()->SetScale(AsoUtility::VECTOR_ONE);
	
	//objects_.push_back(std::make_unique<Button>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[3], ObjectBase::OBJECT_TYPE::BUTTON));
	//objects_.back()->Init();
	//objects_.back()->SetPosition(buttonPos_);
	//objects_.back()->SetScale(VScale(AsoUtility::VECTOR_ONE, 0.5f));

	//objects_.push_back(std::make_unique<Rock>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::ROCK));
	//objects_.back()->Init();
	//objects_.back()->SetPosition(rockPos_);
	//objects_.back()->SetScale(AsoUtility::VECTOR_ONE);

	//objects_.push_back(std::make_unique<Axe>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::AXE));
	//objects_.back()->Init();
	//objects_.back()->SetPosition({ -500.0f, 0.0f, 0.0f });
	//objects_.back()->SetScale(VScale(AsoUtility::VECTOR_ONE, 8.0f));

	//objects_.push_back(std::make_unique<Gate>(GameScene::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::DEFAULT));
	//objects_.back()->Init();
	//objects_.back()->SetPosition({ 1300.0f, -320.0f, 500.0f });
	//objects_.back()->SetScale(AsoUtility::VECTOR_ONE);


#pragma region コライダ登録

	// ステージの各コライダをプレイヤー／カメラ／オブジェクトに登録
	for (const auto& stage : stageManager_->GetStage())
	{
		const ColliderBase* stageCollider =
			stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		for (auto& player : players_)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			player.player_->AddHitCollider(stageCollider);
			player.camera_->AddHitCollider(stageCollider);
		}

		// ステージモデルのコライダーを全オブジェクトに登録
		for (auto& obj : objects_)
		{
			obj->AddHitCollider(stageCollider);
		}

		//if (stageCollider == nullptr) DrawFormatString(100, 100, 0xffffff, "stageCollider is null\n");
	}

	// 踏むButtonのindexをとる
	std::vector<int> pushButtonIndex = {};

	for (int i = 0; i < objects_.size(); i++)
	{
		auto& obj = objects_[i];

		if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::PRESS_BUTTON) continue;

		pushButtonIndex.push_back(i);
	}

	// 岩のindexをとる
	std::vector<int> rockButtonIndex = {};

	for (int i = 0; i < objects_.size(); i++)
	{
		auto& obj = objects_[i];

		if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::ROCK) continue;

		rockButtonIndex.push_back(i);
	}

	// 各オブジェクトの衝突コライダをプレイヤーに登録
	for (int i = 0; i < objects_.size(); i++)
	{
		auto& obj = objects_[i];

		const auto* objCaps = 
			obj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));

		if (!objCaps) continue;

		for (auto& player : players_)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			player.player_->AddHitCollider(objCaps);
		}

		for (auto index : pushButtonIndex)
		{
			if (i != index)
			{
				objects_[index]->AddHitCollider(objCaps);
			}
		}
	}

	const auto* objCaps = objects_[2]->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
	objects_[1]->AddHitCollider(objCaps);

	// 壁
	for (auto& wall : walls_)
	{
		const ColliderBase* wallCollider =
			wall->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		for (int i = 0; i < players_.size(); i++)
		{
			// 壁モデルのコライダーをプレイヤーに登録
			players_[i].player_->AddHitCollider(wallCollider);
		}
	}

#pragma endregion

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

	// オーディオマネージャーのインスタンスの生成
	AudioManager::GetInstance()->CreateInstance();

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
	for (auto& player : players_)
	{
		player.isPlayerHitObject_ = false;
	}

	std::vector<ObjectBase*> newObjects;  // 新規オブジェクト用

	for (auto& obj : objects_)
	{
		if (obj == nullptr) continue;

		// ボタンタイプの場合は専用処理
		if (!obj->isPushButtom() && (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON))
		{
			ButtonProcess(*obj, newObjects);
			
			continue;
		}

		auto& objectPos = obj->GetPos();

		for (auto& player : players_)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			VECTOR playerPos = player.player_->GetTransform().pos;

			float distance1 = VSize(VSub(playerPos, objectPos));
			bool hit = (distance1 < 180.0f);
			if (hit)
			{
				player.isPlayerHitObject_ = true;
				VECTOR pushDir = VSub(objectPos, playerPos);
				pushDir.y = 0.0f; // Y軸(垂直方向)は無視
				pushDir = VNorm(pushDir); // 正規化
			}
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
		if (distance1 < 80.0f)
		{
			isNearButton = true;
		}
	}

	// ボタンの近くにいて、スペースキーか左ボタンが押されたら
	if (isNearButton &&
		(InputManager::GetInstance()->IsTrgDown(KEY_INPUT_F) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT)))
	{
		// ボタンが押されたときの処理（例：ゲームクリア、ドアが開くなど）

		obj.SetButtomPushed(true);

		objects_[0]->SetButtomPushed(true);
		objects_[1]->SetButtomPushed(true);
		objects_[2]->SetButtomPushed(true);
		//objects_[2]->Release();
		//objects_.erase(objects_.begin() + 2);
		for (auto& player : players_)
		{
			player.player_->HitColliderErase(4);
		}
		// 直接追加せず、一時リストに格納
		//ObjectBase* newObj = new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::AKEG);
		//newObjects.push_back(newObj);
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

		for (auto& player : players_)
		{
			player.player_->AddHitCollider(objCaps);
		}

		//player1_->AddHitCollider(objCaps);
		//player2_->AddHitCollider(objCaps);

		objects_.push_back(std::unique_ptr<ObjectBase>(newObj));
	}
}

void GameScene::Update(void)
{
	isPause_ = false;
	// ポーズ画面を積む
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::OPTION))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
		isPause_ = true;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_P)) 
	{
		ChangeScene(std::make_shared<TitleScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_Q))
	{
		ChangeScene(std::make_shared<GameClearScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_H))
	{
		ChangeScene(std::make_shared<GameOverScene>());
		return;
	}

	if (isClear_)
	{
		ChangeScene(std::make_shared<GameClearScene>());
		return;
	}

	// プレイヤー選択切替（TAB か 右クリック)
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_TAB) ||
		InputManager::GetInstance()->IsTrgMouseRight() ||
		InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER))
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

	// クリア
	for (const auto& player : players_)
	{
		// ステージモデルのコライダーをプレイヤーに登録
		const auto& playerPos = player.player_->GetTransform().pos;

		float distance1 = VSize(VSub(playerPos, endPos_));
		float distanceMax = 100.0f;
		bool hit = (distance1 < distanceMax);
		if (hit)
		{
			isClear_ = true;
			return;
		}
	}

	//// 歯車距離処理（後で消す）
	//for (auto& obj : objects_)
	//{
	//	if (obj == nullptr) continue;

	//	// ギアタイプの場合は専用処理
	//	if (obj->GetType() != ObjectBase::OBJECT_TYPE::GEAR)
	//	{
	//		continue;
	//	}

	//	auto& objectPos = obj->GetPos();

	//	for (auto& player : players_)
	//	{
	//		// ステージモデルのコライダーをプレイヤーに登録
	//		VECTOR playerPos = player.player_->GetTransform().pos;

	//		float distance1 = VSize(VSub(playerPos, objectPos));
	//		bool hit = (distance1 < 60.0f);
	//		if (hit)
	//		{
	//			obj->SetIsRot(true);
	//		}
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

	// 削除対象のオブジェクトを配列から削除
	for (auto it = objects_.begin(); it != objects_.end(); )
	{
		if ((*it)->GetObjectType() == ObjectBase::OBJECT_TYPE::ROCK && !(*it)->GetIsRockExist())
		{
			it = objects_.erase(it);
			for (auto& player : players_)
			{
				player.player_->HitColliderErase(2);
			}
		}
		else
		{
			++it;
		}
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
		isClear_ = true;
		return;
	}
}

void GameScene::Draw(void)
{
	ShadowMap_DrawSetup(shadowMapHandle_);

	// 影を落とすモデルだけ描く
	players_[0].player_->Draw();
	players_[1].player_->Draw();
	stageManager_->Draw();
	for (auto& obj : objects_)
	{
		obj->Draw();
	}

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
	
		for (auto& wall : walls_)
		{
			wall->Draw();
		}
	
		// 全オブジェクトを順に描画（それぞれの viewWorld を設定）
		for (auto& obj : objects_)
		{
			if (obj == nullptr) continue;
			//obj->SetViewWorld(WORLD::LEFT);
			obj->Draw();
		}

		DrawNamePlate("ゴール", endPos_);;
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

void GameScene::DrawNamePlate(std::string str, VECTOR pos)
{
	// オブジェクトの位置
	const auto objectPos = ConvWorldPosToScreenPos(pos);

	// 文字の長さ
	const int strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));

	// 半分に
	const int drawX = static_cast<int>(objectPos.x) - (strWidth / 2);

	// 文字を書く
	DrawFormatString(drawX, static_cast<int>(objectPos.y) - 120, 0xffff00, str.c_str());

	// 矢印を書く
	DrawFormatString(static_cast<int>(objectPos.x), static_cast<int>(objectPos.y) - 100, 0xffff00, "　↓");
}

void GameScene::ChangeScene(const std::shared_ptr<SceneBase>& scene) const
{
	// BGM停止とシーン用サウンドの削除
	if (AudioManager::GetInstance())
	{
		AudioManager::GetInstance()->StopBGM();
		AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
	}

	DeleteShadowMap(shadowMapHandle_);
	SceneManager::GetInstance()->ChangeScene(scene);
}

void GameScene::Release(void)
{
	// オーディオマネージャーのインスタンスの削除
	AudioManager::GetInstance()->DeleteInstance();

	objects_.clear();

	MV1DeleteModel(pinID_);

	//enemyManager_->Release();
	//delete enemyManager_;

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