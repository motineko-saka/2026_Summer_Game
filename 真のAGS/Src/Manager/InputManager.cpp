#include <DxLib.h>
#include <cmath>
#include "InputManager.h"

InputManager* InputManager::instance_ = nullptr;

namespace
{
	// ゲームで使用したいキーはここに登録しておく
	constexpr int USE_KEYS[] =
	{
		KEY_INPUT_0, KEY_INPUT_1, KEY_INPUT_2, KEY_INPUT_3, KEY_INPUT_4,
		KEY_INPUT_5, KEY_INPUT_6, KEY_INPUT_7, KEY_INPUT_8, KEY_INPUT_9,

		KEY_INPUT_RETURN, KEY_INPUT_SPACE,

		KEY_INPUT_W, KEY_INPUT_A, KEY_INPUT_S, KEY_INPUT_D,

		KEY_INPUT_UP, KEY_INPUT_DOWN, KEY_INPUT_LEFT, KEY_INPUT_RIGHT,

		KEY_INPUT_Q, KEY_INPUT_E, KEY_INPUT_R, KEY_INPUT_F,
		KEY_INPUT_Z, KEY_INPUT_X, KEY_INPUT_P, KEY_INPUT_H,

		KEY_INPUT_ESCAPE, KEY_INPUT_TAB, KEY_INPUT_LSHIFT, KEY_INPUT_LCONTROL,
	};
}

// ---------------------------------------------------------------
// 初期化・更新
// ---------------------------------------------------------------
void InputManager::Init(void)
{
	for (int key : USE_KEYS)
	{
		Add(key);
	}

	AddMouse(MOUSE_INPUT_LEFT);
	AddMouse(MOUSE_INPUT_RIGHT);
}

void InputManager::Update(void)
{
	// キーボード
	for (auto& [key, info] : keyInfos_)
	{
		UpdateInfo(info, CheckHitKey(key) != 0);
	}

	// マウス
	mouseInput_ = GetMouseInput();

	int x = 0;
	int y = 0;
	GetMousePoint(&x, &y);
	mousePos_ = Vector2(x, y);

	for (auto& [button, info] : mouseInfos_)
	{
		// 左右同時押しにも対応するためビットで判定
		UpdateInfo(info, (mouseInput_ & button) != 0);
	}

	// パッド
	for (int i = 0; i < PAD_NUM; i++)
	{
		SetJPadInState(static_cast<JOYPAD_NO>(i));
	}
}

void InputManager::UpdateInfo(Info& info, bool isPressed)
{
	info.keyOld = info.keyNew;
	info.keyNew = isPressed;
	info.keyTrgDown = info.keyNew && !info.keyOld;
	info.keyTrgUp = !info.keyNew && info.keyOld;
}

// ---------------------------------------------------------------
// キーボード
// ---------------------------------------------------------------
void InputManager::Add(int key)
{
	Info info;
	info.key = key;
	keyInfos_.emplace(key, info);
}

void InputManager::AddMouse(int button)
{
	MouseInfo info;
	info.key = button;
	mouseInfos_.emplace(button, info);
}

void InputManager::Clear(void)
{
	keyInfos_.clear();
}

bool InputManager::IsNew(int key) const
{
	return Find(key).keyNew;
}

bool InputManager::IsTrgDown(int key) const
{
	return Find(key).keyTrgDown;
}

bool InputManager::IsTrgUp(int key) const
{
	return Find(key).keyTrgUp;
}

const InputManager::Info& InputManager::Find(int key) const
{
	auto it = keyInfos_.find(key);
	return (it != keyInfos_.end()) ? it->second : infoEmpty_;
}

// ---------------------------------------------------------------
// マウス
// ---------------------------------------------------------------
bool InputManager::IsClickMouseLeft(void) const
{
	return (mouseInput_ & MOUSE_INPUT_LEFT) != 0;
}

bool InputManager::IsClickMouseRight(void) const
{
	return (mouseInput_ & MOUSE_INPUT_RIGHT) != 0;
}

bool InputManager::IsTrgMouseLeft(void) const
{
	return FindMouse(MOUSE_INPUT_LEFT).keyTrgDown;
}

bool InputManager::IsTrgMouseRight(void) const
{
	return FindMouse(MOUSE_INPUT_RIGHT).keyTrgDown;
}

const InputManager::MouseInfo& InputManager::FindMouse(int key) const
{
	auto it = mouseInfos_.find(key);
	return (it != mouseInfos_.end()) ? it->second : mouseInfoEmpty_;
}

// ---------------------------------------------------------------
// ゲームパッド
// ---------------------------------------------------------------
InputManager::JOYPAD_TYPE InputManager::GetJPadType(JOYPAD_NO no) const
{
	return static_cast<JOYPAD_TYPE>(GetJoypadType(static_cast<int>(no)));
}

DINPUT_JOYSTATE InputManager::GetJPadDInputState(JOYPAD_NO no) const
{
	DINPUT_JOYSTATE state = {};
	GetJoypadDirectInputState(static_cast<int>(no), &state);
	return state;
}

