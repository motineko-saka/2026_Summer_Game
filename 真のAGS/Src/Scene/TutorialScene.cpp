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
#include "SceneTransition.h"
#include "../Manager/EffekseerEffect.h"
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
	sceneTransition_ = std::make_unique<SceneTransition>();
}

TutorialScene::~TutorialScene(void)
{
	Release();
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
		players_[i].camera_->Update();

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

	CreateWall(*stageManager_);

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

	// ボタンを左右両方に配置
	pushObject(SceneBase::WORLD::LEFT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::BUTTON, { -700.0f, -520.0f, 100.0f }, { 0.5f, 0.5f, 0.5f }, true);
	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[0], ObjectBase::OBJECT_TYPE::BUTTON, { 900.0f, -520.0f, 100.0f }, { 0.5f, 0.5f, 0.5f }, true);
	buttonPressHistory_.clear();

	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::AKEG, { 900.0f, -520.0f, 300.0f }, { 0.3f, 0.3f, 0.3f }, true);
	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[1], ObjectBase::OBJECT_TYPE::CHEST, { 900.0f, -520.0f, 300.0f }, { 0.6f, 0.6f, 0.6f }, true);
	pushObject(SceneBase::WORLD::RIGHT, ANSWER_VECTOR_LENGTH[2], ObjectBase::OBJECT_TYPE::WBOX, { 800.0f, -520.0f, 100.0f }, { 0.5f, 0.5f, 0.5f }, true);


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
	}

	// 各オブジェクトの衝突コライダをプレイヤーに登録
	for (size_t i = 0; i < objects_.size(); ++i)
	{
		auto* obj = objects_[i];
		const auto* objCaps = obj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
		if (!objCaps) continue;

		for (size_t p = 0; p < players_.size(); ++p)
		{
			players_[p].player_->AddHitCollider(objCaps);
		}
	}

	// プレイヤーのラインコライダをButtonに登録
	for (auto& player : players_)
	{
		const ColliderBase* playerCollider = player.player_->GetOwnCollider(static_cast<int>(Player::COLLIDER_TYPE::LINE));
		(void)playerCollider;
	}

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

	// オーディオマネージャーはアプリ側で管理している想定
	// シーンのサウンドを読み込み、BGM を再生
	if (auto am = AudioManager::GetInstance())
	{
		am->LoadSceneSound(LoadScene::GAME_TUTORIAL);
		am->LoadSceneSound(LoadScene::GAME);
		am->PlayBGM(SoundID::BGM_TUTORIAL);
	}
	// デバック
	ResourceManager::GetInstance().DebugDump();
	OutputDebugStringA("TutorialScene::Init complete\n");
}

void TutorialScene::TutorialInit(void)
{
	// チュートリアル開始
	tutorial_.Init();
	tutorial_.ClearSteps();

	TyutorialTEXT();

	// 開始
	tutorial_.Start();
}

