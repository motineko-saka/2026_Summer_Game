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
#include "TutorialScene.h"
#include "../UI/Tutorial.h" 

// ワールド座標をカメラ（ビュー）に基づきスクリーン座標へ射影するヘルパー
// vpW/vpH は描画対象のビューポート幅・高さ（ここでは各スクリーンハンドルのサイズ）
// 返り値: true=カメラ前方にあり射影可能 / false=背面（描画しない）
static bool WorldToScreen(const VECTOR& worldPos, const Camera* cam, int vpW, int vpH, float& outX, float& outY)
{
	// カメラ情報取得
	const VECTOR camPos = cam->GetPos();
	const VECTOR camTarget = cam->GetTargetPos();

	// 前方ベクトル
	VECTOR forward = VSub(camTarget, camPos);
	forward = VNorm(forward);

	// カメラの上ベクトル（Quaternion から取得）
	VECTOR up = cam->GetQuaRot().GetUp();

	// 右ベクトル = forward x up
	VECTOR right;
	right.x = forward.y * up.z - forward.z * up.y;
	right.y = forward.z * up.x - forward.x * up.z;
	right.z = forward.x * up.y - forward.y * up.x;
	right = VNorm(right);

	// ワールド空間での対象位置をカメラローカルに変換（内積で座標取得）
	VECTOR dir = VSub(worldPos, camPos);
	float z = forward.x * dir.x + forward.y * dir.y + forward.z * dir.z;

	// カメラの後ろにあるなら描画しない
	if (z <= 0.001f) return false;

	float x = right.x * dir.x + right.y * dir.y + right.z * dir.z;
	float y = up.x * dir.x + up.y * dir.y + up.z * dir.z;

	// 単純な透視投影 (FOV は DxLib デフォルトに合わせておおよそ 60deg)
	const float fovY = 60.0f * (DX_PI_F / 180.0f);
	const float tanHalf = tanf(fovY * 0.5f);
	const float aspect = static_cast<float>(vpW) / static_cast<float>(vpH);

	// 正規化デバイス座標（-1..1）
	float ndcX = (x / z) / (tanHalf * aspect);
	float ndcY = (y / z) / tanHalf;

	// スクリーン座標に変換（左上が (0,0)）
	outX = (ndcX * 0.5f + 0.5f) * vpW;
	outY = (-ndcY * 0.5f + 0.5f) * vpH;

	return true;
}

TutorialScene::TutorialScene(void)
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

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
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

	// オブジェクト作成（複数）
	objects_.reserve(5);

	objects_.push_back(new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::DEFAULT));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });
	objects_.back()->SetPosition({ 1260.0f, -500.0f, -50.5f });
	objects_.back()->SetScale({ 0.5, 0.5, 0.5 });

	objects_.push_back(new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::WBOX));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 1260.0f, -720.0f, -50.5f });
	objects_.back()->SetScale({ 0.5, 0.5, 0.5 });

	objects_.push_back(new ObjectBase(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[2], ObjectBase::OBJECT_TYPE::AKEG));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -1260.0f, -720.0f, -50.5f });
	objects_.back()->SetScale({ 0.2, 0.2, 0.2 });

	objects_.push_back(new ObjectBase(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[3], ObjectBase::OBJECT_TYPE::BUTTON));
	objects_.back()->Init();
	objects_.back()->SetPosition({ 0.0f, 80.0f, -50.0f });
	objects_.back()->SetScale({ 0.5, 0.5, 0.5 });

	objects_.push_back(new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[3], ObjectBase::OBJECT_TYPE::PRESS_BUTTON));
	objects_.back()->Init();
	objects_.back()->SetPosition({ -900.0f, -500.0f, 900.5f });
	objects_.back()->SetScale({ 1.0, 1.0, 1.0 });


	// ステージの各コライダをプレイヤー／カメラ／オブジェクトに登録
	for (const auto& stage : stageManager_->GetStage())
	{
		const ColliderBase* stageCollider =
			stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

		player1_->AddHitCollider(stageCollider);
		player2_->AddHitCollider(stageCollider);

		// カメラに登録
		camera1_->AddHitCollider(stageCollider);
		camera2_->AddHitCollider(stageCollider);

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

		player1_->AddHitCollider(objCaps);
		player2_->AddHitCollider(objCaps);

		for (auto index : pushButtonIndex)
		{
			if (i != index)
			{
				objects_[index]->AddHitCollider(objCaps);
			}
		}
	}

	const ColliderBase* wallCollider =
		wall_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	player1_->AddHitCollider(wallCollider);
	player2_->AddHitCollider(wallCollider);

	const ColliderBase* playerCollider =
		player1_->GetOwnCollider(static_cast<int>(Player::COLLIDER_TYPE::LINE));

	// Buttonだけ
	objects_[4]->AddHitCollider(playerCollider);

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

	// チュートリアル開始（ステップ登録）
	tutorial_.Init();
	tutorial_.ClearSteps();

	// プレイヤー1 の初期位置をキャプチャ
	const VECTOR p1StartPos = player1_->GetTransform().pos;

	// ステップ1: 移動（位置変化で判定）
	tutorial_.AddStep(
		"移動の練習：W/A/S/D または 方向キーでプレイヤーを移動させてください。\n実際に移動すると次へ進みます。",
		[this, p1StartPos]() -> bool {
			VECTOR cur = player1_->GetTransform().pos;
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

	// ステップ3: キャラ切替
	tutorial_.AddStep(
		"キャラクター切替の練習：Tab または 右クリックで操作キャラを切り替えてください。\n切替操作を行うと次へ進みます。",
		[]() -> bool {
			auto const& in = InputManager::GetInstance();
			return in.IsTrgDown(KEY_INPUT_TAB) || in.IsTrgMouseRight();
		}
	);

	// ステップ4: オブジェクト操作（オブジェクトに近づいて E）
	tutorial_.AddStep(
		"オブジェクト操作の練習1：オブジェクトに近づいて E を押してください。\nオブジェクトを押すと次へ進みます。",
		[this]() -> bool {
			for (auto* obj : objects_)
			{
				if (obj == nullptr) continue;
				VECTOR objPos = obj->GetTransform().pos;
				VECTOR p1Pos = player1_->GetTransform().pos;
				float dist1 = VSize(VSub(p1Pos, objPos));
				if (dist1 < 180.0f && InputManager::GetInstance().IsTrgDown(KEY_INPUT_E)) return true;
				VECTOR p2Pos = player2_->GetTransform().pos;
				float dist2 = VSize(VSub(p2Pos, objPos));
				if (dist2 < 180.0f && InputManager::GetInstance().IsTrgDown(KEY_INPUT_E)) return true;
			}
			return false;
		}
	);

	// ステップ5: オブジェクトの設置(正しい位置への設置)
	tutorial_.AddStep(
		"オブジェクト操作の練習2 : オブジェクトを正しい位置に配置してください。\nオブジェクトを配置すると次に進む。",
		[this]() -> bool {
			for (auto* obj : objects_)
			{
				if (obj == nullptr) continue;
				if (obj->IsAnswerPosition()) return true;
			}
			return false;
		}
	);

	// ステップ4: ボタン操作（ボタン近くで Space）
	ObjectBase* buttonObj = nullptr;
	for (auto* o : objects_)
	{
		if (o && o->GetType() == ObjectBase::OBJECT_TYPE::BUTTON) { buttonObj = o; break; }
	}
	tutorial_.AddStep(
		"ボタン操作の練習：ボタンの近くで Space を押してください。\nボタンを押すと次へ進みます。",
		[this]() -> bool {
			if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
			{
				for (auto* obj : objects_)
				{
					if (obj == nullptr) continue;

					if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
					{
						VECTOR objectPos = obj->GetTransform().pos;

						VECTOR player1Pos = player1_->GetTransform().pos;
						float distance1 = VSize(VSub(player1Pos, objectPos));
						if (distance1 < 180.0f)
						{
							obj->SetButtomPushed(true);
						}

						VECTOR player2Pos = player2_->GetTransform().pos;
						float distance2 = VSize(VSub(player2Pos, objectPos));
						if (distance2 < 180.0f)
						{
							obj->SetButtomPushed(true);
						}
					}
				}
			}
			return false;
		}
	);

	// 最終ステップ: 確認して終了
	tutorial_.AddStep(
		"チュートリアル完了：Z / Enter / Space でチュートリアルを終了します。",
		[]() -> bool {
			return CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE);
		}
	);

	// 開始
	tutorial_.Start();
}

