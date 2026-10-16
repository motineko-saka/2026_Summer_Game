#pragma once

#include <memory>
#include "../../../../Scene/GameScene.h"
#include "../../../../Scene/SceneBase.h"

class ObjectBase;

class ObjectFactory
{
public:
	struct ObjectInfo
	{
		GameScene::WORLD        world;
		VECTOR                  answerPos;
		ObjectBase::OBJECT_TYPE type;
		VECTOR                  pos;
		VECTOR                  scale;
	};

	ObjectFactory();
	~ObjectFactory();

	template<class objectClass>
	std::unique_ptr<objectClass> Create(SceneBase::WORLD world, const VECTOR& answer, ObjectBase::OBJECT_TYPE type, const VECTOR& pos, const VECTOR& scl)
	{
		std::unique_ptr<objectClass> object = std::make_unique<objectClass>(world, answer, type);
		object->Init();
		object->SetPosition(pos);
		object->SetScale(scl);
		return std::move(object);
	}
};