void TutorialScene::Load(void)
{
	auto& rm = ResourceManager::GetInstance();
	rm.Load(ResourceManager::SRC::ENOGU1);
	rm.Load(ResourceManager::SRC::ENOGU2);
	rm.Load(ResourceManager::SRC::ENOGU3);
	rm.Load(ResourceManager::SRC::ENOGU4);
	rm.Load(ResourceManager::SRC::ENOGU5);
	rm.Load(ResourceManager::SRC::ENOGU6);
	rm.Load(ResourceManager::SRC::ENOGU7);
	rm.Load(ResourceManager::SRC::ENOGU8);
	rm.Load(ResourceManager::SRC::HINTO);

	rm.Load(ResourceManager::SRC::BARREL);
	rm.Load(ResourceManager::SRC::Chest);
	rm.Load(ResourceManager::SRC::OPENCHEST);
	rm.Load(ResourceManager::SRC::BUTTON);
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

	// オブジェクト走査をインデックスベースに変更
	for (size_t i = 0; i < objects_.size(); ++i)
	{
		auto* obj = objects_[i];
		if (!obj) continue;
		const VECTOR objectPos = obj->GetTransform().pos;

		// ボタンは専用処理へ委譲
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::BUTTON)
		{
			ButtonProcess(*obj, newObjects, removeIndices);
			continue;
		}

		// OPENCHESTを生成する処理
		if (obj->GetType() == ObjectBase::OBJECT_TYPE::CHEST)
		{
			// ボタンの正解が成立していなければチェストは開けない
			if (!butcount_) continue;

			const bool isE = InputManager::GetInstance()->IsTrgDown(KEY_INPUT_E);
			const bool isPadLeft = InputManager::GetInstance()->IsPadBtnTrgDown(
				InputManager::JOYPAD_NO::PAD1,
				InputManager::JOYPAD_BTN::LEFT);

			// 入力が無ければ次へ
			if (!isE && !isPadLeft) continue;

			// どちらかのプレイヤーが近いか確認
			bool isNearPlayer = false;
			for (auto& p : players_)
			{
				const float dist = VSize(VSub(p.player_->GetTransform().pos, objectPos));
				if (dist < INTERACT_DISTANCE)
				{
					isNearPlayer = true;
					break;
				}
			}
			if (!isNearPlayer) continue;

			// OPENCHESTを生成
			newObjects.push_back(new ObjectBase(
				SceneBase::WORLD::LEFT,
				ANSWER_VECTOR_LENGTH[1],
				ObjectBase::OBJECT_TYPE::OPENCHEST));

			// エフェクト
			const VECTOR effectPos = { 900.0f, -520.0f, 300.0f };
			if (EffekseerEffect::GetInstance())
			{
				EffekseerEffect::GetInstance()->PlayTutorialEffect(effectPos, 0.0f);
			}

			// AKEG を再度操作可能にする
			for (auto* ao : objects_)
			{
				if (ao && ao->GetObjectType() == ObjectBase::OBJECT_TYPE::AKEG)
				{
					ao->SetPlaced(false);
					break;
				}
			}

			// CHESTを削除対象
			removeIndices.push_back(static_cast<int>(i));

			// 生成後はフラグをリセット
			butcount_ = false;

			// ボタン関連の進行リセット
			buttonSP_ = 0;
			buttonPCount_ = 0;

			if (auto am = AudioManager::GetInstance())
			{
				am->PlaySE(SoundID::SE_SUCCESS);
			}
			TbutonCount_ = true;
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
				// オブジェクト
				if (objects_[idx])
				{
					const auto& ownCols = objects_[idx]->GetOwnColliders();
					for (const auto& ct : ownCols)
					{
						const ColliderBase* col = ct.second.get();
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
					// オブジェクト自身を delete（メモリ解放）
					delete objects_[idx];
				}

				// vector から削除
				objects_.erase(objects_.begin() + idx);
			}
		}
	}
	if (!newObjects.empty()) MakeNewObject(newObjects);
}

const void TutorialScene::ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects, std::vector<int>& removeIndices)
{
	const VECTOR objectPos = obj.GetTransform().pos;

	// キー／パッド入力を判定（パッドは単一：PAD1 を想定）
	const bool isKeyPush = InputManager::GetInstance()->IsTrgDown(KEY_INPUT_F);
	const bool isPadPush = InputManager::GetInstance()->IsPadBtnTrgDown(
		InputManager::JOYPAD_NO::PAD1,
		InputManager::JOYPAD_BTN::RIGHT);

	// 入力がなければ終了
	if (!isKeyPush && !isPadPush)
	{
		return;
	}

	// 押したプレイヤーは「現在アクティブなプレイヤー」を基準にする
	int playerNo = (activePlayer_ == Player::PLAYER_NO::PLAYER1) ? 0 : 1;

	// ボタンとの距離チェック（アクティブプレイヤー基準）
	const float distance = VSize(VSub(
		players_[playerNo].player_->GetTransform().pos,
		objectPos));

	if (distance >= 180.0f)
	{
		return;
	}

	// ボタンを押した
	obj.SetButtomPushed(true);

	SceneBase::WORLD pressed = obj.GetWorld();

	if (buttonPTarget_ <= 0 || buttonPTarget_ != static_cast<int>(buttonRequiredPattern_.size()))
		buttonPTarget_ = static_cast<int>(buttonRequiredPattern_.size());

	if (buttonSP_ < buttonRequiredPattern_.size() && pressed == buttonRequiredPattern_[buttonSP_])
	{
		buttonSP_++;
		if (buttonSP_ == buttonPTarget_)
		{
			// 正解を記録
			buttonSP_ = 0;
			buttonPCount_ = 0;

			// 正解フラグ
			butcount_ = true;

			// 成功音のみ再生
			if (auto am = AudioManager::GetInstance())
			{
				am->PlaySE(SoundID::SE_SUCCESS);
			}
		}
	}
	else
	{
		buttonSP_ = 0;
		buttonPCount_++;
	}
}

