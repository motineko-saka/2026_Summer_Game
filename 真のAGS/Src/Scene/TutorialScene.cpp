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
#include "TutorialScene.h"
#include "../UI/Tutorial.h" 
#include "GameClearScene.h"
#include "PauseScene.h"
#include "TitleScene.h"
#include "../Manager/EffekseerEffect.h"
#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"

TutorialScene::TutorialScene(void)
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

TutorialScene::~TutorialScene(void)
{
	//Release();
}

void TutorialScene::Init(void)
{
	// 画面サイズの取得
	GetScreenState(&screenWidth_, &screenHeight_, nullptr);

	// 分割画面用のスクリーン作成(左右画面)
	int halfWidth = screenWidth_ / 2;
	screenHandle1_ = MakeScreen(halfWidth, screenHeight_, true);
	screenHandle2_ = MakeScreen(halfWidth, screenHeight_, true);

	lightPillar_ = std::make_unique<LightPillar>();

	players_.resize(2);

	// カメラ生成
	camera_ = new Camera();
	camera_->Init();

	for (int i = 0; i < players_.size(); i++)
	{
		players_[i].camera_ = std::make_unique<Camera>();
		players_[i].camera_->Init();

		// プレイヤー番号を設定
		Player::PLAYER_NO pno = (i == 0) ? 
			Player::PLAYER_NO::PLAYER1 : Player::PLAYER_NO::PLAYER2;
		players_[i].player_ = std::make_unique<Player>(pno, *players_[i].camera_);
		players_[i].player_->Init();

		players_[i].camera_->SetFollow(&players_[i].player_->GetTransform());
		players_[i].camera_->ChangeMode(Camera::MODE::FOLLOW);

		players_[i].isPlayerHitObject_ = false;
	}

	players_[0].camera_->SetMouseCenter(
		screenWidth_ / 4,
		screenHeight_ / 2);

	players_[1].camera_->SetMouseCenter(
		screenWidth_ * 3 / 4,
		screenHeight_ / 2);
	pinID_ = -1;

	endTimer_ = 0.0f;
	isEndTutorial_ = false;
	isPillar_ = false;

	// ステージ
	stageManager_ = std::make_unique<StageManager>(SceneManager::SCENE::TUTORIAL);
	stageManager_->InitStage();

	// スカイドーム(プレイヤー1用)
	skyDome_ = std::make_unique<SkyDome>(players_[0].player_->GetTransform());
	skyDome_->Init();

	// オブジェクト作成（複数）
	objects_.reserve(2);

	//objects_.push_back(new ObjectBase(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::AKEG));
	//objects_.back()->Init();
	////objects_.back()->SetPosition({ 1260.0f, -500.0f, -50.5f });
	//objects_.back()->SetPosition({ 1260.0f, -500.0f, -50.5f });
	//objects_.back()->SetScale({ 0.2f, 0.2f, 0.2f });

	objects_.push_back(new ObjectBase(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::BUTTON));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 0.0f, -600.0f, -50.0f });
	objects_.back()->SetScale({ 0.5f, 0.5f, 0.5f });
	objects_.back()->SetPlaced(true);

	objects_.push_back(new ObjectBase(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::CHEST));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 900.0f, -520.0f, 100.0f });
	objects_.back()->SetScale({ 0.5f, 0.5f, 0.5f });
	objects_.back()->SetPlaced(true);

	//objects_.push_back(new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[2], ObjectBase::OBJECT_TYPE::PRESS_BUTTON));
	//objects_.back()->Init();
	//objects_.back()->SetPosition({ -900.0f, -500.0f, 900.5f });
	//objects_.back()->SetScale({ 1.0, 1.0, 1.0 });

	CreateWall(*stageManager_);

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

		for (auto& obj : objects_)
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

	// ポストエフェクト用スクリーン
	postEffectScreen_ = MakeScreen(
		Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	// ポストエフェクト用
	pixelMaterial_ = std::make_unique<PixelMaterial>("Monotone.cso", 1);
	pixelMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	pixelMaterial_->AddTextureBuf(SceneManager::GetInstance()->GetMainScreen());
	pixelRenderer_ = std::make_unique<PixelRenderer>(*pixelMaterial_);
	pixelRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	//for (auto& wall : walls_)
	//{
	//	const ColliderBase* wallCollider =
	//		wall->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	//	for (int i = 0; i < players_.size(); i++)
	//	{
	//		// ステージモデルのコライダーをプレイヤーに登録
	//		players_[i].player_->AddHitCollider(wallCollider);
	//	}
	//}

	// プレイヤーコライダ登録
	for (auto& player : players_)
	{
		const ColliderBase* playerCollider =
			player.player_->GetOwnCollider(static_cast<int>(Player::COLLIDER_TYPE::LINE));

		// Buttonだけ
		for (auto index : pushButtonIndex)
		{
			objects_[index]->AddHitCollider(playerCollider);
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

	TutorialInit();
}

void TutorialScene::TutorialInit(void)
{
	// チュートリアル開始（ステップ登録）
	tutorial_.Init();
	tutorial_.ClearSteps();

	// プレイヤー1 の初期位置をキャプチャ
	const VECTOR p1StartPos = players_[0].player_->GetTransform().pos;

	// ステップ1: 移動
	tutorial_.AddStep(
		"移動の練習：W/A/S/D または 方向キーでプレイヤーを移動させてください。\n実際に移動すると次へ進みます。",
		[this, p1StartPos]() -> bool {
			VECTOR cur = players_[0].player_->GetTransform().pos;
			float moved = VSize(VSub(cur, p1StartPos));
			return moved > moveStepe_;
		}
	);

	// ステップ2: 視点操作
	tutorial_.AddStep(
		"視点操作の練習：矢印キーで視点を動かしてください。\n視点操作を行うと次へ進みます。",
		[]() -> bool {
			return CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_DOWN) ||
				CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_RIGHT);
		}
	);

	// ステップ3: ボタン操作
	tutorial_.AddStep(
		"ボタン操作の練習：ボタンの近くで Space を押してください。\nボタンを押すと次へ進みます。",
		[this]() -> bool {
			for (auto* obj : objects_)
			{
				if (obj == nullptr) continue;

				if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
				{
					// ボタンが押されたかを確認
					if (obj->isPushButtom())
					{
						return true;
					}
				}
			}
			return false;
		}
	);

	// ステップ4: キャラ切替
	tutorial_.AddStep(
		"プレイヤーの切替の練習：Tab または 右クリックで操作プレイヤー2に切り替えてください。\n切替操作を行うと次へ進みます。",
		[]() -> bool {
			return InputManager::GetInstance()->IsTrgDown(KEY_INPUT_TAB) || InputManager::GetInstance()->IsTrgMouseRight();
		}
	);

	// ステップ5: オブジェクトの操作
	tutorial_.AddStep(
		"オブジェクト操作の練習1：オブジェクトに近づいて E を押してください。\nオブジェクトを押すと次へ進みます。",
		[this]() -> bool {
			for (auto* obj : objects_)
			{
				if (obj == nullptr) continue;

				for (auto& player : players_)
				{
					VECTOR objPos = obj->GetTransform().pos;

					VECTOR pPos = player.player_->GetTransform().pos;
					float dist = VSize(VSub(pPos, objPos));
					if (dist < 180.0f && InputManager::GetInstance()->IsTrgDown(KEY_INPUT_E)) return true;
				}
			}
			return false;
		}
	);

	// ステップ6: オブジェクトの設置
	tutorial_.AddStep(
		"オブジェクト操作の練習2 : オブジェクトを正しい位置に配置してください。\nオブジェクトを配置すると次に進む。",
		[this]() -> bool {
			for (auto* obj : objects_)
			{
				if (obj == nullptr) continue;
				if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::AKEG) continue;
				if (obj->IsAnswerPosition()) return true;
			}
			return false;
		}
	);

	// 最終ステップ: 確認して終了
	tutorial_.AddStep(
		"チュートリアル完了：Z / Enter / Space でチュートリアルを終了します。",
		[this]() -> bool {
			bool ret = false;
			if (CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE))
			{
				ret = true;
				isEndTutorial_ = true;
			}
			return ret;

		}
	);

	// 開始
	tutorial_.Start();
}

