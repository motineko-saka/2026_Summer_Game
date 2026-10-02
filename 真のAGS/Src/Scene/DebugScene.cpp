#include <fstream>
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Collider/ColliderModel.h"
#include "DebugScene.h"

DebugScene::DebugScene(void)
	:
	SceneBase()
{
}

DebugScene::~DebugScene(void)
{
}

void DebugScene::Init(void)
{
}

void DebugScene::Update(void)
{
}

void DebugScene::Draw(void)
{
	// デバッグポイント群を球体描画
	int y = 20;

	for (const auto& point : points_)
	{
		DrawSphere3D(
			point,
			30.0f,
			16,
			GetColor(255, 0, 0),
			GetColor(255, 0, 0),
			false);

		DrawFormatString(20, y,
			0x000000, "座標(%.2f, %.2f, %.2f)",
			point.x, point.y, point.z);

		y += 20;
	}
}

void DebugScene::Release(void)
{
	// デバッグポイント群
	points_.clear();
}

void DebugScene::SavePoints(void)
{
	std::ofstream ofs("Data/Csv/PointSave.txt");
	if (!ofs) {
		return;
	}
	// 形式: x y z
	for (const VECTOR& p : points_) {
		ofs << p.x << " " << p.y << " " << p.z << "\n";
	}
	ofs.close();
}