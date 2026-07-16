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

	void TyutorialTEXT(void);
	void Hint(void);
private:

	constexpr static VECTOR ANSWER_VECTOR_LENGTH[] = { {760.0f, -520.0f, 600.0f} ,
														{0.0f, -600.0f, 50.0f},
														{0.0f, -600.0f, 50.0f}
	};

	// カメラ
	Camera* camera_;

	// ゲームクリアまでの時間
	constexpr static float END_TIME = 3.0f;

	std::unique_ptr<StageManager> stageManager_;

	std::unique_ptr<SkyDome> skyDome_;

	std::unique_ptr<LightPillar> lightPillar_;
	bool isPillar_ = false;

	//EnemyManager* enemyManager_;
	std::vector<PlayerS> players_;

	// 複数のオブジェクトを管理
	std::vector<ObjectBase*> objects_;

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

	const void ButtonProcess(ObjectBase& obj, std::vector<ObjectBase*>& newObjects, std::vector<int>& removeIndices);

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
	VECTOR tempCameraRot_ = { 0,0,0 };
	float score_ = 0;
	// SE
	bool placedSEPlayed_ = false;

	// ボタンの正解パターン（右, 左, 左, 右, 左）
	std::vector<SceneBase::WORLD> buttonRequiredPattern_{ SceneBase::WORLD::RIGHT, SceneBase::WORLD::LEFT, SceneBase::WORLD::LEFT, SceneBase::WORLD::RIGHT, SceneBase::WORLD::LEFT };

	std::vector<SceneBase::WORLD> buttonPressHistory_;
	int buttonPTarget_ = 5;
	int buttonPCount_ = 0;
	// 現在進行
	size_t buttonSP_ = 0;

	// ヒント関連
	bool showHint_ = false;                       // ヒントを表示中か
	VECTOR hintWorldPos_ = { 0.0f, 0.0f, 0.0f };   // ヒントを表示するワールド座標（オブジェクト位置）
	int hintHandle_ = -1;                         // 画像ハンドル

};