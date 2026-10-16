#include <iterator>
#include <string>
#include "EffekseerEffect.h"
#include "../Application.h"

EffekseerEffect* EffekseerEffect::instance_ = nullptr;

namespace
{
	// エフェクトごとの設定
	struct EffectInfo
	{
		const char* file;	// ファイル名
		float scale;		// 拡大率
		float speed;		// 再生速度
	};

	// EFFECT の並び順と一致させること
	constexpr EffectInfo EFFECT_INFOS[] =
	{
		{ "slashu.efkefc",     1.0f,  1.0f },	// SLASH
		{ "nomotoaren.efkefc", 50.0f, 0.2f },	// TUTORIAL
		{ "Fiyaer.efkefc",     5.0f,  0.2f },	// FIRE
		{ "mizu.efkefc",       50.0f, 0.2f },	// WATER
		{ "kori.efkefc",       50.0f, 0.2f },	// ICE
		{ "bakuhatu.efkefc",   50.0f, 0.2f },	// EXPLOSION
		{ "comboLast.efkefc",  50.0f, 0.2f },	// COMBO_LAST
		{ "meteo.efkefc",      50.0f, 0.2f },	// METEOR
	};

	static_assert(
		std::size(EFFECT_INFOS) == static_cast<size_t>(EffekseerEffect::EFFECT::MAX),
		"EFFECT_INFOS と EFFECT の数が一致していません");

	// 読み込み失敗時のメッセージ
	void ShowLoadError(const std::string& path)
	{
		std::wstring msg = L"エフェクトの読み込みに失敗しました。パスやファイル名を確認してください。\n";
		msg += std::wstring(path.begin(), path.end());
		MessageBoxW(nullptr, msg.c_str(), L"エラー", MB_OK);
	}
}

EffekseerEffect::EffekseerEffect(void)
{
	resourceIds_.fill(-1);
	playHandles_.fill(-1);
}

void EffekseerEffect::Init(void)
{
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (Effekseer_Init(MAX_PARTICLE) == -1)
	{
		DxLib_End();
		return;
	}

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// 全エフェクトを読み込む
	for (int i = 0; i < EFFECT_NUM; i++)
	{
		const std::string path = Application::PATH_EFFECT + EFFECT_INFOS[i].file;
		resourceIds_[i] = LoadEffekseerEffect(path.c_str());

		if (resourceIds_[i] == -1)
		{
			ShowLoadError(path);
		}
	}
}

void EffekseerEffect::Update(void)
{
	UpdateEffekseer3D();
}

void EffekseerEffect::Draw(void)
{
	DrawEffekseer3D();
}

void EffekseerEffect::Release(void)
{
	// 再生中のものを止める
	for (int i = 0; i < EFFECT_NUM; i++)
	{
		Stop(static_cast<EFFECT>(i));
	}

	// 読み込んだエフェクトを削除
	for (int& id : resourceIds_)
	{
		if (id != -1) DeleteEffekseerEffect(id);
		id = -1;
	}

	// ※ Effekseer側の関数名が Effkseer_End（綴りはこれで正しい）
	Effkseer_End();
}

void EffekseerEffect::Delete(void)
{
	Stop(EFFECT::TUTORIAL);
}

void EffekseerEffect::Play(EFFECT type, const VECTOR& pos, float rotY)
{
	const int idx = static_cast<int>(type);
	if (idx < 0 || idx >= EFFECT_NUM) return;
	if (resourceIds_[idx] == -1) return;

	const EffectInfo& info = EFFECT_INFOS[idx];
	const int handle = PlayEffekseer3DEffect(resourceIds_[idx]);
	playHandles_[idx] = handle;

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y, pos.z);
	SetRotationPlayingEffekseer3DEffect(handle, 0.0f, rotY, 0.0f);
	SetScalePlayingEffekseer3DEffect(handle, info.scale, info.scale, info.scale);
	SetSpeedPlayingEffekseer3DEffect(handle, info.speed);
}

void EffekseerEffect::Stop(EFFECT type)
{
	const int idx = static_cast<int>(type);
	if (idx < 0 || idx >= EFFECT_NUM) return;

	int& handle = playHandles_[idx];
	if (handle != -1) StopEffekseer3DEffect(handle);
	handle = -1;
}