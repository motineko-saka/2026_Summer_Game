#pragma once
#include "ObjectBase.h"

class Object : public ObjectBase
{
public:
	//using ObjectBase::ObjectBase;
	virtual ~Object() override = default;
};