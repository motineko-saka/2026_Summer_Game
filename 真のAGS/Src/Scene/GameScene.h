#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
class Stage;
class SkyDome;
class Player;
class EnemyManager;
class Camera;
class Object;

class GameScene : public SceneBase
{

public:
	// 世界
	enum class WORLD
	{
		LEFT = 0,
		RIGHT,
		ANSWER,
	};

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:
	constexpr static int STAGE_NUM = 2;

	Stage* stage_;
	std::vector<Stage*> stages_;
	SkyDome* skyDome_;

	// プレイヤー1
	Player* player1_;
	Camera* camera1_;

	// プレイヤー2
	Player* player2_;
	Camera* camera2_;

	EnemyManager* enemyManager_;

	Object* object_;

	// 画面分割用のスクリーンハンドル
	int screenHandle1_;
	int screenHandle2_;

	// 画面サイズ
	int screenWidth_;
	int screenHeight_;

	// 各プレイヤーの描画
	void DrawPlayer1Screen(void);
	void DrawPlayer2Screen(void);
};