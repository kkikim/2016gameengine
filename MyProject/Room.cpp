#include "Room.h"


CRoom::CRoom(Ogre::Vector3 mPosition) : mPosition(mPosition)
{
}


CRoom::~CRoom()
{
}

void CRoom::createObjects(Ogre::SceneManager* mSceneMgr)
{
	mObJectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition);
	mObjectEntity = mSceneMgr->createEntity("room.mesh");
	mObJectNode->attachObject(mObjectEntity);
	mObJectNode->scale(50, 25, 50);
}
