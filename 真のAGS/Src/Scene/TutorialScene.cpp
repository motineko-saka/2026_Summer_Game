#include <DxLib.h>
#include <algorithm>
#include <memory>
#include <vector>
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
#include "GameScene.h"
#include "PauseScene.h"
#include "TitleScene.h"
#include "../Manager/EffekseerEffect.h"
#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"
#include "../Audio/AudioManager.h"

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
	// Release();
}

void TutorialScene::Init(void)
{
	// 画面サイズの取得
	GetScreenState(&screenWidth_, &screenHeight_, nullptr);

	// 分割画面用スクリーン
	const int halfWidth = screenWidth_ / 2;
	screenHandle1_ = MakeScreen(halfWidth, screenHeight_, true);
	screenHandle2_ = MakeScreen(halfWidth, screenHeight_, true);

	lightPillar_ = std::make_unique<LightPillar>();

	players_.resize(2);

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

		players_[i].isPlayerHitObject_ = false;
	}

	players_[0].camera_->SetMouseCenter(screenWidth_ / 4, screenHeight_ / 2);
	players_[1].camera_->SetMouseCenter(screenWidth_ * 3 / 4, screenHeight_ / 2);
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

	// オブジェクト作成
	objects_.reserve(3);

	auto pushObject = [this](SceneBase::WORLD w, const VECTOR& ans, ObjectBase::OBJECT_TYPE type, const VECTOR& pos, const VECTOR& scl, bool placed = false) {
		ObjectBase* o = new ObjectBase(w, ans, type);
		o->Init();
		o->SetPosition(pos);
		o->SetScale(scl);
		if (placed) o->SetPlaced(true);
		objects_.push_back(o);
		};

	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::BUTTON, { -500.0f, -500.0f, 500.0f }, { 0.5f, 0.5f, 0.5f }, true);
	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::CHEST, { 900.0f, -520.0f, 100.0f }, { 0.8f, 0.8f, 0.8f }, true);
	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::AKEG, { 900.0f, -520.0f, 100.0f }, { 0.3f, 0.3f, 0.3f }, false);

	CreateWall(*stageManager_);

	// ステージのコライダをプレイヤー／カメラ／オブジェクトに登録
	for (const auto& stage : stageManager_->GetStage())
	{
		const ColliderBase* stageCollider = stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));
		for (auto& player : players_)
		{
			player.player_->AddHitCollider(stageCollider);
			player.camera_->AddHitCollider(stageCollider);
		}
		for (auto& obj : objects_)
		{
			obj->AddHitCollider(stageCollider);
		}
		//if (stageCollider == nullptr) DrawFormatString(100, 100, 0xffffff, "stageCollider is null\n");
	}

	// 踏むタイプのボタンのインデックスを収集
	std::vector<size_t> pushButtonIndex;
	for (size_t i = 0; i < objects_.size(); ++i)
	{
		if (objects_[i]->GetObjectType() == ObjectBase::OBJECT_TYPE::PRESS_BUTTON)
		{
			pushButtonIndex.push_back(i);
		}
	}

	// 各オブジェクトの衝突コライダをプレイヤーに登録（重複チェック内包）
	for (size_t i = 0; i < objects_.size(); ++i)
	{
		auto* obj = objects_[i];
		const auto* objCaps = obj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
		if (!objCaps) continue;

		for (size_t p = 0; p < players_.size(); ++p)
		{
			players_[p].player_->AddHitCollider(objCaps);
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

	// ポストエフェクト用スクリーン
	postEffectScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

	// ポストエフェクト用マテリアル／レンダラ
	pixelMaterial_ = std::make_unique<PixelMaterial>("Monotone.cso", 1);
	pixelMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });
	pixelMaterial_->AddTextureBuf(SceneManager::GetInstance()->GetMainScreen());
	pixelRenderer_ = std::make_unique<PixelRenderer>(*pixelMaterial_);
	pixelRenderer_->MakeSquereVertex(Vector2(0, 0), Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y));

	// プレイヤーのラインコライダをButtonに登録
	for (auto& player : players_)
	{
		const ColliderBase* playerCollider = player.player_->GetOwnCollider(static_cast<int>(Player::COLLIDER_TYPE::LINE));
		for (auto index : pushButtonIndex)
		{
			objects_[index]->AddHitCollider(playerCollider);
		}
	}

	// 初期アクティブ状態（プレイヤー1）
	activePlayer_ = Player::PLAYER_NO::PLAYER1;
	for (size_t i = 0; i < players_.size(); ++i)
	{
		const bool isActive = (i == 0);
		players_[i].player_->SetActive(isActive);
		players_[i].camera_->SetControlEnabled(isActive);
	}

	tempCameraRot_ = { 0,0,0 };
	score_ = 0;

	TutorialInit();
}

