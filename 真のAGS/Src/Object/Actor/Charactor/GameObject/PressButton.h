#pragma once
#include "ObjectBase.h"
class PressButton :
    public ObjectBase
{
public:
    PressButton(SceneBase::WORLD world, VECTOR ansVec, OBJECT_TYPE type);

private:
    // リソースロード
    void InitLoad(void)override;

    void ObjectUpdateProcess(void)override;
};

