#pragma once
#include <string>
#include <vector>

class Resource
{
public:

	// リソースタイプ
	enum class TYPE
	{
		NONE,
		IMG,
		IMGS,
		MODEL,
		EFFEKSEER,
		MOVIE,
	};

	Resource(void) = default;
	Resource(TYPE type, const std::string& path);

	// IMGS用
	Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY);

	~Resource(void) = default;

	// 読み込み（読み込み済みなら何もしない）
	void Load(void);

	// 解放（何度呼んでも安全）
	void Release(void);

	// 読み込み済みか
	bool IsLoaded(void) const;

	// 分割画像ハンドルを別配列にコピー
	void CopyHandle(int* imgs) const;

	// リソースタイプ
	TYPE type_ = TYPE::NONE;

	// リソースの読み込み先
	std::string path_;

	// 画像とモデルのハンドルID
	int handleId_ = -1;

	// IMGS::LoadDivGraph用
	std::vector<int> handleIds_;
	int numX_ = -1;
	int numY_ = -1;
	int sizeX_ = -1;
	int sizeY_ = -1;

	// モデル複製用
	std::vector<int> duplicateModelIds_;
};