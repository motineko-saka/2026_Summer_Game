#pragma once
#include <DxLib.h>
#include "../Charactor/CharactorBase.h"
#include "../../../Scene/SceneBase.h"

class Object : public CharactorBase
{
public:
	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		MODEL,
		CAPSULE,
		MAX,
		VIEW_RANGE,
	};

	// オブジェクトの種類
	enum class OBJECT_TYPE
	{
		DEFAULT,   
		WBOX,    
		AKEG,   
		SCENE_PROP,
		BUTTOM,
	};

	// world と種類を指定可能に
	Object(SceneBase::WORLD world,VECTOR ansVec, OBJECT_TYPE type = OBJECT_TYPE::DEFAULT);
	virtual ~Object(void);

	//void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	SceneBase::WORLD GetViewWorld(void) { return viewWorld_; }

	SceneBase::WORLD GetWorld(void) { return world_; }

	void SetViewWorld(SceneBase::WORLD world) { viewWorld_ = world; }

	Transform& GetTransform(void) { return transform_; }
	VECTOR& GetPos(void) { return transform_.pos; }

	// プレイヤーから押される処理
	void Push(const VECTOR& direction, float speed);

	// 設置済みフラグ操作
	void SetPlaced(bool placed) { placed_ = placed; }
	bool IsPlaced() const { return placed_; }

	// 種類取得
	OBJECT_TYPE GetObjectType() const { return type_; }
	void SetObjectType(OBJECT_TYPE t) { type_ = t; }

	void SetButtomPushed(bool isPush) { isButtomPushed_ = isPush; }
	const OBJECT_TYPE GetType() const { return type_; }

	// 位置設定の簡易ヘルパ
	void SetPosition(const VECTOR& pos) { transform_.pos = pos; transform_.Update(); }

	bool IsAnswerPosition() const { return isAnswerPosition_; }

	// スケール設定/取得
	void SetScale(const VECTOR& scl) {transform_.scl = scl; transform_.Update(); }
	VECTOR GetScale() const { return transform_.scl; }

protected:

	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void UpdateProcess(void) override;

	void UpdateProcessPost(void) override;

private:
	// 衝突判定用ライン開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用ライン終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部座標
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 100.0f, 0.0f };

	// 衝突判定用カプセル下部座標
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// 衝突判定用カプセルの半径
	static constexpr float COL_CAPSULE_RADIUS = 30.0f;

	// オブジェクトの押される抵抗力（0.0～1.0）
	static constexpr float PUSH_RESISTANCE = 0.1f;

	SceneBase::WORLD world_;		// 
	SceneBase::WORLD viewWorld_;	// 今写っている世界

	// オブジェクト種類
	OBJECT_TYPE type_{ OBJECT_TYPE::DEFAULT };

	// 押されて移動する量
	VECTOR pushPow_;

	// 答えの座標
	VECTOR ansVec_;

	VECTOR defaultScale_{ 1.0f, 1.0f, 1.0f };

	// 答えのオブジェクトかどうか
	bool isAnswerPosition_;

	// つかまれているかどうか 
	// true:つかまれている false:つかまれていない
	bool isGrabbed_;

	bool isButtomPushed_ = false;

	// 設置済みフラグ（true のとき掴めない、動かせない）
	bool placed_{ false };

	// 答えの場所とオブジェクトの衝突判定
	void CheckAnswer(void);
};