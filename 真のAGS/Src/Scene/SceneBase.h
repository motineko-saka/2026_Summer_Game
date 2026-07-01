#pragma once
#include <memory>
#include <vector>
class ResourceManager;
class SceneManager;
class StageManager;
class Wall;

class SceneBase
{

public:

	// 世界
	enum class WORLD
	{
		LEFT = 0,
		RIGHT,
		ANSWER,
	};

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void) = 0;

	// 初期化処理
	virtual void Init(void) = 0;

	// 読み込み
	virtual void Load(void) = 0;

	// 読み込み後の初期化
	virtual void LoadEnd(void) = 0;

	// 更新ステップ
	virtual void Update(void) = 0;

	// 描画処理
	virtual void Draw(void) = 0;

	// 解放処理
	virtual void Release(void) = 0;

protected:

	// リソース管理
	ResourceManager& resMng_;

	std::vector<std::unique_ptr<Wall>> walls_;

	virtual void CreateWall(StageManager& stageM);
};