void TutorialScene::TutorialInit(void)
{
	// チュートリアル開始（ステップ登録）
	tutorial_.Init();
	tutorial_.ClearSteps();

	TyutorialTEXT();

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
	for (auto& player : players_) player.isPlayerHitObject_ = false;

	std::vector<ObjectBase*> newObjects;  // 新規オブジェクト用
	std::vector<int> removeIndices;       // 削除インデックス

	for (auto& obj : objects_)
	{
		if (!obj) continue;
		const VECTOR objectPos = obj->GetTransform().pos;

		// ボタンは専用処理へ委譲
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{
			ButtonProcess(*obj, newObjects, removeIndices);
			continue;
		}

		for (auto& player : players_)
		{
			const VECTOR playerPos = player.player_->GetTransform().pos;
			const float distance = VSize(VSub(playerPos, objectPos));
			if (distance < 180.0f)
			{
				player.isPlayerHitObject_ = true;
				VECTOR pushDir = VSub(objectPos, playerPos);
				pushDir.y = 0.0f;
				pushDir = VNorm(pushDir);
				(void)pushDir; // 現状は計算するのみ（既存コードの挙動を保持）
			}
		}
	}

	if (!removeIndices.empty())
	{
		// 降順ソートして重複を除き、削除処理
		std::sort(removeIndices.begin(), removeIndices.end(), std::greater<int>());
		removeIndices.erase(std::unique(removeIndices.begin(), removeIndices.end()), removeIndices.end());

		for (int idx : removeIndices)
		{
			if (idx >= 0 && idx < static_cast<int>(objects_.size()))
			{
				if (objects_[idx])
				{
					// 重要: 削除前にそのオブジェクトが持つ全コライダを
					// 他の Actor から登録解除してダングリングを防ぐ
					const auto& ownCols = objects_[idx]->GetOwnColliders();
					for (const auto& ct : ownCols)
					{
						const ColliderBase* col = ct.second;
						if (!col) continue;

						// プレイヤーから解除
						for (auto& p : players_)
						{
							if (p.player_) p.player_->RemoveHitCollider(col);
						}

						// 他のオブジェクトから解除
						for (auto& otherObj : objects_)
						{
							if (!otherObj || otherObj == objects_[idx]) continue;
							otherObj->RemoveHitCollider(col);
						}
					}

					objects_[idx]->Release();
					delete objects_[idx];
				}
				objects_.erase(objects_.begin() + idx);
			}
		}
	}
	if (!newObjects.empty()) MakeNewObject(newObjects);
}

const void TutorialScene::ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects, std::vector<int>& removeIndices)
{
	const VECTOR objectPos = obj.GetTransform().pos;

	bool isNearButton = false;
	for (auto& player : players_)
	{
		const float distance = VSize(VSub(player.player_->GetTransform().pos, objectPos));
		if (distance < 180.0f) { isNearButton = true; break; }
	}

	// 近くでスペースならチェストを出現させ、既存チェストを消す
	if (isNearButton && (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_F) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT)))
	{
		obj.SetButtomPushed(true);
		newObjects.push_back(new ObjectBase(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::OPENCHEST));

		for (size_t i = 0; i < objects_.size(); ++i)
		{
			if (objects_[i] && objects_[i]->GetObjectType() == ObjectBase::OBJECT_TYPE::CHEST)
			{
				removeIndices.push_back(static_cast<int>(i));
				break;
			}
		}
	}
}

