#include "PlayState.h"
#include "TitleState.h"
#include "OptionState.h"
#include "GameOverState.h"
#include "Player.h"
#include "Room.h"
#include "Timer.h"
#include "Ninja.h"

using namespace Ogre;

PlayState PlayState::mPlayState;

void PlayState::enter(void)
{
  mRoot = Root::getSingletonPtr(); 
  mRoot->getAutoCreatedWindow()->resetStatistics();

  player = new CPlayer(Vector3(0,150,0));
 
  mSceneMgr = mRoot->getSceneManager("main");
  mCamera = mSceneMgr->getCamera("main");
  mCamera->setPosition(Ogre::Vector3::ZERO);
  mCamera->setFarClipDistance(1000);
  mCamera->setNearClipDistance(30);
  player->createObjects(mSceneMgr);

  //_drawGridPlane();

  _setLights();
 
  drawSkyBox();
  
  mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
  mInformationOverlay->show(); 
  
  mCameraYaw = player->mCharacterRoot->createChildSceneNode("CameraYaw", Vector3(0.0f, 30.0f, 0.0f));
  mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
  mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, -25.0f, 30.0f));
  mCameraHolder->attachObject(mCamera);
  mCamera->lookAt(mCameraYaw->getPosition());
  mCameraYaw->setInheritOrientation(false);

  setRoomPosition();
  setNinjaPosition();
  setTimerPosition();
  setTriggerPosition();
  _drawGroundPlane();
  start = std::chrono::system_clock::now();
  remainStageTime = 100.0;
  remainStageTime2 = 10;

  // Sound
  soundInit();
  FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[0], 0, &g_Channel[0]);
}

void PlayState::exit(void)
{
  // Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();
	Release();
  // ---------------------------------------
}
void PlayState::pause(void)
{
}

