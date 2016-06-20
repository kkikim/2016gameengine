#pragma once

#include "GameState.h"
#include "Player.h"
#include "Room.h"
#include "Timer.h"
#include "Fish.h"
#include "Trigger.h"
#include "Ninja.h"
#include <vector>
#include <chrono>
#include <fmod.h>
#include <fmod.hpp>

using namespace FMOD;

class PlayState : public GameState
{
public:
	void enter(void);
	void exit(void);

	void pause(void);
	void resume(void);

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e);
	bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager* game, const OIS::KeyEvent &e);

	static PlayState* getInstance() { return &mPlayState; }

	void soundInit();
	void Release();

private:

	void _setLights(void);
	void _drawGroundPlane(void);
	void _drawGridPlane(void);
	void drawSkyBox();
	void setTriggerPosition();
	void setTimerPosition();
	void setRoomPosition();
	void setNinjaPosition();
	static PlayState mPlayState;

	Ogre::Root *mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;

	Ogre::Light *mLightP, *mLightD, *mLightS;
	Ogre::Light *mLight1, *mLight2, *mLight3;

	Ogre::SceneNode* mCharacterRoot;
	Ogre::SceneNode* mCharacterYaw;
	Ogre::SceneNode* mCameraHolder;
	Ogre::SceneNode* mCameraYaw;
	Ogre::SceneNode* mCameraPitch;

	Ogre::Entity* mCharacterEntity;

	Ogre::Vector3 mCharacterDirection;

	Ogre::AnimationState* mAnimationState;
	CPlayer *player;
	std::vector<CNinja*> ninjaObject;
	
	std::vector<CRoom*> RoomObject;
	std::vector<CFish*> FishObject;
	std::vector<CTimer*> TimerObject;
	std::vector<CTrigger*> TriggerObject;
	std::vector<CNinja*> NinjaObject;

	Ogre::Real mWalkSpeed = 0.3f;

	Ogre::Overlay*           mInformationOverlay;

	std::chrono::system_clock::time_point start;

	float remainStageTime;
	float remainStageTime2;
	int numOfTimer;

	// 사운드  전역변수 3개를 선언해
	FMOD_SYSTEM *g_System; //사운드 시스템 생성 하는 부분이고
	FMOD_SOUND *g_Sound[2]; //사운드 설정하는부분
	FMOD_CHANNEL *g_Channel[2]; //채널 설정하는부분이고
	FMOD_BOOL mIsPlaying;
	
};