void TutorialScene::Update(void)
{
	// チュートリアル更新
	tutorial_.Update();

	// Effekseer更新
	if (EffekseerEffect::GetInstance())
	{
		EffekseerEffect::GetInstance()->Update();
	}

	Hint();

	// ヒント表示中は更新を停止
	if (showHint_)
	{
		// 更新処理をスキップ
		return;
	}

	// チュートリアル完了でクリアへ遷移
	if (isEndTutorial_)
	{
		isEndTutorial_ = false;

		// 先にオーディオを停止／削除してからシーン切替
		if (auto am = AudioManager::GetInstance())
		{
			am->StopBGM();
			am->DeleteSceneSound(LoadScene::GAME_TUTORIAL);
		}
		SceneManager::GetInstance()->ChangeSceneTransition(
			std::make_shared<GameScene>());

		return;
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_P))
	{
		if (auto am = AudioManager::GetInstance())
		{
			am->StopBGM();
			am->DeleteSceneSound(LoadScene::GAME_TUTORIAL);
		}

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
	//for (auto& wall : walls_) wall->Update();

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
	}
}

const void TutorialScene::MakeNewObject(std::vector<ObjectBase*>& newObjects)
{
	for (auto& newObj : newObjects)
	{
		if (!newObj) continue;

		newObj->Init();
		newObj->SetPosition({ 900.0f, -520.0f, 300.0f });
		newObj->SetScale({ 0.6f, 0.6f, 0.6f });
		newObj->SetPlaced(true);

		// ステージコライダを追加
		for (const auto& stage : stageManager_->GetStage())
		{
			const ColliderBase* stageCollider = stage->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));
			newObj->AddHitCollider(stageCollider);
		}

		const ColliderBase* objCaps = newObj->GetOwnCollider(static_cast<int>(ObjectBase::COLLIDER_TYPE::CAPSULE));
		if (!objCaps)
		{
			// コライダが無ければこのオブジェクトは使えないため確実に破棄して続行（リーク防止）
			delete newObj;
			continue;
		}

		// プレイヤーにコライダ登録
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

		if (!isHold && pinID_ != -1)
		{
			MV1DeleteModel(pinID_);
			pinID_ = -1;
		}
		for (auto& wall : walls_)
		{
			wall->Draw();
		}

		// 全オブジェクトを順に描画
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

		// インタラクト文字表示
		for (auto& obj : objects_)
		{
			if (!obj) continue;
			// 既に掴んでいる物は表示しない
			if (obj->IsGrabbed()) continue;
			if (CheckCameraViewClip(obj->GetPos())) continue;

			// 対象とするオブジェクト種類
			const auto t = obj->GetObjectType();
			if (t != ObjectBase::OBJECT_TYPE::AKEG &&
				t != ObjectBase::OBJECT_TYPE::CHEST &&
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
			case ObjectBase::OBJECT_TYPE::AKEG:
				label = "Eで持つ";
				break;
			case ObjectBase::OBJECT_TYPE::BUTTON:
				label = "Fで押す";
				break;
			case ObjectBase::OBJECT_TYPE::CHEST:
				label = "Eで開ける";
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

		if (EffekseerEffect::GetInstance()) EffekseerEffect::GetInstance()->Draw();
	}

	if (showHint_ && hintHandle_ != -1)
	{
		VECTOR screenPos = ConvWorldPosToScreenPos(hintWorldPos_);
		int w = 0, h = 0;
		GetGraphSize(hintHandle_, &w, &h);
		// オブジェクト上に表示する
		const int drawX1 = static_cast<int>(screenPos.x) - (w / 2);
		const int drawY1 = static_cast<int>(screenPos.y) - h - 20;
		const int drawX2 = drawX1 + w;
		const int drawY2 = drawY1 + h;
		DrawExtendGraph(drawX1, drawY1, drawX2, drawY2, hintHandle_, true);
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

	// ポストエフェクトを廃止したため
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawGraph(0, 0, mainScreen, FALSE);

	//DrawFormatString(10, 200, GetColor(255, 255, 255), "pattern progress: %d / %d, tries: %d",
	//	static_cast<int>(buttonSP_), buttonPTarget_, buttonPCount_);

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

	// アンサーポジションのオブジェクトの座標を表示
	DrawFormatString(10, 240, GetColor(255, 255, 255), "Answer Position: (%.1f, %.1f, %.1f)",
		ANSWER_VECTOR_LENGTH[0].x,
		ANSWER_VECTOR_LENGTH[0].y,
		ANSWER_VECTOR_LENGTH[0].z);

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
	DrawFormatString(static_cast<int>(objectPos.x), static_cast<int>(objectPos.y) - 100, 0xffff00, "↓");
}

void TutorialScene::Release(void)
{
	// デバック
	OutputDebugStringA("TutorialScene::Release start\n");
	ResourceManager::GetInstance().DebugDump();

	// 複製したモデルが残っていれば削除
	if (pinID_ != -1)
	{
		MV1DeleteModel(pinID_);
		pinID_ = -1;
	}

	// 全オブジェクト解放
	for (auto& obj : objects_)
	{
		if (obj) { delete obj; }
	}
	objects_.clear();

	// プレイヤー配列をクリア
	players_.clear();

	// スクリーンハンドルの削除
	if (screenHandle1_ != -1) { DeleteGraph(screenHandle1_); screenHandle1_ = -1; }
	if (screenHandle2_ != -1) { DeleteGraph(screenHandle2_); screenHandle2_ = -1; }

	if (camera_)
	{
		delete camera_;
		camera_ = nullptr;
	}
}

void TutorialScene::Hint(void)
{
	const bool isEDown = InputManager::GetInstance()->IsTrgDown(KEY_INPUT_E)
		|| InputManager::GetInstance()->IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT);

	if (!isEDown) return;

	// ヒントを閉じる
	if (showHint_)
	{
		showHint_ = false;
		hintHandle_ = -1;
		return;
	}

	// ヒントの表示条件式にゃん
	for (auto& obj : objects_)
	{
		if (!obj) continue;
		if (obj->GetObjectType() != ObjectBase::OBJECT_TYPE::WBOX) continue;

		// プレイヤーとの距離
		bool isnear = false;

		for (auto& p : players_)
		{
			const float dist = VSize(VSub(p.player_->GetTransform().pos, obj->GetTransform().pos));
			if (dist < 60.0f)
			{
				isnear = true;
				break;
			}
		}

		if (!isnear) continue;

		// ヒント表示
		showHint_ = true;
		hintWorldPos_ = obj->GetPos();
		hintHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::HINTO).handleId_;
		break;
	}
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
		"あれ？宝箱があるけど、この世界では開けられないみたい…。\n開ける方法がどこかにあるのかも！\n探してみよう！",
		[this]() -> bool {
			return showHint_;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU4).handleId_
	);

	// ステップ4: ボタン操作
	tutorial_.AddStep(
		"あっ！ヒントを見つけたね！\nヒントに書いてあるボタンを押したら何か変わるかな。\n近づいて Fキー または パッドのBボタンで押してみよう！",
		[this]() -> bool {
			return  TbutonCount_;
		},
		nullptr,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::ENOGU5).handleId_
	);

	// ステップ5: オブジェクトの操作
	tutorial_.AddStep(
		"やった！謎が解けたみたい宝箱を開けてみよう！\n中に樽を見つけたね。\n近づいて EキーまたはXで持ち上げてみよう！",
		[this]() -> bool {
			for (auto& obj : objects_)
			{
				if (!obj) continue;
				for (auto& player : players_)
				{
					const float dist = VSize(VSub(player.player_->GetTransform().pos, obj->GetTransform().pos));
					// 距離判定とキー入力を正しくグループ化して評価する
					if (dist < 90.0f &&
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
				{
					// 位置に置かれた瞬間
					if (!placedSEPlayed_)
					{
						if (auto am = AudioManager::GetInstance())
						{
							am->PlaySE(SoundID::SE_SUCCESS);
						}
						placedSEPlayed_ = true;
					}
					return true;
				}
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