void PlayState::resume(void)
{}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	if (player->mHP < 0)
	{
		game->changeState(GameOverState::getInstance());
		return true;
	}
	if (remainStageTime < 0)
	{
		game->changeState(GameOverState::getInstance());
		return true;
	}
	if (remainStageTime2<0)
	{
		game->changeState(GameOverState::getInstance());
		return true;
	}

	if (player->mRoomNumber != 19)
		remainStageTime -= evt.timeSinceLastFrame;
	if (player->mRoomNumber == 19)
	{
		remainStageTime2 -= evt.timeSinceLastFrame;
		player->mAnimationState->setEnabled(false);
		player->mAnimationState->setLoop(false);
		player->mAnimationState2->setEnabled(false);
		player->mAnimationState2->setLoop(false);
		player->mAnimationState3->setEnabled(false);
		player->mAnimationState3->setLoop(false);

		player->mOgreDance->setEnabled(true);
		player->mOgreDance->setLoop(true);
		player->mOgreDance->addTime(evt.timeSinceLastFrame);
	}

	//캐릭터 이동. 및 충돌.
  // Fill Here -------------------------------------------------------------------
	if (player->mCharacterDirection != Vector3::ZERO)
	{
		player->mCharacterRoot->setOrientation(mCameraYaw->getOrientation());
		Quaternion quat = Vector3(Vector3::UNIT_Z).getRotationTo(player->mCharacterDirection);
		player->mCharacterYaw->setOrientation(quat);

		if (!(player->collideRoom(RoomObject[player->mRoomNumber]->mPosition)))// 방하고 충돌체크
		{
			//아이템과의 충돌체크
			for (auto d : TimerObject)
			{
				if (!(player->collideTimer(d->mPosition)))
				{
					player->mCharacterRoot->translate(player->mCharacterDirection.normalisedCopy() * 50 * evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);
				}
				else if ((player->collideTimer(d->mPosition)))	//타이머와의 충돌후처리
				{
					FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[1], 0, &g_Channel[1]);

					d->mObJectNode->setVisible(false);
					d->mObjectEntity->setVisible(false);
					d->mPosition = Vector3(-1500,1000,-1500);
					remainStageTime += 30;
					
				}
			}
			for (auto T : TriggerObject)		//트리거와 충돌.
			{
				if (T->goRoomInfo>-1)
				{
					if (player->collideTrigger(T->mPosition))
					{
						player->mCharacterRoot->setPosition(RoomObject[T->goRoomInfo]->mPosition.x, 40, RoomObject[T->goRoomInfo]->mPosition.z);
						player->mRoomNumber = T->goRoomInfo;
					}
				}
			}
		}
		
		player->mPosition = player->mCharacterRoot->getPosition();
		player->mAnimationState = player->mCharacterEntity->getAnimationState("RunBase");
		player->mAnimationState->setLoop(true);
		player->mAnimationState->setEnabled(true);

		player->mAnimationState2 = player->mCharacterEntity->getAnimationState("RunTop");
		player->mAnimationState2->setLoop(true);
		player->mAnimationState2->setEnabled(true);
	}
	else
	{
		player->mAnimationState->setEnabled(false);
		player->mAnimationState = player->mCharacterEntity->getAnimationState("IdleTop");
		player->mAnimationState->setLoop(true);
		player->mAnimationState->setEnabled(true);

		player->mAnimationState2->setEnabled(false);
		player->mAnimationState2 = player->mCharacterEntity->getAnimationState("IdleBase");
		player->mAnimationState2->setLoop(true);
		player->mAnimationState2->setEnabled(true);

		player->mPosition = player->mCharacterRoot->getPosition();
	}
	player->mAnimationState->addTime(evt.timeSinceLastFrame);
	player->mAnimationState2->addTime(evt.timeSinceLastFrame);
	player->mAnimationState3->addTime(evt.timeSinceLastFrame);

  // -----------------------------------------------------------------------------

	//닌자관련
	Real move = mWalkSpeed * evt.timeSinceLastFrame;
	for (auto d : ninjaObject)
	{
		d->mNinjaDirection = player->mPosition - d->mObJectNode->getPosition();
		d->mNinjaDirection.y = 0;
		Quaternion quat = Vector3(-Vector3::UNIT_Z).getRotationTo(d->mNinjaDirection);
		

		d->mAnimationState->addTime(evt.timeSinceLastFrame);
		d->mAnimationState2->addTime(evt.timeSinceLastFrame);
		d->mAnimationState3->addTime(evt.timeSinceLastFrame);
		d->mObJectNode->setOrientation(quat);
		if (d->mPosition.distance(player->mPosition) >150 && d->mPosition.distance(player->mPosition)<300)
		{
			d->tracing = true;
			
			d->mAnimationState->setLoop(false);
			d->mAnimationState->setEnabled(false);

			d->mAnimationState2->setLoop(true);
			d->mAnimationState2->setEnabled(true);

			d->mAnimationState3->setLoop(false);
			d->mAnimationState3->setEnabled(false); 
		}
		else if (d->mPosition.distance(player->mPosition)>200)
		{
			d->tracing = false;

			d->mAnimationState->setLoop(true);
			d->mAnimationState->setEnabled(true);

			d->mAnimationState2->setLoop(false);
			d->mAnimationState2->setEnabled(false);

			d->mAnimationState3->setLoop(false);
			d->mAnimationState3->setEnabled(false);
		}
		else if (d->mPosition.distance(player->mPosition) < 150)
		{
			d->tracing = false;
			d->mAnimationState->setLoop(false);
			d->mAnimationState->setEnabled(false);

			d->mAnimationState2->setLoop(false);
			d->mAnimationState2->setEnabled(false);

			d->mAnimationState3->setLoop(true);
			d->mAnimationState3->setEnabled(true);
			player->mHP -= 1;
		}

		if (d->tracing)
		{
			d->mObJectNode->setOrientation(quat);
			d->mObJectNode->translate(d->mNinjaDirection*move);
		}

		d->mPosition = d->mObJectNode->getPosition();
	}
		
	//모래시계 아이템
	for (auto d : TimerObject)
		d->mObJectNode->yaw(Degree(0.3)); 

	return true;
}

void PlayState::soundInit()
{

	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 10, FMOD_INIT_NORMAL, NULL);

	//배경음
	FMOD_System_CreateStream(g_System, "Background.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[0]);
	FMOD_System_CreateSound(g_System, "coin.wav", FMOD_DEFAULT, 0, &g_Sound[1]);

	// 형식은 :                g_system에, 경로, 재생 방식(계속 음악끝나면 반복), 사운드 설정 해주고


}


void PlayState::Release() //마지막으로 음악 다 재생 되면 이런식으로 릴리즈해줘야 해.
{
	for (int i = 0; i < 2; ++i)
	{
		FMOD_Sound_Release(g_Sound[i]);
	}
	/*FMOD_Sound_Release(g_Sound[0]);
	FMOD_Sound_Release(g_Sound[1]);*/
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}


bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
  static Ogre::DisplayString currFps = L"현재 FPS: "; 
  static Ogre::DisplayString avgFps = L"평균 FPS: ";
  static Ogre::DisplayString bestFps = L"최고 FPS: ";
  static Ogre::DisplayString worstFps = L"남은 시간 : ";

  OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
  OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
  OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
  OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");


  const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

  guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
  guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
  guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
  guiWorst->setCaption(worstFps + StringConverter::toString(remainStageTime));

  return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch (e.key)
	{
	case OIS::KC_W: case OIS::KC_UP:
	{
		player->mCharacterDirection.z = 0.0f;
		break;
	}
	case OIS::KC_S: case OIS::KC_DOWN:
		player->mCharacterDirection.z =0.0f;
		break;
	case OIS::KC_A: case OIS::KC_LEFT:
		player->mCharacterDirection.x = 0.0f;
		break;
	case OIS::KC_D: case OIS::KC_RIGHT:
		player->mCharacterDirection.x =0.0f;
		break;
	}

  return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
  // Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_W: case OIS::KC_UP:
	{
		player->mCharacterDirection.z = -1.5f;

		std::cout << player->mCharacterRoot->getPosition() << std::endl;
		std::cout << player->mPosition << std::endl;
		break;
	}
		case OIS::KC_S: case OIS::KC_DOWN:
			player->mCharacterDirection.z = 1.5f;
			break;
		case OIS::KC_A: case OIS::KC_LEFT:
			player->mCharacterDirection.x = -1.5f;
			break;
		case OIS::KC_D: case OIS::KC_RIGHT:
			player->mCharacterDirection.x = 1.5f;
			break;
		case OIS::KC_T:
			game->changeState(GameOverState::getInstance());
			break;
		case OIS::KC_Y:
			player->mCharacterRoot->setPosition(RoomObject[19]->mPosition.x, 40, RoomObject[19]->mPosition.z);
			player->mRoomNumber = 19;
			break;

	case OIS::KC_ESCAPE:
		game->changeState(TitleState::getInstance());
		break;
	case OIS::KC_SPACE:
		if (player->mRoomNumber == 19)
		{
			exit();
			break;
		}
	}
  // -----------------------------------------------------

  return true;
}

bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{

	switch (id)
	{
	case OIS::MB_Left:
		if (player->mNumOfFish > 0)
		{
			player->mAnimationState->setEnabled(false);
			player->mAnimationState2->setEnabled(false);
			player->mAnimationState3->setEnabled(true);
		}
		break;

	case OIS::MB_Right:
		{
			std::cout << player->mCharacterRoot->getPosition() << std::endl;
			std::cout << player->mHP << std::endl;
			break;
		}
	}
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	switch (id)
	{
	case OIS::MB_Left:
		if (player->mNumOfFish > 0)
		{
			player->mAnimationState->setEnabled(true);
			player->mAnimationState2->setEnabled(true);
			player->mAnimationState3->setEnabled(false);

			FishObject.push_back(new CFish(Vector3(player->mCharacterRoot->getPosition().x, player->mCharacterRoot->getPosition().y - 35, player->mCharacterRoot->getPosition().z)));
			FishObject.back()->createObjects(mSceneMgr);
			player->mNumOfFish -= 1;
		}
	default:
		break;
	}

  return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{ 
	mCameraYaw->yaw(Degree(-e.state.X.rel));
	mCameraPitch->pitch(Degree(-e.state.Y.rel));

	//mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel *0.05f));//줌인 줌아웃.
	return true;
}

void PlayState::_setLights(void)
{
  mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));
  mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

  mLightD = mSceneMgr->createLight("LightD");
  mLightD->setType(Light::LT_DIRECTIONAL);
  mLightD->setDirection( Vector3( 1, -2.0f, -1 ) );
  mLightD->setVisible(true);

  mLightP = mSceneMgr->createLight("LightP");
  mLightP->setDiffuseColour(0.5,0.1,0.3); 
  mLightP->setType(Light::LT_POINT);
  mLightP->setPosition(Vector3(0,300,0));
  mLightP->setVisible(false);
  
}

void PlayState :: drawSkyBox()
{
	mSceneMgr->setSkyBox(true, "Sky/SpaceSkyBox", 10000);
}

