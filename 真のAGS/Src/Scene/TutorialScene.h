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
class Camera;
class ObjectBase;
class Wall;
class LightPillar;
class ModelRenderer;
class ModelMaterial;
class PixelRenderer;
class PixelMaterial;

class TutorialScene : public SceneBase
{
	struct PlayerS
	{
		Player* player_;
		Camera* camera_;
		bool isPlayerHitObject_;
	};

	// ポストエフェクトモード
	enum class MODE
	{
		MAIN,
		MONO,
		SCAN,
		LENS,
		VINE,
		MAX
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

	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = { {760.0f, -520.0f, 600.0f} ,
														{0.0f, -600.0f, -50.0f},
	//													{-900.0f, -500.0f, 900.5f},
	};

	// ゲームクリアまでの時間（秒）
	constexpr static float END_TIME = 3.0f;

	StageManager* stageManager_;

	SkyDome* skyDome_;

	std::unique_ptr<LightPillar> lightPillar_;
	bool isPillar_ = false;

	// プレイヤー1
	Player* player1_;
	Camera* camera1_;

	// プレイヤー2
	Player* player2_;
	Camera* camera2_;

	// ポストエフェクトモード
	MODE mode_;

	//EnemyManager* enemyManager_;
	std::vector<PlayerS> players_;

	std::unique_ptr<Wall> wall_;

	// 複数のオブジェクトを管理
	std::vector<ObjectBase*> objects_;

	// ポストエフェクト用スクリーン
	int postEffectScreen_;

	// シェーダー
	std::unique_ptr<PixelMaterial> pixelMaterial_;
	std::unique_ptr<PixelRenderer> pixelRenderer_;
	
	// 画面分割用のスクリーンハンドル
	int screenHandle1_;
	int screenHandle2_;

	// 画面サイズ
	int screenWidth_;
	int screenHeight_;

	// 衝突判定フラグ
	bool isPlayer1HitObject_;
	bool isPlayer2HitObject_;

	int pinID_;

	float endTimer_ = 0.0f;

	// 現在選択中のプレイヤー
	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	// 各プレイヤーの描画
	void DrawPlayer1Screen(void);
	void DrawPlayer2Screen(void);

	// オブジェクトを生成（今は樽だけ）
	const void MakeNewObject(std::vector<ObjectBase*>& newObjects);

	const void ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects);

	// 衝突判定チェック
	void CheckCollisions(void);

	// 答えの場所にあるのか判定
	void AnswerChack(void);

	// チュートリアル
	//--------------------------------------------
	Tutorial tutorial_; // 追加
	int moveStepe_ = 650; // 移動する距離
	bool isEndTutorial_ = false;
};