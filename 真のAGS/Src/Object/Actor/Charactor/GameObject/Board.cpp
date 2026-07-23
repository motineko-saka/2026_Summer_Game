#include "Board.h"
#include <math.h>

Board::Board() 
    : 
    isCleared_(false),
    chestAnimY_(-50.0f)
{
    // パネルステップロックを初期化
    for (auto& row : panelStepLock_)
    {
        for (auto& lock : row)
        {
            lock = false;
        }
    }
}

Board::~Board()
{
}

void Board::Initialize(const std::array<std::array<ELEMENT, STAGE_SIZE>, STAGE_SIZE>& initialBoard)
{
    board_ = initialBoard;
    chestAnimY_ = -50.0f;
    isCleared_ = false;
}

void Board::Update()
{
    // クリア状態でアニメーション処理
    if (isCleared_ && chestAnimY_ < 10.0f)
    {
        chestAnimY_ += 1.0f;
    }
}

void Board::Draw() const
{
    // 背景のグリッド床を描画
    //DrawFieldGrid();

    // パズルパネルの描画
    for (int y = 0; y < STAGE_SIZE; ++y)
    {
        for (int x = 0; x < STAGE_SIZE; ++x)
        {
            Draw3DPanel(x, y, GetElementColor(board_[y][x]));
        }
    }

    // クリアしたら中央から「宝箱」がせり上がってくる
    if (isCleared_)
    {
        DrawCube3D(
            VGet(-10.0f, chestAnimY_ + 10.0f, 10.0f),
            VGet(10.0f, chestAnimY_ - 10.0f, -10.0f),
            GetColor(139, 69, 19),
            GetColor(255, 215, 0),
            TRUE
        );
    }
}

void Board::DrawFieldGrid() const
{
    unsigned int gridColor = GetColor(80, 140, 180);
    for (float i = -300.0f; i <= 300.0f; i += 30.0f)
    {
        DrawLine3D(VGet(i, 0.0f, -300.0f), VGet(i, 0.0f, 300.0f), gridColor);
        DrawLine3D(VGet(-300.0f, 0.0f, i), VGet(300.0f, 0.0f, i), gridColor);
    }
}

void Board::Draw3DPanel(int x, int y, unsigned int color) const
{
    VECTOR center = GetPanelCenterPos(x, y);
    float hSize = PANEL_3D_SIZE / 2.0f;

    VECTOR v0 = VGet(center.x - hSize, center.y, center.z + hSize);
    VECTOR v1 = VGet(center.x + hSize, center.y, center.z + hSize);
    VECTOR v2 = VGet(center.x - hSize, center.y, center.z - hSize);
    VECTOR v3 = VGet(center.x + hSize, center.y, center.z - hSize);

    DrawTriangle3D(v0, v1, v2, color, TRUE);
    DrawTriangle3D(v1, v3, v2, color, TRUE);
}

Board::ELEMENT Board::GetElement(int x, int y) const
{
    if (x >= 0 && x < STAGE_SIZE && y >= 0 && y < STAGE_SIZE)
    {
        return board_[y][x];
    }
    return WATER;
}

void Board::SetElement(int x, int y, ELEMENT element)
{
    if (x >= 0 && x < STAGE_SIZE && y >= 0 && y < STAGE_SIZE)
    {
        board_[y][x] = element;
    }
}

void Board::ChangeElement(int x, int y)
{
    if (x >= 0 && x < STAGE_SIZE && y >= 0 && y < STAGE_SIZE)
    {
        board_[y][x] = static_cast<ELEMENT>((board_[y][x] + 1) % ELEMENT_COUNT);
    }
}

void Board::PushPanel(int x, int y)
{
    if (x < 0 || x >= STAGE_SIZE || y < 0 || y >= STAGE_SIZE)
    {
        return;
    }

    if (!panelStepLock_[y][x])
    {
        ChangeElement(x, y);
        ChangeElement(x, y - 1);
        ChangeElement(x, y + 1);
        ChangeElement(x - 1, y);
        ChangeElement(x + 1, y);

        isCleared_ = CheckClear();
        panelStepLock_[y][x] = true;
    }
}

bool Board::CheckClear() const
{
    for (int y = 0; y < STAGE_SIZE; ++y)
    {
        for (int x = 0; x < STAGE_SIZE; ++x)
        {
            if (board_[y][x] != WATER)
            {
                return false;
            }
        }
    }
    return true;
}

VECTOR Board::GetPanelCenterPos(int x, int y) const
{
    float centerX = (x - 1) * (PANEL_3D_SIZE + GAP);
    float centerZ = -(y - 1) * (PANEL_3D_SIZE + GAP);
    return VGet(-1000.0f + centerX, -600.0f, 1000.0f + centerZ);
}

unsigned int Board::GetElementColor(ELEMENT element) const
{
    switch (element)
    {
    case WATER:
        return GetColor(0, 128, 255);
    case ICE:
        return GetColor(220, 240, 255);
    case FIRE:
        return GetColor(255, 64, 0);
    default:
        return GetColor(128, 128, 128);
    }
}