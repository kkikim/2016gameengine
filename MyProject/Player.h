#pragma once

#include "GameState.h"

class CPlayer
{
private:

public:
	CPlayer();
	CPlayer(Ogre::Vector3 mPosition);
	~CPlayer();

	void createObjects(Ogre::SceneManager* mSceneMgr);
	bool collideTimer(Ogre::Vector3 objectPosition);
	bool collideRoom(Ogre::Vector3 objectPosition);
	bool collideTrigger(Ogre::Vector3 objectPosition);
	void playerMoving();
	Ogre::SceneNode* mCharacterRoot;
	Ogre::SceneNode* mCharacterYaw;
	Ogre::Entity* mCharacterEntity;

	Ogre::Vector3 mPosition;
	Ogre::Vector3 mCharacterDirection;
	Ogre::AnimationState* mAnimationState;
	Ogre::AnimationState* mAnimationState2;
	Ogre::AnimationState* mAnimationState3;

	Ogre::AnimationState* mOgreDance;

	int mState;
	int mCollisionStateOfTimer = 0;
	int mCollisionStateOfRoom = 0;
	int mRoomNumber = 0;
	int mNumOfFish = 10;
	int mHP = 1000;

	enum { IdleState = 0, WalkState, JumpingState, SmallJump };
};

