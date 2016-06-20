#include "Timer.h"


CTimer::CTimer(Ogre::Vector3 mPosition) : mPosition(mPosition)
{
}


CTimer::~CTimer()
{
}

void CTimer::createObjects(Ogre::SceneManager* mSceneMgr)
{
	mObJectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition);
	mObjectEntity = mSceneMgr->createEntity("Timer.mesh");
	mObJectNode->attachObject(mObjectEntity);
	//mObJectNode->translate(0,-35,0);
	mObJectNode->scale(0.2, 0.2, 0.2);
}