void TutorialScene::Load(void)
{
}

void TutorialScene::LoadEnd(void)
{
	Init();
}

void TutorialScene::CheckCollisions(void)
{
	for (auto& player : players_)
	{
		player.isPlayerHitObject_ = false;
	}

	std::vector<ObjectBase*> newObjects;  // 新規オブジェクト用

	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;

		VECTOR objectPos = obj->GetTransform().pos;

		// ボタンタイプの場合は専用処理（ButtonProcess に委譲）
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{
			// ButtonProcess は近接と Space 判定を行い、
			ButtonProcess(*obj, newObjects);
			continue;
		}

		for (auto& player : players_)
		{
			// プレイヤーとの距離
			VECTOR playerPos = player.player_->GetTransform().pos;
			float distance = VSize(VSub(playerPos, objectPos));
			bool hit = (distance < 180.0f);
			if (hit)
			{
				player.isPlayerHitObject_ = true;
				VECTOR pushDir = VSub(objectPos, playerPos);
				pushDir.y = 0.0f; // Y軸(垂直方向)は無視
				pushDir = VNorm(pushDir); // 正規化
			}
		}
	}

	// ButtonProcessによってnewObjectsに追加されたオブジェクトを初期化して登録
	if (!newObjects.empty())
	{
		MakeNewObject(newObjects);
	}
}

