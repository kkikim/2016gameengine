#include "Player.h"

CPlayer::CPlayer(){}
CPlayer::CPlayer(Ogre::Vector3 mPosition) : mPosition(mPosition)
{
}


CPlayer::~CPlayer()
{
}
void CPlayer::createObjects(Ogre::SceneManager* mSceneMgr)
{
	mCharacterDirection = Ogre::Vector3::ZERO;
	mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
	mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw");
	mCharacterEntity = mSceneMgr->createEntity("Player", "Sinbad.mesh");
	mCharacterYaw->attachObject(mCharacterEntity);
	mCharacterEntity->setCastShadows(true);
	mAnimationState = mCharacterEntity->getAnimationState("IdleTop");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);

	mAnimationState2 = mCharacterEntity->getAnimationState("IdleBase");
	mAnimationState2->setLoop(true);
	mAnimationState2->setEnabled(true);

	mAnimationState3 = mCharacterEntity->getAnimationState("SliceVertical");
	mAnimationState3->setLoop(true);
	mAnimationState3->setEnabled(false);

	mOgreDance = mCharacterEntity->getAnimationState("Dance");
	mOgreDance->setLoop(false);
	mOgreDance->setEnabled(false);

	mCharacterRoot->translate(0, 40, 0);
	mCharacterRoot->scale(8, 8, 8);
	//mCharacterRoot->scale(30,30, 30);
}
///////////¹Ø¿¡²¨¾ä.//
bool CPlayer::collideTimer(Ogre::Vector3 objectPosition)
{

	if (mPosition.x + 25 < objectPosition.x - 50)
	{
		mCollisionStateOfTimer = 1;
		//mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, player->mCharacterRoot->getPosition().y, 399);
		return false;
	}
	if (mPosition.x - 25 > objectPosition.x  +50)
	{
		mCollisionStateOfTimer = 2;
		return false;
	}
	if (mPosition.z + 25 < objectPosition.z - 50)
	{
		mCollisionStateOfTimer = 3;
		//mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z-10);
		return false;
	}
	if (mPosition.z -25  >objectPosition.z + 50)
	{
		mCollisionStateOfTimer = 4;
		//mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z );
		return false;
	}
	if (mCollisionStateOfTimer == 1)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x-20, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
	if (mCollisionStateOfTimer == 2)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x+20, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
	if (mCollisionStateOfTimer == 3)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z - 20);
	if (mCollisionStateOfTimer == 4)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z + 20);

	return true;
}

bool CPlayer::collideRoom(Ogre::Vector3 objectPosition)
{
	std::cout << objectPosition << std::endl;
	if (mPosition.x + 25 > objectPosition.x +340)
	{
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x - 50, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
		return true;
	}
	if (mPosition.x - 25 < objectPosition.x - 340)
	{
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x + 50, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
		return true;
	}
	if (mPosition.z + 25 > objectPosition.z + 340)
	{
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z - 50);
		return true;
	}
	if (mPosition.z - 25  <objectPosition.z - 340)
	{
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z + 50);
		return true;
	}
	return false;
}

bool CPlayer::collideTrigger(Ogre::Vector3 objectPosition)
{

	if (mPosition.x + 25 < objectPosition.x - 50)
		return false;
	if (mPosition.x - 25 > objectPosition.x + 50)
		return false;
	if (mPosition.z + 25 < objectPosition.z - 50)
		return false;
	if (mPosition.z - 25  >objectPosition.z + 50)
		return false;

	if (mCollisionStateOfTimer == 1)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x - 20, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
	if (mCollisionStateOfTimer == 2)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x + 20, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
	if (mCollisionStateOfTimer == 3)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z - 20);
	if (mCollisionStateOfTimer == 4)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z + 20);

	return true;
}

void CPlayer::playerMoving()
{
}