XINPUT_STATE InputManager::GetJPadXInputState(JOYPAD_NO no) const
{
	XINPUT_STATE state = {};
	GetJoypadXInputState(static_cast<int>(no), &state);
	return state;
}

void InputManager::SetJPadInState(JOYPAD_NO jpNo)
{
	const int no = static_cast<int>(jpNo);
	if (no < 0 || no >= PAD_NUM) return;

	const JOYPAD_IN_STATE stateNew = GetJPadInputState(jpNo);
	JOYPAD_IN_STATE& stateNow = padInfos_[no];

	for (int i = 0; i < BTN_NUM; i++)
	{
		stateNow.ButtonsOld[i] = stateNow.ButtonsNew[i];
		stateNow.ButtonsNew[i] = stateNew.ButtonsNew[i];

		stateNow.IsOld[i] = stateNow.IsNew[i];
		stateNow.IsNew[i] = stateNow.ButtonsNew[i] > 0;

		stateNow.IsTrgDown[i] = stateNow.IsNew[i] && !stateNow.IsOld[i];
		stateNow.IsTrgUp[i] = !stateNow.IsNew[i] && stateNow.IsOld[i];
	}

	stateNow.AKeyLX = stateNew.AKeyLX;
	stateNow.AKeyLY = stateNew.AKeyLY;
	stateNow.AKeyRX = stateNew.AKeyRX;
	stateNow.AKeyRY = stateNew.AKeyRY;
}

InputManager::JOYPAD_IN_STATE InputManager::GetJPadInputState(JOYPAD_NO no)
{
	JOYPAD_IN_STATE ret;

	auto set = [&ret](JOYPAD_BTN btn, unsigned char value)
		{
			ret.ButtonsNew[static_cast<int>(btn)] = value;
		};

	switch (GetJPadType(no))
	{
	case JOYPAD_TYPE::XBOX_ONE:
	{
		const DINPUT_JOYSTATE d = GetJPadDInputState(no);
		const XINPUT_STATE x = GetJPadXInputState(no);

		//   Y
		// X   B
		//   A
		set(JOYPAD_BTN::TOP, d.Buttons[3]);		// Y
		set(JOYPAD_BTN::LEFT, d.Buttons[2]);	// X
		set(JOYPAD_BTN::RIGHT, d.Buttons[1]);	// B
		set(JOYPAD_BTN::DOWN, d.Buttons[0]);	// A

		set(JOYPAD_BTN::R_TRIGGER, x.RightTrigger);	// RT
		set(JOYPAD_BTN::L_TRIGGER, x.LeftTrigger);	// LT
		set(JOYPAD_BTN::L2_TRIGGER, d.Buttons[4]);	// LB
		set(JOYPAD_BTN::R2_TRIGGER, d.Buttons[5]);	// RB
		set(JOYPAD_BTN::OPTION, d.Buttons[7]);		// MENU

		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;
		break;
	}

	case JOYPAD_TYPE::DUAL_SENSE:
	{
		const DINPUT_JOYSTATE d = GetJPadDInputState(no);

		//   △
		// □  ○
		//   ×
		set(JOYPAD_BTN::TOP, d.Buttons[3]);		// △
		set(JOYPAD_BTN::LEFT, d.Buttons[0]);	// □
		set(JOYPAD_BTN::RIGHT, d.Buttons[2]);	// ○
		set(JOYPAD_BTN::DOWN, d.Buttons[1]);	// ×

		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		ret.AKeyRX = d.Z;
		ret.AKeyRY = d.Rz;
		break;
	}

	default:
		// 未対応のコントローラ
		break;
	}

	return ret;
}

const InputManager::JOYPAD_IN_STATE* InputManager::FindPad(JOYPAD_NO no) const
{
	const int idx = static_cast<int>(no);
	if (idx < 0 || idx >= PAD_NUM) return nullptr;
	return &padInfos_[idx];
}

bool InputManager::IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	const JOYPAD_IN_STATE* pad = FindPad(no);
	return pad != nullptr && pad->IsNew[static_cast<int>(btn)];
}

bool InputManager::IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	const JOYPAD_IN_STATE* pad = FindPad(no);
	return pad != nullptr && pad->IsTrgDown[static_cast<int>(btn)];
}

bool InputManager::IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	const JOYPAD_IN_STATE* pad = FindPad(no);
	return pad != nullptr && pad->IsTrgUp[static_cast<int>(btn)];
}

VECTOR InputManager::GetDirectionXZAKey(int aKeyX, int aKeyY) const
{
	// スティックの入力値は -1000 〜 1000 なので -1.0 〜 1.0 に正規化
	const float dirX = static_cast<float>(aKeyX) / AKEY_VAL_MAX;
	const float dirZ = static_cast<float>(aKeyY) / AKEY_VAL_MAX;

	// 円形のデッドゾーン
	const float len = sqrtf(dirX * dirX + dirZ * dirZ);
	if (len < THRESHOLD)
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}

	// 方向のみを返す（倒し具合の強さは含まない）
	// Y は前に倒すとマイナスなので反転して Z にする
	return VGet(dirX / len, 0.0f, -dirZ / len);
}