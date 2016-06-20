#pragma once

#include "GameState.h"
class CNinja
{
private:

public:
	CNinja();
	CNinja(Ogre::Vector3 mPosition);
	~CNinja();

	void createObjects(Ogre::SceneManager* mSceneMgr);

	Ogre::SceneNode* mObJectNode;
	Ogre::Entity* mObjectEntity;
	Ogre::Vector3 mPosition;
	Ogre::Vector3 mNinjaDirection;

	Ogre::AnimationState* mAnimationState;
	Ogre::AnimationState* mAnimationState2;
	Ogre::AnimationState* mAnimationState3;
	Ogre::Quaternion mSrcQuat, mDestQuat;
	bool tracing;

	int mState;
};

