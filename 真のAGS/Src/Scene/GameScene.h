#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/GameObject/ObjectBase.h"
#include "../Object/Actor/Charactor/GameObject/Board.h"
#include "../Object/Actor/Charactor/GameObject/Panel.h"
#include <vector>
#include <memory>
#include <array>

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
		bool isPlayerHitObject_;
	};

	static constexpr float INTERACT_DISTANCE = 100.0f;

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

	// カメラ
	Camera* camera_;

	// プレイヤーの数
	static constexpr int PLAYER_NUM = 2;

	constexpr static VECTOR ANSWER_VECTOR = { 1260.0f, -720.0f, -50.5f };
	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = {
		{-1260.0f, -720.0f, -50.5f},
		{-1260.0f, -720.0f, -50.5f},
		{ 1260.0f, -720.0f, -50.5f},
		{-1260.0f, -720.0f, -50.5f},
		{-1260.0f, -720.0f, -50.5f},
	};

	std::unique_ptr<StageManager> stageManager_;
	std::unique_ptr<SkyDome> skyDome_;
	std::unique_ptr<LightPillar> lightPillar_;
	std::vector<PlayerS> players_;

	std::vector<std::unique_ptr<ObjectBase>> objects_;

	// Board と Panel の管理
	std::unique_ptr<Board> board_;
	std::vector<std::unique_ptr<Panel>> panels_;

	int screenHandle1_;
	int screenHandle2_;
	int screenWidth_;
	int screenHeight_;
	int pinID_;

	bool isPause_ = false;
	int stageProgress_ = 0;
	bool isClear_ = false;
	bool isBreak_ = false;
	bool isRot_ = false;

	VECTOR buttonPos_ = { -850.0f, -616.0f, 522.0f };
	VECTOR rockPos_ = { -660.0f, -320.0f, 630.0f };
	VECTOR endPos_ = { 1364.0f, -300.0f, 620.0f };

	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	void CheckCollisions(void);
	const void MakeNewObject(std::vector<ObjectBase*>& newObjects);
	const void ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects);
	void DrawNamePlate(std::string str, VECTOR pos);
	void ChangeScene(const std::shared_ptr<SceneBase>& scene) const;

	// Board と Panel の初期化
	void InitializeBoardAndPanels(void);

	template<class objectClass>
	void PushObject(SceneBase::WORLD w, const VECTOR& ans, ObjectBase::OBJECT_TYPE type, const VECTOR& pos, const VECTOR& scl)
	{
		std::unique_ptr<objectClass> o = std::make_unique<objectClass>(w, ans, type);
		o->Init();
		o->SetPosition(pos);
		o->SetScale(scl);
		objects_.push_back(std::move(o));
	}

	int shadowMapHandle_;
};