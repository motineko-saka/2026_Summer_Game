#pragma once

#include <DxLib.h>
#include <string>
#include <vector>

class TextManager
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new TextManager(); } }
	static TextManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
	// 静的インスタンス
	static TextManager* instance_;

	TextManager();		// コンストラクタ
	~TextManager();	// デストラクタ

	// コピー・ムーブ操作を禁止
	TextManager(const TextManager&) = delete;
	TextManager& operator=(const TextManager&) = delete;
	TextManager(TextManager&&) = delete;
	TextManager& operator=(TextManager&&) = delete;

public:
	// 初期化（デフォルトテキストをロード）
	void Init();

	// ファイルから読み込む（成功/失敗を返す）
	bool LoadFromFile(const std::string& path);

	// 現在参照している生バイト（1バイト）を取得（制御文字チェック用）
	char CurrentByte() const;

	// 現在位置の「2バイト文字」を out にコピー（終端を考慮して '\0' で終了）
	void GetCurrentTwoByte(char out[3]) const;

	// バイト単位で進める（デフォルト 1）。行末を越えたら次行へ自動移動
	void AdvanceBytes(int n = 1);

	// 末尾到達判定（すべての行を消費したか）
	bool IsAllTextConsumed() const;

	// リセット（先頭に戻す）
	void Reset();

private:

	// 内部ヘルパ
	void LoadDefaultText();

	// テキスト本体（各行はバイト列として扱う）
	std::vector<std::string> texts_;
	size_t sp_; // 行インデックス
	size_t cp_; // 行内バイトオフセット
};