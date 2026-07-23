#pragma once
#include <DxLib.h>
#include <array>

class Panel;

class Board
{
public:
    static constexpr int STAGE_SIZE = 3;
    static constexpr float PANEL_3D_SIZE = 80.0f;
    static constexpr float GAP = 2.0f;

    enum ELEMENT
    {
        WATER = 0,
        ICE,
        FIRE,
        ELEMENT_COUNT
    };

    Board();
    ~Board();

    // パネルの初期化と管理
    void Initialize(const std::array<std::array<ELEMENT, STAGE_SIZE>, STAGE_SIZE>& initialBoard);
    void Update();
    void Draw() const;

    // パネルの状態管理
    ELEMENT GetElement(int x, int y) const;
    void SetElement(int x, int y, ELEMENT element);
    void ChangeElement(int x, int y);
    void PushPanel(int x, int y);

    // クリア判定
    bool CheckClear() const;

    // ユーティリティ
    VECTOR GetPanelCenterPos(int x, int y) const;
    unsigned int GetElementColor(ELEMENT element) const;

private:
    void DrawFieldGrid() const;
    void Draw3DPanel(int x, int y, unsigned int color) const;

private:
    std::array<std::array<ELEMENT, STAGE_SIZE>, STAGE_SIZE> board_;
    std::array<std::array<bool, STAGE_SIZE>, STAGE_SIZE> panelStepLock_;
    VECTOR position_ = VGet(100.0f, 0.0f, 0.0f);
    bool isCleared_ = false;
    float chestAnimY_ = -50.0f;
};