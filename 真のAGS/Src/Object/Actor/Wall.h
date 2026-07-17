#pragma once
#include "ActorBase.h"

class ModelMaterial;
class ModelRenderer;

class Wall :
    public ActorBase
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

    Wall(VECTOR pos, bool isRot = false);
    ~Wall(void) override;

	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;
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

private:
	static constexpr VECTOR STAGE_DEFAULT_POS = { 0.0f, 0.0f, 0.0f };

	static constexpr VECTOR STAGE_DEFAULT_SCALE = { 1.0f,1000.0f,1000.0f };

	// 座標一時保存変数
	VECTOR tempPos_;
	bool isRot_ = false;

	std::unique_ptr<ModelMaterial> vertexMaterial_;
	std::unique_ptr<ModelRenderer> vertexRenderer_;

	// 時間
	float time_;

	float dissolveTime_;

	// シェーダ
	int shader_;

	// 定数バッファの確保サイズ(FLOAT4をいくつ作るか)
	int constBufFloat4Size_;

	// 定数バッファハンドル
	int constBuf_;


	// シェーダ
	int shaderVS_;

	// 定数バッファの確保サイズ(FLOAT4をいくつ作るか)
	int constBufFloat4SizeVS_;

	// 定数バッファハンドル
	int constBufVS_;

	const int CONSTANT_BUF_SLOT_BEGIN_VS = 1;
};