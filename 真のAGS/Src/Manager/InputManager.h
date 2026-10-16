#pragma once
#include <array>
#include <map>
#include <DxLib.h>
#include "../Common/Vector2.h"

class InputManager
{
public:
	// ゲームコントローラーの認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class JOYPAD_NO
	{
		KEY_PAD1,			// キー入力とパッド１入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		INPUT_KEY = 4096	// キー入力
	};

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		OTHER = 0,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,
		MAX
	};

	// ゲームコントローラーボタン
	enum class JOYPAD_BTN
	{
		LEFT = 0,
		RIGHT,
		TOP,
		DOWN,
		R_TRIGGER,
		L_TRIGGER,
		R2_TRIGGER,
		L2_TRIGGER,
		OPTION,
		MAX
	};

	static constexpr int BTN_NUM = static_cast<int>(JOYPAD_BTN::MAX);

	// ゲームコントローラーの入力情報
	struct JOYPAD_IN_STATE
	{
		unsigned char ButtonsOld[BTN_NUM] = {};
		unsigned char ButtonsNew[BTN_NUM] = {};
		bool IsOld[BTN_NUM] = {};
		bool IsNew[BTN_NUM] = {};
		bool IsTrgDown[BTN_NUM] = {};
		bool IsTrgUp[BTN_NUM] = {};
		int AKeyLX = 0;
		int AKeyLY = 0;
		int AKeyRX = 0;
		int AKeyRY = 0;
	};

	// アナログキーの最大値
	static constexpr float AKEY_VAL_MAX = 1000.0f;

	// アナログキーの入力受付しきい値(0.0〜1.0)
	static constexpr float THRESHOLD = 0.35f;

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) instance_ = new InputManager(); }
	static InputManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { delete instance_; instance_ = nullptr; }

	void Init(void);	// 初期化
	void Update(void);	// 更新

	// 判定を行うキーを追加
	void Add(int key);

	// 判定を行うキーをクリア
	void Clear(void);

	// キーの押下判定
	bool IsNew(int key) const;

	// キーの押下判定(押しっぱなしはNG)
	bool IsTrgDown(int key) const;

	// キーを離した時の判定
	bool IsTrgUp(int key) const;

	// マウス座標の取得
	Vector2 GetMousePos(void) const { return mousePos_; }

	// マウスのクリック状態を取得(MOUSE_INPUT_LEFT、RIGHT のビットの組み合わせ)
	int GetMouse(void) const { return mouseInput_; }

	// マウスが左クリックされているか
	bool IsClickMouseLeft(void) const;

	// マウスが右クリックされているか
	bool IsClickMouseRight(void) const;

	// マウスが左クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseLeft(void) const;

	// マウスが右クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseRight(void) const;

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);

	// ボタンが押された
	bool IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const;
	bool IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const;
	bool IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const;

	// アナログキーの入力値から方向(正規化済み)を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY) const;

private:

	// キー・マウスボタンの入力情報
	struct Info
	{
		int key = -1;			// キーID
		bool keyOld = false;	// 1フレーム前の押下状態
		bool keyNew = false;	// 現フレームの押下状態
		bool keyTrgDown = false;// 現フレームでボタンが押されたか
		bool keyTrgUp = false;	// 現フレームでボタンが離されたか
	};
	using MouseInfo = Info;

	// パッド情報の数（KEY_PAD1 〜 PAD4）
	static constexpr int PAD_NUM = 5;

	// シングルトン用インスタンス
	static InputManager* instance_;

	// キー情報
	std::map<int, Info> keyInfos_;
	Info infoEmpty_;

	// マウス情報
	std::map<int, MouseInfo> mouseInfos_;
	MouseInfo mouseInfoEmpty_;

	// マウスカーソルの位置
	Vector2 mousePos_;

	// マウスボタンの入力状態
	int mouseInput_ = 0;

	// パッド情報
	std::array<JOYPAD_IN_STATE, PAD_NUM> padInfos_{};

	InputManager(void) = default;
	~InputManager(void) = default;

	// コピー・ムーブ禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// 判定を行うマウスボタンを追加
	void AddMouse(int button);

	// 押下状態から前フレーム・トリガーを更新
	static void UpdateInfo(Info& info, bool isPressed);

	// 配列の中からキー情報を取得する
	const Info& Find(int key) const;

	// 配列の中からマウス情報を取得する
	const MouseInfo& FindMouse(int key) const;

	// パッド情報を取得する（範囲外なら nullptr）
	const JOYPAD_IN_STATE* FindPad(JOYPAD_NO no) const;

	// 接続されたコントローラの種別を取得する
	JOYPAD_TYPE GetJPadType(JOYPAD_NO no) const;

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no) const;

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no) const;

	// コントローラの入力情報を更新する
	void SetJPadInState(JOYPAD_NO jpNo);
};