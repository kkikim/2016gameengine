#pragma once

#include "GameState.h"
class CTimer
{
public:
	CTimer(Ogre::Vector3 mPosition);
	~CTimer();

	void createObjects(Ogre::SceneManager* mSceneMgr);
	Ogre::SceneNode* mObJectNode;
	Ogre::Entity* mObjectEntity;

	Ogre::Vector3 mPosition;
	Ogre::Vector3 mDirection;
};

