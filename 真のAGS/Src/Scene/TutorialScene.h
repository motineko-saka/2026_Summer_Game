#pragma once
#include <vector>
#include <memory>
#include <string>
#include "../Object/Common/Transform.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../UI/Tutorial.h"

class StageManager;
class SkyDome;
class Camera;
class ObjectBase;
class LightPillar;
class ModelRenderer;
class ModelMaterial;
class SceneTransition;

class TutorialScene : public SceneBase
{
	struct PlayerS
	{
		std::unique_ptr<Player> player_;
		std::unique_ptr<Camera> camera_;
		bool isPlayerHitObject_ = false;
	};

public:
	// 世界
	enum class WORLD
	{
		LEFT = 0,
		RIGHT,
		ANSWER,
	};

	static constexpr float INTERACT_DISTANCE = 100.0f;

	// コンストラクタ / デストラクタ
	TutorialScene(void);
	~TutorialScene(void) override;

	// ライフサイクル
	void Init(void) override;
	void Load(void) override;
	void LoadEnd(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	// チュートリアル関連
	void TyutorialTEXT(void);
	void Hint(void);

private:
	// アンサーポジション
	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = {
		{760.0f, -520.0f, 600.0f},
		{0.0f,  -600.0f, 50.0f},
		{0.0f,  -600.0f, 50.0f}
	};

	// カメラ
	Camera* camera_ = nullptr;

	// ゲームクリアまでの時間
	constexpr static float END_TIME = 3.0f;

	std::unique_ptr<StageManager> stageManager_;
	std::unique_ptr<SkyDome>     skyDome_;
	std::unique_ptr<LightPillar> lightPillar_;
	bool isPillar_ = false;

	std::unique_ptr<SceneTransition> sceneTransition_;

	// プレイヤー
	std::vector<PlayerS> players_;

	// 管理するオブジェクト
	std::vector<ObjectBase*> objects_;

	// 分割スクリーン
	int screenHandle1_ = -1;
	int screenHandle2_ = -1;

	// 画面サイズ
	int screenWidth_ = 0;
	int screenHeight_ = 0;

	// ゴールのモデルID
	int pinID_ = -1;

	// クリア→遷移までのタイマー
	float endTimer_ = 0.0f;

	// 現在選択中のプレイヤー
	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	// 初期化補助
	void TutorialInit(void);

	// オブジェクト生成 / ボタン処理
	const void MakeNewObject(std::vector<ObjectBase*>& newObjects);
	const void ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects, std::vector<int>& removeIndices);

	// 衝突判定・解答判定
	void CheckCollisions(void);
	void AnswerChack(void);

	// 表示補助
	void DrawNamePlate(std::string str, VECTOR pos);

	//-------------------------
	// チュートリアル状態
	//-------------------------
	Tutorial tutorial_;
	int moveStepe_ = 650; // 移動判定距離
	bool isEndTutorial_ = false;
	VECTOR tempCameraRot_ = { 0,0,0 };
	float score_ = 0.0f;
	bool placedSEPlayed_ = false;

	//-------------------------
	// ボタンパターン
	//-------------------------
	std::vector<SceneBase::WORLD> buttonRequiredPattern_{ SceneBase::WORLD::RIGHT, SceneBase::WORLD::RIGHT, SceneBase::WORLD::RIGHT, SceneBase::WORLD::RIGHT, SceneBase::WORLD::RIGHT };
	std::vector<SceneBase::WORLD> buttonPressHistory_;
	int buttonPTarget_ = 5;
	int buttonPCount_ = 0;
	size_t buttonSP_ = 0;
	int butcount_ = false;
	int TbutonCount_ = false;
	bool buttonsLocked_ = false;  
	bool chestOpenedOnce_ = false;  

	//-------------------------
	// ヒント
	//-------------------------
	bool showHint_ = false;
	VECTOR hintWorldPos_ = { 0.0f, 0.0f, 0.0f };
	int hintHandle_ = -1;
};