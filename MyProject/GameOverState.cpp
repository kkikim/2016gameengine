#include "TitleState.h"
#include "PlayState.h"
#include "OptionState.h"
#include "GameOverState.h"

using namespace Ogre;

GameOverState GameOverState::mGameOverState;

void GameOverState::enter(void)
{
	mContinue = true;
	mTitleOverlay = OverlayManager::getSingleton().getByName("Overlay/GAMEOVER");
	mTitleOverlay->show();
}

void GameOverState::exit(void)
{
	mTitleOverlay->hide();
}

void GameOverState::pause(void)
{
	std::cout << "GameOverState pause\n";
}

void GameOverState::resume(void)
{
	std::cout << "GameOverState resume\n";
}

bool GameOverState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	static float elapsed = 0.0f;

	elapsed += evt.timeSinceLastFrame;

	return true;
}

bool GameOverState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	return mContinue;
}

bool GameOverState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------------------
	switch (e.key)
	{
	case OIS::KC_SPACE:
		exit();
		break;
	case  OIS::KC_ESCAPE:
		exit();
		break;
	}
	// -----------------------------------------------------------------
	return true;
}
