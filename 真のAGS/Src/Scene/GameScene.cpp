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
#include "../Object/Actor/Charactor/GameObject/Panel.h"
#include "../Object/Actor/Charactor/GameObject/Board.h"
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
	Release();
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
	isBreak_ = false;
	isRot_ = false;

	lightPillar_ = std::make_unique<LightPillar>();

	players_.resize(PLAYER_NUM);

	// グローバルカメラ
	camera_ = new Camera();
	camera_->Init();

	// プレイヤー＆カメラ生成
	for (size_t i = 0; i < players_.size(); ++i)
	{
		players_[i].camera_ = std::make_unique<Camera>();
		players_[i].camera_->Init();

		Player::PLAYER_NO pno = (i == 0) ? Player::PLAYER_NO::PLAYER1 : Player::PLAYER_NO::PLAYER2;
		players_[i].player_ = std::make_unique<Player>(pno, *players_[i].camera_);
		players_[i].player_->Init();

		players_[i].camera_->SetFollow(&players_[i].player_->GetTransform());
		players_[i].camera_->ChangeMode(Camera::MODE::FOLLOW);
		players_[i].camera_->Update();

		players_[i].isPlayerHitObject_ = false;
	}

	players_[0].camera_->SetMouseCenter(screenWidth_ / 4, screenHeight_ / 2);
	players_[1].camera_->SetMouseCenter(screenWidth_ * 3 / 4, screenHeight_ / 2);

	// ステージ
	stageManager_ = std::make_unique<StageManager>(SceneManager::SCENE::MAIN);
	stageManager_->InitStage();

	// スカイドーム(プレイヤー1用)
	skyDome_ = std::make_unique<SkyDome>(players_[0].player_->GetTransform());
	skyDome_->Init();

	CreateWallGame(*stageManager_);

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

	//objects_.push_back(std::make_unique<Gaer>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR, *objects_[4]));
	//objects_.back()->Init();
	//objects_.back()->SetPosition({ -600.0f, 100.0f, 0.0f });
	//objects_.back()->SetScale(AsoUtility::VECTOR_ONE);
	objects_.push_back(std::make_unique<Gaer>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -600.0f, -620.0f, 0.0f });
	objects_.back()->SetScale(AsoUtility::VECTOR_ONE);

	PushObject<Object>(GameScene::WORLD::LEFT, ANSWER_VECTOR_LENGTH[4], ObjectBase::OBJECT_TYPE::GEAR_OBJECT, 
		{ -600.0f, -620.0f, 0.0f }, AsoUtility::VECTOR_ONE);


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

	for (const auto& obj : objects_)
	{
		if (obj->GetType() != ObjectBase::OBJECT_TYPE::GEAR) continue;

		 Gaer* gaer = dynamic_cast<Gaer*>(obj.get());

		if (gaer == nullptr)
			continue;

		int gaerObjectNumber = -1;

		for (int i = 0; i < objects_.size(); i++)
		{
			if (objects_[i]->GetObjectType() == ObjectBase::OBJECT_TYPE::GEAR_OBJECT)
			{
				gaerObjectNumber = i;
				break;
			}
		}

		gaer->AddObject(objects_[gaerObjectNumber].get());
	}
	// Board と Panel の初期化
	InitializeBoardAndPanels();


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

	//for (int i = 0; i < panels_.size(); i++)
	//{
	//	auto& panel = panels_[i];

	//	const auto* panelCaps =
	//		panel->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));

	//	if (!panelCaps) continue;

	//	for (auto& player : players_)
	//	{
	//		// ステージモデルのコライダーをプレイヤーに登録
	//		player.player_->AddHitCollider(panelCaps);
	//	}
	//}

	for (int i = 0; i < players_.size(); i++)
	{
		auto& player = players_[i];

		const auto* playerCaps =
			player.player_->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));

		if (!playerCaps) continue;

		for (auto& panel : panels_)
		{
			// ステージモデルのコライダーをプレイヤーに登録
			panel->AddHitCollider(playerCaps);
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
		walls_.pop_back();
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

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE) || 
		InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::OPTION))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
		isPause_ = true;
	}

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_P))
	{
		ChangeScene(std::make_shared<TitleScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_Q) || isClear_)
	{
		ChangeScene(std::make_shared<GameClearScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_H))
	{
		ChangeScene(std::make_shared<GameOverScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_8))
	{
		walls_.pop_back();
	}
