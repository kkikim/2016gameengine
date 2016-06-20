#pragma once

#include "GameState.h"
class CFish
{
public:
	CFish(Ogre::Vector3 mPosition);
	~CFish();

	void createObjects(Ogre::SceneManager* mSceneMgr);
	Ogre::SceneNode* mObJectNode;
	Ogre::Entity* mObjectEntity;

	Ogre::Vector3 mPosition;
	Ogre::Vector3 mDirection;
};