void TutorialScene::Update(void)
{
	// チュートリアル更新
	tutorial_.Update();

	// チュートリアル完了でクリアへ遷移
	if (isEndTutorial_)
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_P))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
		return;
	}

	// ポーズ
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::OPTION))
	{
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
	}

	// プレイヤー切替（TAB)
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_TAB) || InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER))
	{
		for (size_t i = 0; i < players_.size(); ++i)
		{
			const bool enable = (activePlayer_ != static_cast<Player::PLAYER_NO>(i));
			players_[i].player_->SetActive(enable);
			players_[i].camera_->SetControlEnabled(enable);
		}
		activePlayer_ = (activePlayer_ == Player::PLAYER_NO::PLAYER1) ? Player::PLAYER_NO::PLAYER2 : Player::PLAYER_NO::PLAYER1;

		if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
			players_[0].camera_->SetMouseCenter(screenWidth_ / 4, screenHeight_ / 2);
		else
			players_[1].camera_->SetMouseCenter(screenWidth_ * 3 / 4, screenHeight_ / 2);
	}

	stageManager_->Update();
	skyDome_->Update();

	for (auto& player : players_)
	{
		player.player_->Update();
		player.camera_->Update();
	}

	lightPillar_->Update();
	for (auto& wall : walls_) wall->Update();

	// 衝突判定（Object更新前）
	CheckCollisions();

	// 全オブジェクトの更新
	for (auto& obj : objects_)
	{
		if (obj) obj->Update();
	}

	AnswerChack();
}

void TutorialScene::AnswerChack(void)
{
	// 全オブジェクトが答えの場所にあるか判定
	bool isAnswer = true;
	std::vector<ObjectBase*> notPlaced;

	for (auto& obj : objects_)
	{
		if (!obj->IsAnswerPosition())
		{
			isAnswer = false;
			notPlaced.push_back(obj);
		}
	}

	if (isAnswer && !isPillar_)
	{
		for (auto& obj : notPlaced) lightPillar_->Init(obj->GetPos());
		isPillar_ = true;
	}

	if (isAnswer && isEndTutorial_)
	{
		endTimer_ += SceneManager::GetInstance()->GetDeltaTime();
		// SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClearScene>());
	}

	if (endTimer_ > END_TIME)
	{
		// END 動作があればここへ
	}
}

const void TutorialScene::MakeNewObject(std::vector<ObjectBase*>& newObjects)
{
	for (auto& newObj : newObjects)
	{
		newObj->Init();
		newObj->SetPosition({ 900.0f, -520.0f, 100.0f });
		newObj->SetScale({ 0.8f, 0.8f, 0.8f });
		newObj->SetPlaced(true);
		for (const auto& stage : stageManager_->GetStage())
		{
			const ColliderBase* stageCollider = stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));
			newObj->AddHitCollider(stageCollider);
		}

		const ColliderBase* objCaps = newObj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
		if (!objCaps) return;

		for (auto& player : players_) player.player_->AddHitCollider(objCaps);

		objects_.push_back(newObj);
	}
}