void PlayState::_drawGroundPlane(void)
{
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,3,-25))->attachObject(groundEntity);
	groundEntity->setMaterialName("HOWTOPLAY");

	Plane plane2(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground2",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane2,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity2 = mSceneMgr->createEntity("GroundPlane2", "Ground2");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[4]->mPosition.x, 3,RoomObject[4]->mPosition.z-25))->attachObject(groundEntity2);
	groundEntity2->setMaterialName("DEATHROOM");

	Plane plane3(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground3",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane3,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity3 = mSceneMgr->createEntity("GroundPlane3", "Ground3");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[12]->mPosition.x, 3, RoomObject[12]->mPosition.z - 25))->attachObject(groundEntity3);
	groundEntity3->setMaterialName("DEATHROOM");

	Plane plane4(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground4",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane4,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity4 = mSceneMgr->createEntity("GroundPlane4", "Ground4");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[17]->mPosition.x, 3, RoomObject[17]->mPosition.z - 25))->attachObject(groundEntity4);
	groundEntity4->setMaterialName("DEATHROOM");

	//퀴즈1
	Plane plane5(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground5",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane5,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity5 = mSceneMgr->createEntity("GroundPlane5", "Ground5");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[5]->mPosition.x, 3, RoomObject[5]->mPosition.z - 25))->attachObject(groundEntity5);
	groundEntity5->setMaterialName("QUIZ1");

	//퀴즈2
	Plane plane6(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground6",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane6,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity6 = mSceneMgr->createEntity("GroundPlane6", "Ground6");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[7]->mPosition.x, 3, RoomObject[7]->mPosition.z - 25))->attachObject(groundEntity6);
	groundEntity6->setMaterialName("QUIZ2");

	//퀴즈3
	Plane plane7(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground7",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane7,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity7 = mSceneMgr->createEntity("GroundPlane7", "Ground7");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[13]->mPosition.x, 3, RoomObject[13]->mPosition.z - 25))->attachObject(groundEntity7);
	groundEntity7->setMaterialName("QUIZ3");

	Plane plane8(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground8",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane8,
		400, 400,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);
	Entity* groundEntity8 = mSceneMgr->createEntity("GroundPlane8", "Ground8");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(RoomObject[19]->mPosition.x, 3, RoomObject[19]->mPosition.z - 25))->attachObject(groundEntity8);
	groundEntity8->setMaterialName("FINAL");

}

void PlayState::_drawGridPlane(void)
{
  // 좌표축 표시
  Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
  mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
  mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

  Ogre::ManualObject* gridPlane =  mSceneMgr->createManualObject("GridPlane"); 
  Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode"); 

  Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial","General"); 
  gridPlaneMaterial->setReceiveShadows(false); 
  gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true); 
  gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,1,1,0); 
  gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1,1,1); 
  gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,1); 

  gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
  for(int i=0; i<21; i++)
  {
    gridPlane->position(-500.0f, 0.0f, 500.0f-i*50);
    gridPlane->position(500.0f, 0.0f, 500.0f-i*50);

    gridPlane->position(-500.f+i*50, 0.f, 500.0f);
    gridPlane->position(-500.f+i*50, 0.f, -500.f);
  }

  gridPlane->end(); 

  gridPlaneNode->attachObject(gridPlane);
}
void PlayState::setNinjaPosition()
{
	NinjaObject.reserve(10);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[4]->mPosition.x + 150, 0, RoomObject[4]->mPosition.z + 150)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[12]->mPosition.x - 150, 0, RoomObject[12]->mPosition.z)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[17]->mPosition.x + 150, 0, RoomObject[17]->mPosition.z + 150)));
	ninjaObject.back()->createObjects(mSceneMgr);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ninjaObject.push_back(new CNinja(Vector3(RoomObject[8]->mPosition.x + 250, 0, RoomObject[8]->mPosition.z + 250)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[9]->mPosition.x - 250, 0, RoomObject[9]->mPosition.z - 250)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[3]->mPosition.x - 250, 0, RoomObject[3]->mPosition.z - 250)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[2]->mPosition.x - 250, 0, RoomObject[2]->mPosition.z - 250)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[11]->mPosition.x - 250, 0, RoomObject[11]->mPosition.z - 250)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[18]->mPosition.x - 250, 0, RoomObject[18]->mPosition.z - 250)));
	ninjaObject.back()->createObjects(mSceneMgr);

	ninjaObject.push_back(new CNinja(Vector3(RoomObject[15]->mPosition.x - 250, 0, RoomObject[15]->mPosition.z - 250)));
	ninjaObject.back()->createObjects(mSceneMgr);
}

