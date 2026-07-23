#pragma once
#include "ObjectBase.h"
#include "Board.h"

class Board;

class Panel : public ObjectBase
{
public:
    Panel(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

    void SetIndex(int x, int y)
    {
        indexX_ = x;
        indexY_ = y;
    }

    void SetBoard(Board* board) { board_ = board; }

    bool IsPushButton() const { return isButtonPushed_; }

    Board::ELEMENT GetElement() const;

    bool IsHitPlayer() { return true; }

private:
    void InitLoad() override;
    void InitObjCol() override;
    void ObjectUpdateProcess() override;
    void SetFlame(const Transform* follow) override { handFrame_ = MV1SearchFrame(follow->modelId, "mixamorig:LeftHand"); }

private:
    int indexX_ = 0;
    int indexY_ = 0;

    bool stepLock_ = false;
    bool isButtonPushed_ = false;

    Board* board_ = nullptr;
};