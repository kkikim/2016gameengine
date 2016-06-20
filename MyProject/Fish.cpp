#include "Fish.h"
#include "OgreMath.h"

CFish::CFish(Ogre::Vector3 mPosition) : mPosition(mPosition)
{
}


CFish::~CFish()
{
}

void CFish::createObjects(Ogre::SceneManager* mSceneMgr)
{
	mObJectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition);
	mObjectEntity = mSceneMgr->createEntity("fish.mesh");
	mObJectNode->attachObject(mObjectEntity);
	mObJectNode->scale(4, 4, 4);
	//mObJectNode->scale(8, 8, 8);
	mObJectNode->pitch(Ogre::Radian(90.0f));
}
