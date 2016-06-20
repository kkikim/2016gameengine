#include "Ninja.h"

CNinja::CNinja(){}
CNinja::CNinja(Ogre::Vector3 mPosition) : mPosition(mPosition)
{
}


CNinja::~CNinja()
{
}
void CNinja::createObjects(Ogre::SceneManager* mSceneMgr)
{
	mNinjaDirection = Ogre::Vector3::ZERO;

	mObJectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mPosition);
	mObjectEntity = mSceneMgr->createEntity("ninja.mesh");
	mObJectNode->attachObject(mObjectEntity);
	
	mObJectNode->setScale(0.7, 0.7, 0.7);

	mAnimationState = mObjectEntity->getAnimationState("Idle1");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);

	mAnimationState2 = mObjectEntity->getAnimationState("Walk");
	mAnimationState2->setLoop(false);
	mAnimationState2->setEnabled(true);

	mAnimationState3 = mObjectEntity->getAnimationState("Kick");
	mAnimationState3->setLoop(false);
	mAnimationState3->setEnabled(true);

	mObJectNode->translate(0, -5, 0);

	tracing = false;

}