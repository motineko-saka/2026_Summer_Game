#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
#include "Common/FpsController.h"
#include "Application.h"
#include "Manager/EffekseerEffect.h"
#include "Audio/AudioManager.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_CSV = "Data/Csv/";
const std::string Application::PATH_MOV = "Data/Movie/";
const std::string Application::PATH_SHADER = "Data/Shader/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText("AGS");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	ChangeWindowMode(true);

	// FPS制御初期化
	fpsController_ = new FpsController(FRAME_RATE);

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	// Effekseerの初期化（低レベル）
	InitEffekseer();

	// EffekseerEffect（ラッパー）の生成・初期化
	EffekseerEffect::CreateInstance();
	if (EffekseerEffect::GetInstance()) {
		EffekseerEffect::GetInstance()->Init();
	}

	// 3Dオブジェクトの初期化
	Init3D();

	// 乱数のシード値を設定する
	DATEDATA date;

	// 現在時刻を取得する
	GetDateTime(&date);

	// 乱数の初期値を設定する
	// 設定する数値によって、ランダムの出方が変わる
	SRand(date.Year + date.Mon + date.Day + date.Hour + date.Min + date.Sec);

	// 入力制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	// キー登録等の初期化
	InputManager::GetInstance()->Init();

	// リソース管理初期化
	ResourceManager::CreateInstance();

	// サウンド管理初期化
	AudioManager::CreateInstance();
	if (AudioManager::GetInstance())
	{
		AudioManager::GetInstance()->Init();
	}

	// シーン管理初期化
	SceneManager::CreateInstance();
	SceneManager::GetInstance()->Init();
}

void Application::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(
		BACKGROUND_COLOR_R,
		BACKGROUND_COLOR_G,
		BACKGROUND_COLOR_B);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// ライトの設定
	ChangeLightTypeDir({ 0.3f, -0.7f, 0.8f });

	// フォグ設定
	SetFogEnable(true);
	SetFogColor(5, 5, 5);
	SetFogStartEnd(10000.0f, 20000.0f);
}

void Application::Run(void)
{
	// マウスの非表示
	SetMouseDispFlag(false);

	// SceneManager を先に取得して安全確認を行う
	SceneManager* sceneMgr = SceneManager::GetInstance();
	if (sceneMgr == nullptr)
	{
		OutputDebugStringA("Error: SceneManager::GetInstance() returned nullptr in Application::Run (before loop)\n");
		return;
	}

	// ゲームループ
	while (ProcessMessage() == 0 && !sceneMgr->GetGameEnd())
	{
		InputManager::GetInstance()->Update();
		sceneMgr->Update();

		// Effekseer の毎フレーム更新（必須）
		if (EffekseerEffect::GetInstance()) {
			EffekseerEffect::GetInstance()->Update();
		}

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		sceneMgr->Draw();

		// Effekseer の描画（Scene の描画後に呼ぶ）
		//if (EffekseerEffect::GetInstance()) {
		//	EffekseerEffect::GetInstance()->Draw();
		//}

#ifdef _DEBUG
		// 平均FPS描画
		fpsController_->Draw();
#endif // _DEBUG

		ScreenFlip();

		// 理想FPS経過待ち
		fpsController_->Wait();
	}

}

void Application::Destroy(void)
{
	// 入力制御の開放
	InputManager::DeleteInstance();

	// FPS制御メモリ解放
	delete fpsController_;

	// シーン管理解放
	SceneManager::GetInstance()->Release();
	SceneManager::DeleteInstance();

	ResourceManager::GetInstance().Destroy();

	// EffekseerEffect の解放（内部で Effkseer_End を呼ぶ）
	if (EffekseerEffect::GetInstance()) {
		EffekseerEffect::GetInstance()->Release();
		EffekseerEffect::DeleteInstance();
	}

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	// インスタンスのメモリ解放
	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

Application::Application(void)
	:
	isInitFail_(false),
	isReleaseFail_(false),
	fpsController_(nullptr)
{
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