#endif

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

	// クリア判定
	if (board_ && board_->CheckClear())
	{
		isClear_ = true;
		return;
	}

	for (const auto& player : players_)
	{
		const auto& playerPos = player.player_->GetTransform().pos;
		float distance = VSize(VSub(playerPos, endPos_));
		if (distance < 100.0f)
		{
			isClear_ = true;
			return;
		}
	}

	stageManager_->Update();
	skyDome_->Update();

	for (int i = 0; i < players_.size(); i++)
	{
		players_[i].player_->Update();
		players_[i].camera_->Update();
	}

	lightPillar_->Update();

	if (board_)
	{
		board_->Update();
	}

	for (auto& panel : panels_)
	{
		if (panel)
		{
			panel->Update();
		}
	}

	// 既存のコード...
	CheckCollisions();

	for (auto& obj : objects_)
	{
		if (obj) obj->Update();
	}

}

void GameScene::Draw(void)
{
	ShadowMap_DrawSetup(shadowMapHandle_);

	players_[0].player_->Draw();
	players_[1].player_->Draw();
	stageManager_->Draw();

	for (auto& obj : objects_)
	{
		obj->Draw();
	}

	// Panel の描画
	for (auto& panel : panels_)
	{
		if (panel)
		{
			panel->Draw();
		}
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

		players_[i].camera_->SetBeforeDraw();

		skyDome_->Draw();
		stageManager_->Draw();
		lightPillar_->Draw();

		for (int j = 0; j < players_.size(); j++)
		{
			players_[j].player_->Draw();
		}

		for (int i = 0; i < objects_.size(); i++)
		{
			auto& obj = objects_[i];
			if (!obj->IsGrabbed()) continue;
			DrawSphere3D(ANSWER_VECTOR_LENGTH[i], 80.0f, 16, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);
			MV1SetPosition(pinID_, ANSWER_VECTOR_LENGTH[i]);
			MV1DrawModel(pinID_);
		}

		//for (auto& wall : walls_)
		//{
		//	wall->Draw();
		//}

		for (auto& obj : objects_)
		{
			if (obj == nullptr) continue;
			obj->Draw();
		}

<<<<<<< HEAD
		// Board の描画
		if (board_)
		{
			board_->Draw();
		}

		// Panel の描画
		for (auto& panel : panels_)
		{
			if (panel)
			{
				panel->Draw();
			}
		}

		DrawNamePlate("ゴール", endPos_);
=======
		// インタラクト文字表示
		for (auto& obj : objects_)
		{
			if (!obj) continue;
			// 既に掴んでいる物は表示しない
			if (obj->IsGrabbed()) continue;
			if (CheckCameraViewClip(obj->GetPos())) continue;

			// 対象とするオブジェクト種類
			const auto t = obj->GetObjectType();
			if (t != ObjectBase::OBJECT_TYPE::ROCK &&
				t != ObjectBase::OBJECT_TYPE::AXE &&
				t != ObjectBase::OBJECT_TYPE::WBOX &&
				t != ObjectBase::OBJECT_TYPE::BUTTON)
				continue;

			// プレイヤーとの距離
			const float dist = VSize(VSub(players_[i].player_->GetTransform().pos, obj->GetTransform().pos));
			if (dist > INTERACT_DISTANCE) continue;

			// オブジェクト種類
			std::string label;
			switch (obj->GetObjectType())
			{
			case ObjectBase::OBJECT_TYPE::ROCK:
				label = "呼び設定";
				break;
			case ObjectBase::OBJECT_TYPE::BUTTON:
				label = "Fで押す";
				break;
			case ObjectBase::OBJECT_TYPE::AXE:
				label = "Eで持てる";
				break;
			case ObjectBase::OBJECT_TYPE::WBOX:
				label = "Eで開く、閉じる";
				break;
			default:
				break;
			}

			// ワールド座標をスクリーンへ変換して描画
			VECTOR screenPos = ConvWorldPosToScreenPos(obj->GetPos());
			const int textW = GetDrawStringWidth(label.c_str(), static_cast<int>(label.length()));
			const int drawX = static_cast<int>(screenPos.x) - (textW / 2);
			const int drawY = static_cast<int>(screenPos.y) - 80; // 表示オフセット調整

			// 背景ボックス（半透明黒）
			const int pad = 6;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
			DrawBox(drawX - pad, drawY - pad, drawX + textW + pad, drawY + 18 + pad, GetColor(0, 0, 0), TRUE);
			DrawFormatString(drawX, drawY, GetColor(255, 255, 255), label.c_str());
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		/*if (EffekseerEffect::GetInstance()) EffekseerEffect::GetInstance()->Draw();*/

		DrawNamePlate("ゴール", endPos_);;
>>>>>>> 3b12d7343cea60c22c85d594c3f3bff7b2ac1b10
	}

	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	DrawExtendGraph(0, 0, halfWidth, screenHeight_, screenHandle1_, true);
	DrawExtendGraph(halfWidth, 0, screenWidth_, screenHeight_, screenHandle2_, true);

	int dimAlpha = 150;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, dimAlpha);
	if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
	{
		DrawBox(halfWidth, 0, screenWidth_, screenHeight_, GetColor(0, 0, 0), TRUE);
	}
	else
	{
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

	for (auto& stage : stageManager_->GetStage())
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
#endif
#pragma endregion
}

void GameScene::DrawNamePlate(std::string str, VECTOR pos)
{
	const auto objectPos = ConvWorldPosToScreenPos(pos);
	const int strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
	const int drawX = static_cast<int>(objectPos.x) - (strWidth / 2);

	DrawFormatString(drawX, static_cast<int>(objectPos.y) - 120, 0xffff00, str.c_str());
	DrawFormatString(static_cast<int>(objectPos.x), static_cast<int>(objectPos.y) - 100, 0xffff00, "↓");
}

void GameScene::ChangeScene(const std::shared_ptr<SceneBase>& scene) const
{
	// BGM停止とシーン用サウンドの削除
	if (AudioManager::GetInstance())
	{
		AudioManager::GetInstance()->StopBGM();
		AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
	}

	//DeleteShadowMap(shadowMapHandle_);
	SceneManager::GetInstance()->ChangeScene(scene);
}

void GameScene::Release(void)
{
<<<<<<< HEAD
	AudioManager::GetInstance()->DeleteInstance();
=======
	// オーディオマネージャーのインスタンスの削除（存在チェック）
	if (AudioManager::GetInstance())
	{
		AudioManager::GetInstance()->DeleteInstance();
	}
>>>>>>> 3b12d7343cea60c22c85d594c3f3bff7b2ac1b10

	// オブジェクトの破棄（unique_ptr がリソースを解放）
	objects_.clear();
	panels_.clear();
	board_.reset();

	// ピンモデルを安全に削除
	if (pinID_ != -1)
	{
		MV1DeleteModel(pinID_);
		pinID_ = -1;
	}

<<<<<<< HEAD
	players_.clear();

=======
	// シャドウマップを安全に削除
	if (shadowMapHandle_ != -1)
	{
		DeleteShadowMap(shadowMapHandle_);
		shadowMapHandle_ = -1;
	}

	// グローバル camera_ は raw pointer -> delete して nullptr に
	if (camera_ != nullptr)
	{
		delete camera_;
		camera_ = nullptr;
	}

	players_.clear();

	// walls, skyDome, stageManager, lightPillar のリセット
	walls_.clear();
	if (skyDome_) skyDome_.reset();
	if (stageManager_) stageManager_.reset();
	if (lightPillar_) lightPillar_.reset();

	// スクリーンハンドル削除
>>>>>>> 3b12d7343cea60c22c85d594c3f3bff7b2ac1b10
	if (screenHandle1_ != -1)
	{
		DeleteGraph(screenHandle1_);
		screenHandle1_ = -1;
	}
	if (screenHandle2_ != -1)
	{
		DeleteGraph(screenHandle2_);
		screenHandle2_ = -1;
	}
}

void GameScene::InitializeBoardAndPanels(void)
{
	// Board の作成と初期化
	board_ = std::make_unique<Board>();

	// 初期状態
	std::array<std::array<Board::ELEMENT, Board::STAGE_SIZE>, Board::STAGE_SIZE> initialBoard =
	{ {
		{{ Board::ELEMENT::ICE,  Board::ELEMENT::FIRE,  Board::ELEMENT::ICE  }},
		{{ Board::ELEMENT::FIRE, Board::ELEMENT::WATER, Board::ELEMENT::FIRE }},
		{{ Board::ELEMENT::ICE,  Board::ELEMENT::FIRE,  Board::ELEMENT::ICE  }}
	} };

	board_->Initialize(initialBoard);

	// Panel の作成
	panels_.clear();
	panels_.reserve(Board::STAGE_SIZE * Board::STAGE_SIZE);

	for (int y = 0; y < Board::STAGE_SIZE; ++y)
	{
		for (int x = 0; x < Board::STAGE_SIZE; ++x)
		{
			// Panel をワールド座標に配置
			VECTOR panelPos = board_->GetPanelCenterPos(x, y);
			panelPos.y = 0.0f;

			auto panel = std::make_unique<Panel>(
				SceneBase::WORLD::LEFT,
				VECTOR{ 0.0f, 0.0f, 0.0f },
				ObjectBase::OBJECT_TYPE::BUTTON
			);

			panel->SetIndex(x, y);
			panel->SetBoard(board_.get());
			panel->SetPosition(panelPos);
			panel->Init();

			panels_.push_back(std::move(panel));
		}
	}
}