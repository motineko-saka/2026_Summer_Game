#pragma once
#include "SceneBase.h"
#include "../Application.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/AnimationController.h"
class SkyDome;

class TitleScene : public SceneBase
{

public:

	enum class ANIM_TYPE
	{
		RUN,
		MAX,
	};

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	static constexpr int IMG_TITLE_POS_Y = 260;
	static constexpr int IMG_TITLE_POS_X = 640;

	static constexpr int IMG_PUSH_SPACE_POS_Y = 410;
	static constexpr int IMG_PUSH_SPACE_POS_X = 640;

	static constexpr VECTOR SPHERE_PLANET_DEFAULT_POS = { -250.0f, -100.0f, -100.0f };

	static constexpr VECTOR SPHERE_PLANET_DEFAULT_SCALE = { 0.7f,0.7f,0.7f };

	static constexpr VECTOR SPHERE_PLANET_ROT = { 90.0f + DX_PI_F / 180.0f,0.0f,0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_POS = { -250.0f, -32.0f, -105.0f };

	static constexpr VECTOR PLAYER_DEFAULT_SCALE = { 0.4f,0.4f,0.4f };

	// 惑星
	Transform bigPlanet_;
	Transform SpherePlanet_;
	
	Transform player_;

	AnimationController* animController_;
	SkyDome* skyDome_;

	// スカイドーム用の空Transform
	Transform empty_;

	int imgTitle_;
	int imgPushSpace_;

};
