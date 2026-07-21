#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/GameObject/ObjectBase.h"
#include <vector>
#include <memory>
class StageManager;
class SkyDome;
class Player;
class EnemyManager;
class Camera;
class ObjectBase;
class LightPillar;

class GameScene : public SceneBase
{
public:
	struct PlayerS
	{
		std::unique_ptr<Player> player_;
		std::unique_ptr<Camera> camera_;
		bool isPlayerHitObject_;
	};

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 読み込み
	void Load(void)	override;

	// 読み込み後の初期化
	void LoadEnd(void)	override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	// カメラ
	Camera* camera_;

	// プレイヤーの数
	static constexpr int PLAYER_NUM = 2;

	constexpr static VECTOR ANSWER_VECTOR = { 1260.0f, -720.0f, -50.5f };

	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = {	{-1260.0f, -720.0f, -50.5f},
														{-1260.0f, -720.0f, -50.5f},
														{ 1260.0f, -720.0f, -50.5f},
														{-1260.0f, -720.0f, -50.5f},
														{-1260.0f, -720.0f, -50.5f},
														};
	std::unique_ptr<StageManager> stageManager_;

	std::unique_ptr<SkyDome> skyDome_;

	std::unique_ptr<LightPillar> lightPillar_;

	std::vector<PlayerS> players_;

	//EnemyManager* enemyManager_;

	// 複数のオブジェクトを管理
	std::vector<std::unique_ptr<ObjectBase>> objects_;

	// 画面分割用のスクリーンハンドル
	int screenHandle1_;
	int screenHandle2_;

	// 画面サイズ
	int screenWidth_;
	int screenHeight_;

	int pinID_;

	bool isPause_ = false;

	int stageProgress_ = 0;
	bool isClear_ = false;

	VECTOR buttonPos_ = { -770.0f, -300.0f, 760.0f };
	VECTOR rockPos_ =	{ -660.0f, -320.0f, 630.0f };
	VECTOR endPos_ =	{ 1364.0f, -300.0f, 620.0f };

	// 現在選択中のプレイヤー
	Player::PLAYER_NO activePlayer_{ Player::PLAYER_NO::PLAYER1 };

	// 衝突判定チェック
	void CheckCollisions(void);

	// オブジェクトを生成（今は樽だけ）
	const void MakeNewObject(std::vector<ObjectBase*>& newObjects);

	const void ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects);

	void DrawNamePlate(std::string str, VECTOR pos);

	// ゲームシーンでのシーンチェンジのまとめ
	void ChangeScene(const std::shared_ptr<SceneBase>& scene) const;

	template<class objectClass>
	void PushObject(SceneBase::WORLD w, const VECTOR& ans, ObjectBase::OBJECT_TYPE type, const VECTOR& pos, const VECTOR& scl)
	{
		std::unique_ptr<objectClass> o = std::make_unique<objectClass>(w, ans, type);
		o->Init();
		o->SetPosition(pos);
		o->SetScale(scl);
		objects_.push_back(std::move(o));
	}

	// シャドウマップ用のハンドル
	int shadowMapHandle_;


};