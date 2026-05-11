#pragma once
#include <DxLib.h>
#include <functional>
#include "../CharactorBase.h"
class Player;

class EnemyBase : public CharactorBase
{

public:

	// 種別
	enum class TYPE
	{
		RAT,
		ROBOT,
	};

	// エネミーデータ
	struct EnemyData
	{
		int id;
		EnemyBase::TYPE type;
		int hp;
		VECTOR defaultPos;
		float moveRange;
	};

	// コンストラクタ
	EnemyBase(const EnemyBase::EnemyData& data,Player* player);

	// デストラクタ
	virtual ~EnemyBase(void) override;

	// 描画
	virtual void Draw(void) override;

protected:

	// 状態管理
	int stateBase_;
	// 状態管理(状態遷移時初期処理)
	std::map<int, std::function<void(void)>> stateChanges_;
	// 状態管理(更新ステップ)
	std::function<void(void)> stateUpdate_;

	Player* player_;

	// 種別
	TYPE type_;

	// HP
	int hp_;

	// 初期位置
	const VECTOR defaultPos_;

	// 移動可能範囲
	const float moveRange_;

	// リソースロード
	void InitLoad(void) override {}

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override {}

	// 衝突判定の初期化
	void InitCollider(void) override {}

	// アニメーションの初期化
	void InitAnimation(void) override {}

	// 初期化後の個別処理
	void InitPost(void) override {}

	// 更新系
	virtual void UpdateProcessPost(void) override {}

	// 移動可能範囲判定
	bool InMovableRange(void) const;

	// 状態遷移
	void ChangeState(int state);
};
