#include "Tutorial.h"
#include <DxLib.h>
#include "../Application.h"

#ifndef KEY_TUTORIAL_NEXT
#define KEY_TUTORIAL_NEXT KEY_INPUT_Z
#endif

Tutorial::Tutorial()
	: step_(Step::NONE)
	, active_(false)
	, inputDelay_(0)
{
}

Tutorial::~Tutorial()
{
}

void Tutorial::Init()
{
	step_ = Step::NONE;
	active_ = false;
	inputDelay_ = 0;
}

void Tutorial::Start()
{
	step_ = Step::MOVE;
	active_ = true;
	inputDelay_ = DELAY_MAX;
}

static bool AnyKey_WASD()
{
	return CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_A) ||
		CheckHitKey(KEY_INPUT_S) || CheckHitKey(KEY_INPUT_D);
}

const char* Tutorial::GetTextForStep(Step s) const
{
	switch (s)
	{
	case Step::MOVE:
		return "移動: WASDキーで移動します。\n(W:前, S:後, A:左, D:右)\nいずれかのキーを押してください。";
	case Step::CAMERA_ANGLE:
		return "カメラ角度: 方向キー ↑ ↓ ← → でカメラ角度を変えられます。\nいずれかの方向キーを押してください。";
	case Step::CAMERA_STATE:
		return "カメラ状態切替: Ctrlキーでカメラの状態を切り替えます。\nCtrlを押してください。";
	case Step::SWITCH_CHAR:
		return "キャラ切替: Tabキーで操作キャラを切り替えます。\nTabを押してください。";
	case Step::FINISHED:
		return "チュートリアルが完了しました。\nZ/Enter/Spaceで閉じます。";
	default:
		return "";
	}
}

void Tutorial::Update()
{
	if (!active_) return;

	if (inputDelay_ > 0) --inputDelay_;

	switch (step_)
	{
	case Step::MOVE:
		if (inputDelay_ == 0 && AnyKey_WASD())
		{
			step_ = Step::CAMERA_ANGLE;
			inputDelay_ = DELAY_MAX;
		}
		break;

	case Step::CAMERA_ANGLE:
		if (inputDelay_ == 0 &&
			(CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_DOWN) ||
				CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_RIGHT)))
		{
			step_ = Step::CAMERA_STATE;
			inputDelay_ = DELAY_MAX;
		}
		break;

	case Step::CAMERA_STATE:
		if (inputDelay_ == 0 &&
			(CheckHitKey(KEY_INPUT_LCONTROL) || CheckHitKey(KEY_INPUT_RCONTROL)))
		{
			step_ = Step::SWITCH_CHAR;
			inputDelay_ = DELAY_MAX;
		}
		break;

	case Step::SWITCH_CHAR:
		if (inputDelay_ == 0 && CheckHitKey(KEY_INPUT_TAB))
		{
			step_ = Step::FINISHED;
			inputDelay_ = DELAY_MAX;
		}
		break;

	case Step::FINISHED:
		if (inputDelay_ == 0 &&
			(CheckHitKey(KEY_TUTORIAL_NEXT) || CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE)))
		{
			active_ = false;
			step_ = Step::NONE;
		}
		break;

	default:
		break;
	}
}

void Tutorial::Draw() const
{
	if (!active_) return;

	const int margin = 24;
	const int boxH = 160;
	int x0 = margin;
	int y0 = Application::SCREEN_SIZE_Y - boxH - margin;
	int x1 = Application::SCREEN_SIZE_X - margin;
	int y1 = Application::SCREEN_SIZE_Y - margin;

	// 背景ボックス（半透明）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
	DrawBox(x0, y0, x1, y1, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 文字表示（左上に余白）
	const int tx = x0 + 16;
	const int ty = y0 + 12;
	const char* text = GetTextForStep(step_);
	DrawString(tx, ty, text, GetColor(255, 255, 255));

	// 補助表示: 現在のステップ名（右下）
	switch (step_)
	{
	case Step::MOVE:           DrawString(x1 - 220, y1 - 28, "STEP: 移動", GetColor(200, 200, 200)); break;
	case Step::CAMERA_ANGLE:   DrawString(x1 - 220, y1 - 28, "STEP: カメラ角度", GetColor(200, 200, 200)); break;
	case Step::CAMERA_STATE:   DrawString(x1 - 220, y1 - 28, "STEP: カメラ状態", GetColor(200, 200, 200)); break;
	case Step::SWITCH_CHAR:    DrawString(x1 - 220, y1 - 28, "STEP: キャラ切替", GetColor(200, 200, 200)); break;
	case Step::FINISHED:       DrawString(x1 - 220, y1 - 28, "STEP: 完了", GetColor(200, 200, 200)); break;
	default: break;
	}
}