const void TutorialScene::ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects)
{
	VECTOR objectPos = obj.GetTransform().pos;

	bool isNearButton = false;

	for (auto& player : players_)
	{
		// プレイヤーとの距離チェック
		VECTOR playerPos = player.player_->GetTransform().pos;
		float distance = VSize(VSub(playerPos, objectPos));
		if (distance < 180.0f)
		{
			isNearButton = true;

		}
	}

	// ボタンの近くにいて、スペースキーが押されたら
	if (isNearButton && InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		obj.SetButtomPushed(true);
		// 直接追加せず、一時リストに格納
		ObjectBase* newObj = new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::AKEG);
		newObjects.push_back(newObj);
	}
}

void TutorialScene::Update(void)
{
	// チュートリアル更新
	tutorial_.Update();

	// チュートリアル全完了ならゲームクリアへ遷移
	if (isEndTutorial_)
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClearScene>());
		return; 
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_P))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
		return;
	}

	// シーン遷移
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
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

		if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
		{
			players_[0].camera_->SetMouseCenter(
				screenWidth_ / 4,
				screenHeight_ / 2);
		}
		else
		{
			players_[1].camera_->SetMouseCenter(
				screenWidth_ * 3 / 4,
				screenHeight_ / 2);
		}
	}

	stageManager_->Update();
	skyDome_->Update();
	for (auto& player : players_)
	{
		player.player_->Update();
		player.camera_->Update();
	}
	//enemyManager_->Update();
	lightPillar_->Update();

	for (auto& wall : walls_)
	{
		wall->Update();
	}

	// 衝突判定チェック(Objectの更新前に実行)
	CheckCollisions();

	// 全オブジェクトの更新
	for (auto* obj : objects_)
	{
		if (obj) obj->Update();
	}

	AnswerChack();
}

void TutorialScene::AnswerChack(void)
{
	// 答えの場所に全てのオブジェクトがあるか判定
	bool isAnswer = true;
	std::vector<ObjectBase*> object;

	for (auto* obj : objects_)
	{
		if (!obj->IsAnswerPosition())
		{
			isAnswer = false;
			object.push_back(obj);
		}
	}

	if (isAnswer && !isPillar_)
	{
		for (auto& obj : object)
		{
			lightPillar_->Init(obj->GetPos());
		}
		isPillar_ = true;
	}

	if (isAnswer && isEndTutorial_)
	{
		endTimer_ += SceneManager::GetInstance()->GetDeltaTime(); 
		//SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClearScene>());
	}

	if(endTimer_ > END_TIME)
	{

	}
}

const void TutorialScene::MakeNewObject(std::vector<ObjectBase*>& newObjects)
{
	for (auto& newObj : newObjects)
	{
		newObj->Init();
		newObj->SetPosition({ 1260.0f, -500.0f, -50.5f });
		newObj->SetScale({ 0.5, 0.5, 0.5 });
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

		objects_.push_back(newObj);
	}
}

