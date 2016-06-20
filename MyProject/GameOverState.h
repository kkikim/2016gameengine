#pragma once

#include "GameState.h"

class GameOverState : public GameState
{
public:
	// Fill Here --------------------------------------------------------------------------------------------
	void enter();
	void exit();

	void pause();
	void resume();

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e)
	{
		return true;
	}
	bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
	{
		return true;
	}
	bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
	{
		return true;
	}

	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager *game, const OIS::KeyEvent &e)
	{
		return true;
	}

	// -------------------------------------------------------------------------------------------------------

	static GameOverState* getInstance() { return &mGameOverState; }

private:
	static GameOverState mGameOverState;

	bool mContinue;

	Ogre::Overlay* mTitleOverlay;
	Ogre::OverlayElement* mStartMsg;
};
