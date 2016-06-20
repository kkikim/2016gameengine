#pragma once

#include "GameState.h"
class CTrigger
{
public:
	CTrigger(Ogre::Vector3 mPosition,int);
	~CTrigger();

	void createObjects(Ogre::SceneManager* mSceneMgr);
	Ogre::SceneNode* mObJectNode;
	Ogre::Entity* mObjectEntity;

	Ogre::Vector3 mPosition;
	int goRoomInfo;
};

