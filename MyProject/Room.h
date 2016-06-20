#pragma once

#include "GameState.h"
class CRoom
{
public:
	CRoom(Ogre::Vector3 mPosition);
	~CRoom();

	void createObjects(Ogre::SceneManager* mSceneMgr);
	Ogre::SceneNode* mObJectNode;
	Ogre::Entity* mObjectEntity;

	Ogre::Vector3 mPosition;
	Ogre::Vector3 mDirection;
};