void TutorialScene::CheckCollisions(void)
{
	// 各オブジェクトに対してプレイヤーとの距離判定を行う
	isPlayer1HitObject_ = false;
	isPlayer2HitObject_ = false;

	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;

		VECTOR objectPos = obj->GetTransform().pos;

		// ボタンタイプの場合は専用処理
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{
			bool isNearButton = false;

			VECTOR player1Pos = player1_->GetTransform().pos;
			float distance1 = VSize(VSub(player1Pos, objectPos));
			if (distance1 < 180.0f)
			{
				isNearButton = true;
			}

			VECTOR player2Pos = player2_->GetTransform().pos;
			float distance2 = VSize(VSub(player2Pos, objectPos));
			if (distance2 < 180.0f)
			{
				isNearButton = true;
			}

			if (isNearButton && InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
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
		}

		// プレイヤー2との距離
		VECTOR player2Pos = player2_->GetTransform().pos;
		float distance2 = VSize(VSub(player2Pos, objectPos));
		bool hit2 = (distance2 < 180.0f);
		if (hit2)
		{
			isPlayer2HitObject_ = true;
		}
	}

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		for (auto* obj : objects_)
		{
			if (obj == nullptr) continue;

			if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
			{
				VECTOR objectPos = obj->GetTransform().pos;

				VECTOR player1Pos = player1_->GetTransform().pos;
				float distance1 = VSize(VSub(player1Pos, objectPos));
				if (distance1 < 180.0f)
				{
					obj->SetButtomPushed(true);
				}

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

void TutorialScene::Update(void)
{
	// チュートリアル更新
	tutorial_.Update();

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

void TutorialScene::DrawPlayer1Screen(void)
{
	// プレイヤー1用のカメラ設定
	camera1_->SetBeforeDraw();

	// 3D描画
	stageManager_->Draw();
	skyDome_->Draw();
	player1_->Draw();
	player2_->Draw(); // プレイヤー2も描画

	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;
		obj->Draw();
		// ボタンオブジェクトがどれかわかるようにオブジェクトの上に↓を描画してわかるようにする
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{

		}
	}
}

void TutorialScene::DrawPlayer2Screen(void)
{
	// プレイヤー2用のカメラ設定
	camera2_->SetBeforeDraw();

	// 3D描画
	stageManager_->Draw();
	skyDome_->Draw();
	player1_->Draw();
	player2_->Draw();

	for (auto* obj : objects_)
	{
		if (obj == nullptr) continue;
		obj->Draw();
		// 右側スクリーンでも同様にマーカー表示
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{

		}
	}
}

void TutorialScene::Draw(void)
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

	// BUTTONの上の位置にマーカーを描画



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

	// チュートリアル描画（最前面）
	tutorial_.Draw();
}

void TutorialScene::Release(void)
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