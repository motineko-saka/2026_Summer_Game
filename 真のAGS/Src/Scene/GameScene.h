#pragma once

#include <vector>
#include <memory>
#include <array>
#include "../Object/Common/Transform.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/GameObject/ObjectBase.h"

class StageManager;
class SkyDome;
class Player;
class EnemyManager;
class Camera;
class ObjectBase;
class LightPillar;
class Board;
class Panel;

class GameScene : public SceneBase
{
public:

	struct PlayerS
	{
		std::unique_ptr<Player> player_;
		std::unique_ptr<Camera> camera_;
		bool isPlayerHitObject_ = false;
	};

	// コンストラクタ
	GameScene(void);
	~GameScene(void) override;

	void Init(void) override;
	void Load(void) override;
	void LoadEnd(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	// プレイヤーの数
	static constexpr int PLAYER_NUM = 2;

	// オブジェクトの数
	static constexpr int OBJECT_NUM = 10;

	static constexpr float INTERACT_DISTANCE = 100.0f;

	static constexpr float BUTTON_PUSH_RADIUS = 180.0f;

	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = {
		{-1260.0f, -720.0f, -50.5f},
		{-1260.0f, -720.0f, -50.5f},
		{ 1260.0f, -720.0f, -50.5f},
		{-1260.0f, -720.0f, -50.5f},
		{-1260.0f, -720.0f, -50.5f},
	};

	static constexpr VECTOR INIT_BUTTON_POS = { -850.0f, -616.0f, 522.0f };
	static constexpr VECTOR INIT_ROCK_POS = { -660.0f, -320.0f, 630.0f };
	static constexpr VECTOR INIT_CHEST_POS = { 1000.0f, 0.0f, 1000.0f };
	static constexpr VECTOR INIT_AXE_POS = { -500.0f, 0.0f, 0.0f };
	static constexpr VECTOR INIT_GATE_POS = { 1300.0f, -320.0f, 500.0f };
	static constexpr VECTOR INIT_GEAR_POS = { -600.0f, -620.0f, 0.0f };

	static constexpr VECTOR INIT_NUMBER_BUTTON_POS_ONE = { -517.0f,  -616.0f, -789.0f };
	static constexpr VECTOR INIT_NUMBER_BUTTON_POS_TWO = { -1106.0f, -616.0f, -745.0f };
	static constexpr VECTOR INIT_GOAL_BUTTON_POS = { -850.0f, -616.0f, -1500.0f };
	static constexpr VECTOR INIT_END_POS = { 1364.0f, -300.0f, 620.0f };

	std::unique_ptr<StageManager> stageManager_;
	std::unique_ptr<SkyDome> skyDome_;
	std::unique_ptr<LightPillar> lightPillar_;
	std::vector<PlayerS> players_;

	std::vector<std::unique_ptr<ObjectBase>> objects_;

	// スクリーン系
	int screenHandle1_;
	int screenHandle2_;
	int screenWidth_;
	int screenHeight_;

	// 答えを置く場所にモデルを描画するための変数
	int answerSpotModelHandle_;

	bool isClear_;
	bool isOpen_;

	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	void CheckCollisions(void);
	const void MakeNewObject(std::vector<std::unique_ptr<ObjectBase>>&);
	const bool ButtonProcess(ObjectBase& obj);
	void DrawNamePlate(std::string str, VECTOR pos);
	void ChangeScene(const std::shared_ptr<SceneBase>& scene) const;

	const void ButtonProcess(ObjectBase& obj, std::vector<std::unique_ptr<ObjectBase>>& newObjects, std::vector<int>& removeIndices);

	// オブジェクト追加の
	template<class objectClass>
	void PushObject(SceneBase::WORLD w, const VECTOR& ans, ObjectBase::OBJECT_TYPE type, const VECTOR& pos, const VECTOR& scl)
	{
		std::unique_ptr<objectClass> o = std::make_unique<objectClass>(w, ans, type);
		o->Init();
		o->SetPosition(pos);
		o->SetScale(scl);
		objects_.push_back(std::move(o));
	}

	void SetMouseCenterPos(int i, int& width, int& height);

	int shadowMapHandle_;

	//-------------------------
	// ボタンパターン
	//-------------------------
	std::vector<SceneBase::WORLD> pbuttonRequiredPattern_{ SceneBase::WORLD::RIGHT, SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT, SceneBase::WORLD::RIGHT };
	std::vector<SceneBase::WORLD> buttonRequiredPattern_{ SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT };
	int buttonPTarget_;
	int buttonPCount_;
	size_t buttonSP_;
	int butcount_;
};