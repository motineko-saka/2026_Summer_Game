#pragma once
#include <DxLib.h>
#include "../../Charactor/CharactorBase.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Scene/SceneBase.h"

class ObjectBase : public CharactorBase
{
public:
	// オブジェクトの種類
	enum class OBJECT_TYPE
	{
		DEFAULT,
		WBOX,
		AKEG,
		SCENE_PROP,
		OPEN_BUTTON,
		NUMBER_BUTTON,
		BUTTON,
		GOAL_BUTTON,
		PRESS_BUTTON,
		GEAR,
		GEAR_OBJECT,
		ROCK,
		AXE,
		CHEST,
		OPENCHEST,
	};

	// world と種類を指定可能に
	ObjectBase(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type = OBJECT_TYPE::DEFAULT, bool isHold = false);
	virtual ~ObjectBase(void);

	//void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	// ゲッター関数
	const SceneBase::WORLD& GetWorld(void) const{ return world_; }
	const Transform& GetTransform(void) const{ return transform_; }
	const VECTOR& GetPos(void) const{ return transform_.pos; }
	const OBJECT_TYPE& GetObjectType() const{ return type_; }
	const OBJECT_TYPE& GetType() const{ return type_; }
	const VECTOR& GetAnswerPos(void) const{ return ansVec_; }

	// セッター関数
	void SetViewWorld(SceneBase::WORLD world) { viewWorld_ = world; }
	void SetPlaced(bool placed);
	void SetObjectType(OBJECT_TYPE t) { type_ = t; }
	void SetButtomPushed(bool isPush) { isButtomPushed_ = isPush; }
	void SetPosition(const VECTOR& pos) { transform_.pos = pos; transform_.Update(); }
	void SetScale(const VECTOR& scl) { transform_.scl = scl; transform_.Update(); }
	void SetIsRot(const bool& is) { isRot_ = is; }

	// プレイヤーから押される処理
	void Push(const VECTOR& direction, float speed);

	const bool& IsPlaced() const { return placed_; }

	const bool& IsAnswerPosition() const { return isAnswerPosition_; }

	const bool& isPushButtom(void) const{ return isButtomPushed_; }

	const bool& IsGrabbed(void) const { return isGrabbed_; }

	void PushButton(void);
	void PressButton(void);

	// PUSH_BUTTON が踏まれているか取得
	const bool& IsPushButtonPressed(void) const { return isPressButton_; }

	// PUSH_BUTTON のリセット（次のフレーム判定のため）
	void ResetPushButton(void) { isPressButton_ = false; }

protected:
	// 衝突判定用カプセルの半径
	float capsule_radius_ = 30.0f;

	// 答えにおいて正誤判定をするオブジェクトかどうか
	bool isAnswerObject_ = true;

	// PUSH_BUTTON が踏まれているか
	bool isPressButton_ = false;

	// BUTTON が押されているか
	bool isButtomPushed_ = false;

	// 持っているか
	bool isHoldAble_ = false;

	int handFrame_;

	// オブジェクト種類
	OBJECT_TYPE type_{ OBJECT_TYPE::DEFAULT };

	// リソースロード
	void InitLoad(void)override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	virtual void InitObjTrans(void) {};

	virtual void InitObjCol(void) {};

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void UpdateProcess(void) override;

	void UpdateProcessPost(void) override;

	// 継承するオブジェクトごとの更新処理
	virtual void ObjectUpdateProcess(void) {};

	virtual void SetFlame(const Transform* follow) {};

	ColliderBase::TAG tag_;
private:
	// 衝突判定用ライン開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用ライン終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部座標
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 100.0f, 0.0f };

	// 衝突判定用カプセル下部座標
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// オブジェクトの押される抵抗力（0.0～1.0）
	static constexpr float PUSH_RESISTANCE = 0.1f;

	SceneBase::WORLD world_;		// 
	SceneBase::WORLD viewWorld_;	// 今写っている世界

	// 押されて移動する量
	VECTOR pushPow_;

	// 答えの座標
	VECTOR ansVec_;

	VECTOR defaultScale_{ 1.0f, 1.0f, 1.0f };

	// 答えに置かれているかどうか
	bool isAnswerPosition_ = false;

	// つかまれているかどうか 
	// true:つかまれている false:つかまれていない
	bool isGrabbed_ = false;

	bool isRot_ = false;

	// 設置済みフラグ（true のとき掴めない、動かせない）
	bool placed_{ false };

	// 答えの場所とオブジェクトの衝突判定
	void CheckAnswer(void);
};