void PlayState::setTimerPosition()
{
	TimerObject.reserve(4);

	TimerObject.push_back(new CTimer(Vector3(RoomObject[0]->mPosition.x + 100, 65, RoomObject[0]->mPosition.z + 100)));
	TimerObject.back()->createObjects(mSceneMgr);

	TimerObject.push_back(new CTimer(Vector3(RoomObject[7]->mPosition.x+100, 65, RoomObject[7]->mPosition.z+100)));
	TimerObject.back()->createObjects(mSceneMgr);

	TimerObject.push_back(new CTimer(Vector3(RoomObject[10]->mPosition.x + 100, 65, RoomObject[10]->mPosition.z + 100)));
	TimerObject.back()->createObjects(mSceneMgr);

	TimerObject.push_back(new CTimer(Vector3(RoomObject[15]->mPosition.x + 100, 65, RoomObject[15]->mPosition.z + 100)));
	TimerObject.back()->createObjects(mSceneMgr);
}

void PlayState::setRoomPosition()
{
	RoomObject.reserve(20);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 0.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(-1000.0f, 0.0f, 1000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 1000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(1000.0f, 0.0f, 1000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(-1000.0f, 0.0f, 2000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 2000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(1000.0f, 0.0f, 2000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(-1000.0f, 0.0f, 3000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 3000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(1000.0f, 0.0f, 3000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(-1000.0f, 0.0f, 4000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 4000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(1000.0f, 0.0f, 4000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(-1000.0f, 0.0f, 5000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 5000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(1000.0f, 0.0f, 5000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(-1000.0f, 0.0f, 6000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 6000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(1000.0f, 0.0f, 6000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);

	RoomObject.push_back(new CRoom(Vector3(0.0f, 0.0f, 7000.0f)));
	RoomObject.back()->createObjects(mSceneMgr);
}

void PlayState::setTriggerPosition()
{

	TriggerObject.reserve(75);

	// 시작지점 
	TriggerObject.push_back(new CTrigger(Vector3(330, 0, 0),3));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(0, 0, 330), 2));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(-330, 0, 0), 1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//1번방
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[1]->mPosition.x, 0, RoomObject[1]->mPosition.z -330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[1]->mPosition.x+330, 0, RoomObject[1]->mPosition.z), 3));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[1]->mPosition.x, 0, RoomObject[1]->mPosition.z+330), 4));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[1]->mPosition.x - 330, 0, RoomObject[1]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//2번방
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[2]->mPosition.x, 0, RoomObject[2]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[2]->mPosition.x + 330, 0, RoomObject[2]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[2]->mPosition.x, 0, RoomObject[2]->mPosition.z + 330), 5));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[2]->mPosition.x - 330, 0, RoomObject[2]->mPosition.z), 1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//3번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[3]->mPosition.x, 0, RoomObject[3]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[3]->mPosition.x + 330, 0, RoomObject[3]->mPosition.z), 0));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[3]->mPosition.x, 0, RoomObject[3]->mPosition.z + 330), 6));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[3]->mPosition.x - 330, 0, RoomObject[3]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//4번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[4]->mPosition.x, 0, RoomObject[4]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[4]->mPosition.x + 330, 0, RoomObject[4]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[4]->mPosition.x, 0, RoomObject[4]->mPosition.z + 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[4]->mPosition.x - 330, 0, RoomObject[4]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//5번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[5]->mPosition.x, 0, RoomObject[5]->mPosition.z - 330), 3));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[5]->mPosition.x + 330, 0, RoomObject[5]->mPosition.z), 6));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[5]->mPosition.x, 0, RoomObject[5]->mPosition.z + 330), 7));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[5]->mPosition.x - 330, 0, RoomObject[5]->mPosition.z), 4));
	TriggerObject.back()->createObjects(mSceneMgr);

	//6번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[6]->mPosition.x, 0, RoomObject[6]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[6]->mPosition.x + 330, 0, RoomObject[6]->mPosition.z), 8));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[6]->mPosition.x, 0, RoomObject[6]->mPosition.z + 330), 6));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[6]->mPosition.x - 330, 0, RoomObject[6]->mPosition.z), 5));
	TriggerObject.back()->createObjects(mSceneMgr);

	//7번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[7]->mPosition.x, 0, RoomObject[7]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[7]->mPosition.x + 330, 0, RoomObject[7]->mPosition.z), 11));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[7]->mPosition.x, 0, RoomObject[7]->mPosition.z + 330), 10));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[7]->mPosition.x - 330, 0, RoomObject[7]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//8번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[8]->mPosition.x, 0, RoomObject[8]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[8]->mPosition.x + 330, 0, RoomObject[8]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[8]->mPosition.x, 0, RoomObject[8]->mPosition.z + 330), 12));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[8]->mPosition.x - 330, 0, RoomObject[8]->mPosition.z), 1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//9번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[9]->mPosition.x, 0, RoomObject[9]->mPosition.z - 330), 6));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[9]->mPosition.x + 330, 0, RoomObject[9]->mPosition.z), 6));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[9]->mPosition.x, 0, RoomObject[9]->mPosition.z + 330), 12));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[9]->mPosition.x - 330, 0, RoomObject[9]->mPosition.z), 8));
	TriggerObject.back()->createObjects(mSceneMgr);

	//10번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[10]->mPosition.x, 0, RoomObject[10]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[10]->mPosition.x + 330, 0, RoomObject[10]->mPosition.z), 11));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[10]->mPosition.x, 0, RoomObject[10]->mPosition.z + 330), 13));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[10]->mPosition.x - 330, 0, RoomObject[10]->mPosition.z), 13));
	TriggerObject.back()->createObjects(mSceneMgr);

	//11번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[11]->mPosition.x, 0, RoomObject[11]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[11]->mPosition.x + 330, 0, RoomObject[11]->mPosition.z), 8));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[11]->mPosition.x, 0, RoomObject[11]->mPosition.z + 330), 16));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[11]->mPosition.x - 330, 0, RoomObject[11]->mPosition.z), 13));
	TriggerObject.back()->createObjects(mSceneMgr);

	//12번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[12]->mPosition.x, 0, RoomObject[12]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[12]->mPosition.x + 330, 0, RoomObject[12]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[12]->mPosition.x, 0, RoomObject[12]->mPosition.z + 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[12]->mPosition.x - 330, 0, RoomObject[12]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//13번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[13]->mPosition.x, 0, RoomObject[13]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[13]->mPosition.x + 330, 0, RoomObject[13]->mPosition.z), 14));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[13]->mPosition.x, 0, RoomObject[13]->mPosition.z + 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[13]->mPosition.x - 330, 0, RoomObject[13]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//14번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[14]->mPosition.x, 0, RoomObject[14]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[14]->mPosition.x + 330, 0, RoomObject[14]->mPosition.z), 18));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[14]->mPosition.x, 0, RoomObject[14]->mPosition.z + 330), 17));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[14]->mPosition.x - 330, 0, RoomObject[14]->mPosition.z), 16));
	TriggerObject.back()->createObjects(mSceneMgr);

	//15번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[15]->mPosition.x, 0, RoomObject[15]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[15]->mPosition.x + 330, 0, RoomObject[15]->mPosition.z), 18));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[15]->mPosition.x, 0, RoomObject[15]->mPosition.z + 330), 14));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[15]->mPosition.x - 330, 0, RoomObject[15]->mPosition.z), 12));
	TriggerObject.back()->createObjects(mSceneMgr);

	//16번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[16]->mPosition.x, 0, RoomObject[16]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[16]->mPosition.x + 330, 0, RoomObject[16]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[16]->mPosition.x, 0, RoomObject[16]->mPosition.z + 330), 19));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[16]->mPosition.x - 330, 0, RoomObject[16]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//17번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[17]->mPosition.x, 0, RoomObject[17]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[17]->mPosition.x + 330, 0, RoomObject[17]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[17]->mPosition.x, 0, RoomObject[17]->mPosition.z + 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[17]->mPosition.x - 330, 0, RoomObject[17]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	//18번방.
	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[18]->mPosition.x, 0, RoomObject[18]->mPosition.z - 330), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[18]->mPosition.x + 330, 0, RoomObject[18]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[18]->mPosition.x, 0, RoomObject[18]->mPosition.z + 330), 19));
	TriggerObject.back()->createObjects(mSceneMgr);

	TriggerObject.push_back(new CTrigger(Vector3(RoomObject[18]->mPosition.x - 330, 0, RoomObject[18]->mPosition.z), -1));
	TriggerObject.back()->createObjects(mSceneMgr);
}