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
class LightPillar;
class ModelRenderer;
class ModelMaterial;
class PixelRenderer;
class PixelMaterial;

class TutorialScene : public SceneBase
{
	struct PlayerS
	{
		std::unique_ptr<Player> player_;
		std::unique_ptr<Camera> camera_;
		bool isPlayerHitObject_;
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
	};

	// ゲームクリアまでの時間（秒）
	constexpr static float END_TIME = 3.0f;


	std::unique_ptr<StageManager> stageManager_;

	std::unique_ptr<SkyDome> skyDome_;

	std::unique_ptr<LightPillar> lightPillar_;
	bool isPillar_ = false;

	//EnemyManager* enemyManager_;
	std::vector<PlayerS> players_;

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

	// ゴール地点のmodelID（持っているオブジェクトと一緒）
	int pinID_;

	// クリアしてからClearシーンまで行くまでのタイマー
	float endTimer_ = 0.0f;

	// 現在選択中のプレイヤー
	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	// チュートリアル関連の初期化
	void TutorialInit(void);

	// オブジェクトを生成（今は樽だけ）
	const void MakeNewObject(std::vector<ObjectBase*>& newObjects);

	const void ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects);

	// 衝突判定チェック
	void CheckCollisions(void);

	// 答えの場所にあるのか判定
	void AnswerChack(void);

	// modelの上に出る矢印
	void DrawNamePlate(std::string str, VECTOR pos);

	// チュートリアル
	//--------------------------------------------
	Tutorial tutorial_; // 追加
	int moveStepe_ = 650; // 移動する距離
	bool isEndTutorial_ = false;
};