void TutorialScene::Draw(void)
{
	const int halfWidth = screenWidth_ / 2;
	const int mainScreen = SceneManager::GetInstance()->GetMainScreen();

	for (size_t i = 0; i < players_.size(); ++i)
	{
		const int screenHandle = (i == 0) ? screenHandle1_ : screenHandle2_;
		SetDrawScreen(screenHandle);
		ClearDrawScreen();

		players_[i].camera_->SetBeforeDraw();

		// 3D描画
		skyDome_->Draw();
		stageManager_->Draw();
		lightPillar_->Draw();

		for (auto& p : players_) p.player_->Draw();

		// 答えのプレビュー表示
		bool isHold = false;
		for (size_t j = 0; j < objects_.size(); ++j)
		{
			auto* obj = objects_[j];
			if (!obj || !obj->IsGrabbed()) continue;

			isHold = true;
			if (pinID_ == -1) pinID_ = MV1DuplicateModel(obj->GetTransform().modelId);

			MV1SetDifColorScale(pinID_, COLOR_F(0.0, 0.5, 1.0, 0.5));
			MV1SetPosition(pinID_, ANSWER_VECTOR_LENGTH[j]);
			MV1SetScale(pinID_, obj->GetTransform().scl);
			MV1DrawModel(pinID_);
		}

		if (!isHold && pinID_ != -1) pinID_ = -1;

		//wall_->Draw();

		// 全オブジェクトを順に描画（それぞれの viewWorld を設定）
		for (auto& obj : objects_)
		{
			if (!obj) continue;
			if (CheckCameraViewClip(obj->GetPos())) continue;

			if (obj->GetObjectType() == ObjectBase::OBJECT_TYPE::BUTTON)
				DrawNamePlate("ボタン", obj->GetPos());
			else
				DrawNamePlate("オブジェクト", obj->GetPos());

			obj->Draw();
		}

		// アクティブプレイヤー関連のポスト処理（コメント化されていた処理を維持）
		if (activePlayer_ == players_[i].player_->GetPlayerNo())
		{
			// ここに任意の描画（未使用）
		}
	}

	// メイン画面に転送
	SetDrawScreen(mainScreen);
	ClearDrawScreen();

	DrawExtendGraph(0, 0, halfWidth, screenHeight_, screenHandle1_, true);
	DrawExtendGraph(halfWidth, 0, screenWidth_, screenHeight_, screenHandle2_, true);

	// 非アクティブ側を薄暗く
	const int dimAlpha = 150;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, dimAlpha);
	if (activePlayer_ == Player::PLAYER_NO::PLAYER1)
		DrawBox(halfWidth, 0, screenWidth_, screenHeight_, GetColor(0, 0, 0), TRUE);
	else
		DrawBox(0, 0, halfWidth, screenHeight_, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ポストエフェクト転送
	SetDrawScreen(postEffectScreen_);
	ClearDrawScreen();
	DrawGraph(0, 0, mainScreen, FALSE);

	// 最終表示
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawGraph(0, 0, postEffectScreen_, FALSE);

#ifdef _DEBUG
	// デバッグ表示
	for (size_t i = 0; i < players_.size(); ++i)
	{
		const int w = halfWidth * static_cast<int>(i);
		DrawFormatString(w, 0, GetColor(255, 255, 255), "P%d角度:(%.1f, %.1f, %.1f)",
			static_cast<int>(i + 1),
			players_[i].player_->GetTransform().quaRot.ToEuler().x,
			players_[i].player_->GetTransform().quaRot.ToEuler().y,
			players_[i].player_->GetTransform().quaRot.ToEuler().z);

		if (players_[i].isPlayerHitObject_)
			DrawFormatString(w, 40, GetColor(255, 0, 0), "P%d: オブジェクトと衝突中!", static_cast<int>(i + 1));
		else
			DrawFormatString(w, 40, GetColor(0, 255, 0), "P%d: 衝突なし", static_cast<int>(i + 1));
	}

	if (!objects_.empty() && objects_[0])
	{
		DrawFormatString(halfWidth, 80, GetColor(0, 0, 0), "座標:(%.1f, %.1f, %.1f)",
			objects_[0]->GetTransform().pos.x,
			objects_[0]->GetTransform().pos.y,
			objects_[0]->GetTransform().pos.z);
	}

	int y = 120;
	for (auto& object : objects_)
	{
		DrawFormatString(halfWidth, y, GetColor(255, 255, 255),
			"Object情報:座標(%.1f, %.1f, %.1f) 回転(%.1f, %.1f, %.1f)\nViewWorld : %d isAnswer : %d",
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

	// チュートリアルUI表示
	tutorial_.Draw();
}

void TutorialScene::DrawNamePlate(std::string str, VECTOR pos)
{
	const auto objectPos = ConvWorldPosToScreenPos(pos);
	const int strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
	const int drawX = static_cast<int>(objectPos.x) - (strWidth / 2);

	DrawFormatString(drawX, static_cast<int>(objectPos.y) - 120, 0xffff00, str.c_str());
	DrawFormatString(static_cast<int>(objectPos.x), static_cast<int>(objectPos.y) - 100, 0xffff00, "　↓");
}

void TutorialScene::Release(void)
{
	// 全オブジェクト解放
	for (auto& obj : objects_)
	{
		if (obj) { obj->Release(); delete obj; }
	}
	objects_.clear();

	players_.clear();

	if (screenHandle1_ != -1) DeleteGraph(screenHandle1_);
	if (screenHandle2_ != -1) DeleteGraph(screenHandle2_);
}

void TutorialScene::TyutorialTEXT(void)
{
	// プレイヤー
	const VECTOR p1StartPos = players_[0].player_->GetTransform().pos;

	// ステップ1: 移動
	tutorial_.AddStep(
		"やあ！まずは移動してみよう！\nW/A/S/Dキー または パッドの左スティックで歩けるよ！\n少し動いたら次に進もう！",
		[this, p1StartPos]() -> bool {
			const VECTOR cur = players_[0].player_->GetTransform().pos;
			const float moved = VSize(VSub(cur, p1StartPos));
			return moved > moveStepe_;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU1).handleId_
	);

	// ステップ2: 視点操作
	tutorial_.AddStep(
		"いいね！次は周りを見渡してみよう！\nマウス、矢印キー、または パッドの右スティックで視点を動かせるよ！\n視点を動かしたら次へ進もう！",
		[this]() -> bool {
			for (size_t i = 0; i < players_.size(); ++i)
			{
				if (tempCameraRot_.x != players_[i].camera_->GetAngles().x ||
					tempCameraRot_.y != players_[i].camera_->GetAngles().y ||
					tempCameraRot_.z != players_[i].camera_->GetAngles().z)
				{
					score_ += 0.1f;
				}
			}

			tempCameraRot_ = players_[0].camera_->GetAngles();

			return score_ >= 30.0f;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU2).handleId_
	);

	// ステップ3: キャラ切替
	tutorial_.AddStep(
		"ここまで順調だね！次はプレイヤーを切り替えてみよう！\nTabキー または RTでプレイヤー2に切り替えられるよ！。\n切り替えられたら次へ進もう！",
		[]() -> bool {
			return InputManager::GetInstance()->IsTrgDown(KEY_INPUT_TAB) 
				|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER);
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU3).handleId_
	);

	tutorial_.AddStep(
		"あれ？宝箱があるけど、この世界では開けられないみたい…。\n開ける方法は向こう側の世界にあるのかも！\nもう一度プレイヤーを切り替えて確かめてみよう！",
		[]() -> bool {
			return InputManager::GetInstance()->IsTrgDown(KEY_INPUT_TAB) 
				|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER);
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU4).handleId_
	);

	// ステップ4: ボタン操作
	tutorial_.AddStep(
		"あっ！ボタンを見つけたね！\n押したら何か変わるかもしれないよ。\n近づいて Spaceキー または パッドのBボタンで押してみよう！", [this]() -> bool {
			for (auto& obj : objects_)
			{
				if (!obj) continue;
				if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON && obj->isPushButtom()) return true;
			}
			return false;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU5).handleId_
	);

	// ステップ5: オブジェクトの操作
	tutorial_.AddStep(
		"やった！宝箱が開いたよ！\n中に樽を見つけたね。\n近づいて EキーまたはXで持ち上げてみよう！",
		[this]() -> bool {
			for (auto& obj : objects_)
			{
				if (!obj) continue;
				for (auto& player : players_)
				{
					const float dist = VSize(VSub(player.player_->GetTransform().pos, obj->GetTransform().pos));
					// 距離判定とキー入力を正しくグループ化して評価する
					if (dist < 180.0f &&
						(InputManager::GetInstance()->IsTrgDown(KEY_INPUT_E)
							|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT)))
					{
						return true;
					}
				}
			}
			return false;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU6).handleId_
	);

	// ステップ6: オブジェクトの設置
	tutorial_.AddStep(
		"いい感じ！樽を指定された場所に置いてみよう（EキーまたはXで置けるよ）！\n運んで配置できたら成功だよ！",
		[this]() -> bool {
			for (auto& obj : objects_)
			{
				if (!obj) continue;
				if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::AKEG) continue;
				if (obj->IsAnswerPosition()) 
				
				// 効果音
				AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);

				return true;
			}
			return false;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU7).handleId_
	);

	// 最終ステップ: 確認して終了
	tutorial_.AddStep(
		"お疲れさま！これでチュートリアルは完了だよ！\n操作はもうバッチリ！あとは実際に遊びながら慣れていこう！\nZキー / Enterキー / Spaceキー で冒険を始めよう！",
		[this]() -> bool {
			// ここではトリガー判定
			if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_Z)
				|| InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RETURN)
				|| InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE)
				|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
			{
				isEndTutorial_ = true;
				return true;
			}
			return false;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU8).handleId_
	);
}
