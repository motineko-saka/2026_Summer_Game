#pragma once

#include "ObjectBase.h"

class ModelMaterial;
class ModelRenderer;

class Gate :
	public ObjectBase
{
public:
	Gate(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

	void Draw() override;

private:
	// オブジェクト種類
	OBJECT_TYPE type_{ OBJECT_TYPE::CHEST };

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

	// リソースロード
	void InitLoad(void)override;
	void InitObjTrans(void)override;

	// 初期化後の個別処理
	void InitPost(void)override;

	void ObjectUpdateProcess(void)override;
};

