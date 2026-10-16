#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <algorithm>
#include "Resource.h"

Resource::Resource(TYPE type, const std::string& path)
	:
	type_(type),
	path_(path)
{
}

Resource::Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY)
	:
	type_(type),
	path_(path),
	numX_(numX),
	numY_(numY),
	sizeX_(sizeX),
	sizeY_(sizeY)
{
}

bool Resource::IsLoaded(void) const
{
	return handleId_ != -1 || !handleIds_.empty();
}

void Resource::Load(void)
{
	// 二重読み込みによるリークを防ぐ
	if (IsLoaded()) return;

	switch (type_)
	{
	case TYPE::IMG:
	case TYPE::MOVIE:
		handleId_ = LoadGraph(path_.c_str());
		break;

	case TYPE::IMGS:
	{
		const int num = numX_ * numY_;
		if (num <= 0) break;

		handleIds_.assign(num, -1);
		LoadDivGraph(path_.c_str(), num, numX_, numY_, sizeX_, sizeY_, handleIds_.data());
		break;
	}

	case TYPE::MODEL:
		handleId_ = MV1LoadModel(path_.c_str());
		break;

	case TYPE::EFFEKSEER:
		handleId_ = LoadEffekseerEffect(path_.c_str());
		break;

	default:
		break;
	}
}

void Resource::Release(void)
{
	switch (type_)
	{
	case TYPE::IMG:
	case TYPE::MOVIE:
		if (handleId_ != -1) DeleteGraph(handleId_);
		break;

	case TYPE::IMGS:
		for (int handle : handleIds_)
		{
			DeleteGraph(handle);
		}
		break;

	case TYPE::MODEL:
		for (int id : duplicateModelIds_)
		{
			MV1DeleteModel(id);
		}
		if (handleId_ != -1) MV1DeleteModel(handleId_);
		break;

	case TYPE::EFFEKSEER:
		if (handleId_ != -1) DeleteEffekseerEffect(handleId_);
		break;

	default:
		break;
	}

	// 解放済みの状態に戻す（二重解放防止）
	handleId_ = -1;
	handleIds_.clear();
	duplicateModelIds_.clear();
}

void Resource::CopyHandle(int* imgs) const
{
	if (imgs == nullptr) return;
	std::copy(handleIds_.begin(), handleIds_.end(), imgs);
}