#include "SceneTransition.h"
#include <DxLib.h>
#include <algorithm>
#include <random>

SceneTransition::SceneTransition()
{
    timer_ = 0;
    isPlaying_ = false;
    state_ = STATE::NONE;
}

SceneTransition::~SceneTransition()
{
}
void SceneTransition::Start()
{
    timer_ = 0;
    isPlaying_ = true;
    state_ = STATE::CLOSE;

    pieces_.clear();

    const int ROW = 4;
    const int COL = 4;

    const int pieceW = 1280 / COL;
    const int pieceH = 720 / ROW;

    for (int y = 0; y < ROW; y++)
    {
        for (int x = 0; x < COL; x++)
        {
            Piece piece;

            piece.rect.left = x * pieceW;
            piece.rect.top = y * pieceH;
            piece.rect.right = piece.rect.left + pieceW;
            piece.rect.bottom = piece.rect.top + pieceH;

            piece.visible = false;
            piece.frame = 0;

            pieces_.push_back(piece);
        }
    }

    std::random_device rd;
    std::mt19937 mt(rd());

    std::shuffle(pieces_.begin(), pieces_.end(), mt);
}

void SceneTransition::Open()
{
    timer_ = 0;
    isPlaying_ = true;
    state_ = STATE::OPEN;

    for (auto& piece : pieces_)
    {
        piece.visible = true;
    }
}

void SceneTransition::Update()
{
    if (!isPlaying_)
        return;

    timer_++;

    const int interval = 5;
    int index = timer_ / interval;

    if (state_ == STATE::CLOSE)
    {
        if (index < pieces_.size())
        {
            pieces_[index].visible = true;
        }

        if (index >= pieces_.size())
        {
            isPlaying_ = false;
        }
    }
    else if (state_ == STATE::OPEN)
    {
        if (index < pieces_.size())
        {
            pieces_[index].visible = false;
        }

        if (index >= pieces_.size())
        {
            isPlaying_ = false;
        }
    }
}

void SceneTransition::Draw()
{
    for (auto& piece : pieces_)
    {
        if (!piece.visible)
            continue;

        DrawBox(
            piece.rect.left,
            piece.rect.top,
            piece.rect.right,
            piece.rect.bottom,
            GetColor(0, 0, 0),
            true);
    }
}

bool SceneTransition::IsPlay() const
{
    return isPlaying_;
}

SceneTransition::STATE SceneTransition::GetState() const
{
    return state_;
}