void TutorialScene::Draw(void)
{
	int halfWidth = screenWidth_ / 2;
	int mainScreen = SceneManager::GetInstance()->GetMainScreen();

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
		bool isHold = false;

		for (int i = 0; i < objects_.size(); i++)
		{
			auto& obj = objects_[i];

			if (!obj->IsGrabbed()) continue;

			// 持っている
			// 答えの場所に描画
			//DrawSphere3D(ANSWER_VECTOR_LENGTH[i], 80.0f, 16, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);

			isHold = true;

			MV1SetPosition(pinID_, ANSWER_VECTOR_LENGTH[i]);

			// pinIDに何も入っていなかったら
			if(pinID_ == -1)
			{
				pinID_ = MV1DuplicateModel(obj->GetTransform().modelId);
			}

			MV1SetDifColorScale(pinID_, COLOR_F(0.0, 0.5, 1.0, 0.5));
			MV1SetPosition(pinID_, ANSWER_VECTOR_LENGTH[i]);
			MV1SetScale(pinID_, obj->GetTransform().scl);
			MV1DrawModel(pinID_);
		}

		// 何も持っていなかったら & pinIDが-1じゃなかったら
		if (!isHold && pinID_ != -1)
		{
			pinID_ = -1;
		}

		//wall_->Draw();

		// 全オブジェクトを順に描画（それぞれの viewWorld を設定）
		for (auto* obj : objects_)
		{
			if (obj == nullptr) continue;

			if ((bool)CheckCameraViewClip(obj->GetPos()) == true) continue;
			// 視界に入っている

			// オブジェクトの上の矢印を表示
			if (obj->GetObjectType() == ObjectBase::OBJECT_TYPE::BUTTON)
			{
				// ボタン
				DrawNamePlate("ボタン", obj->GetPos());
			}
			if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::BUTTON)
			{
				// オブジェクト
				DrawNamePlate("オブジェクト", obj->GetPos());
			}

			obj->Draw();
			// 右側スクリーンでも同様にマーカー表示
			if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
			{

			}
		}

		if (activePlayer_ == players_[i].player_->GetPlayerNo())
		{
			//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0);
			//// 右側を暗くする
			//pixelRenderer_->Draw();
			//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	// メイン画面に転送
	SetDrawScreen(mainScreen);
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

	// mainScreenをシェーダへ渡す
	//pixelMaterial_->SetTextureBuf(0, mainScreen);

	SetDrawScreen(postEffectScreen_);
	ClearDrawScreen();
	DrawGraph(
		0,
		0,
		mainScreen,
		false);

	

	// 最後に画面へ表示
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
	DrawGraph(
		0,
		0,
		postEffectScreen_,
		false);
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#pragma region デバッグ表示
#ifdef _DEBUG
	// デバッグ表示
	for (int i = 0; i < players_.size(); i++)
	{
		int w = halfWidth * i;
		DrawFormatString(w, 0, GetColor(255, 255, 255), "P%d角度:(%.1f, %.1f, %.1f)",
			i + 1,
			players_[i].player_->GetTransform().quaRot.ToEuler().x,
			players_[i].player_->GetTransform().quaRot.ToEuler().y,
			players_[i].player_->GetTransform().quaRot.ToEuler().z);

		if (players_[i].isPlayerHitObject_)
		{
			DrawFormatString(w, 40, GetColor(255, 0, 0), "P%d: オブジェクトと衝突中!", i + 1);
		}
		else
		{
			DrawFormatString(w, 40, GetColor(0, 255, 0), "P%d: 衝突なし", i + 1);
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

	stageManager_->DrawDebug();
#endif // _DEBUG

	
#pragma endregion

	// チュートリアル描画（最前面）
	tutorial_.Draw();
}

void TutorialScene::DrawNamePlate(std::string str, VECTOR pos)
{
	auto objectPos = ConvWorldPosToScreenPos(pos);

	// 文字列の幅を取得して中央寄せのX座標を計算
	int strWidth = GetDrawStringWidth(str.c_str(), (int)str.length());
	auto drawX = objectPos.x - strWidth / 2;

	DrawFormatString((int)drawX, (int)objectPos.y - 120, 0xffff00, str.c_str());
	DrawFormatString((int)objectPos.x, (int)objectPos.y - 100, 0xffff00, "　↓");
	//DrawCircle(buttonPos.x, buttonPos.y - 100, 10, 0xffffff, true);
}

void TutorialScene::Release(void)
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