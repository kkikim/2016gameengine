#include "Trigger.h"


CTrigger::CTrigger(Ogre::Vector3 mPosition, int roomInfo) : mPosition(mPosition), goRoomInfo(roomInfo)
{
}


CTrigger::~CTrigger()
{
}

void CTrigger::createObjects(Ogre::SceneManager* mSceneMgr)
{
	mObJectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition);
	mObjectEntity = mSceneMgr->createEntity("TriggerBox.mesh");
	mObJectNode->attachObject(mObjectEntity);
	mObJectNode->scale(10, 10, 5);
}
