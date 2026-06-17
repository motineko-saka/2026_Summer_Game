#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Object/Actor/Charactor/Player.h"
#include <vector>
#include <memory>
#include "../UI/Tutorial.h"
class StageManager;
class SkyDome;
class Player;
class EnemyManager;
class Camera;
class ObjectBase;
class Wall;

class TutorialScene : public SceneBase
{
	struct PlayerS
	{
		Player* player_;
		Camera* camera_;
	};

public:
	// 世界
	enum class WORLD
	{
		LEFT = 0,
		RIGHT,
		ANSWER,
	};

	// コンストラクタ
	TutorialScene(void);

	// デストラクタ
	~TutorialScene(void) override;

	// 初期化
	void Init(void) override;

	// ロード
	void Load(void) override;

	// ロード後の初期化
	void LoadEnd(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:
	constexpr static VECTOR ANSWER_VECTOR = { 1260.0f, -720.0f, -50.5f };

	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = { {1000.0f, -500.0f, -50.5f} ,
														{0.0f, -600.0f, -50.0f},
														{-900.0f, -500.0f, 900.5f},
	};

	StageManager* stageManager_;

	SkyDome* skyDome_;

	// プレイヤー1
	Player* player1_;
	Camera* camera1_;

	// プレイヤー2
	Player* player2_;
	Camera* camera2_;

	//EnemyManager* enemyManager_;
	std::vector<PlayerS> players_;

	std::unique_ptr<Wall> wall_;

	// 複数のオブジェクトを管理
	std::vector<ObjectBase*> objects_;

	// 画面分割用のスクリーンハンドル
	int screenHandle1_;
	int screenHandle2_;

	// 画面サイズ
	int screenWidth_;
	int screenHeight_;

	// 衝突判定フラグ
	bool isPlayer1HitObject_;
	bool isPlayer2HitObject_;

	VECTOR ansVec_ = {};

	int pinID_;

	// 現在選択中のプレイヤー
	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	// 各プレイヤーの描画
	void DrawPlayer1Screen(void);
	void DrawPlayer2Screen(void);

	// 衝突判定チェック
	void CheckCollisions(void);

	// 答えの場所にあるのか判定
	void AnswerChack(void);

	// チュートリアル
	//--------------------------------------------
	Tutorial tutorial_; // 追加
	int moveStepe_ = 800; // 移動する距離

};