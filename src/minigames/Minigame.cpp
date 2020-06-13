#include "common.h"
#include "DungeonMaster.h"
#include "minigames/Minigame.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Minigame::NotifyDMMinigameComplete() {
	mDM->EndMinigame(this);
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Minigame::Minigame() {
	mLog = Log::Instance();
	mDM = DungeonMaster::Instance();
	mTheButton = TheButton::Instance();
}


Minigame::~Minigame() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

const char* Minigame::GetTitle() {
	return mMinigameName;
}


vector<const char*> Minigame::GetInstructions() {
	return mInstructions;
}


MinigameType Minigame::GetMinigameType() {
	return mGameType;
}


int Minigame::InitialPreload() {
	// get a handle on the main screen
	mScreen = mDM->GetScreen();

	return this->_InitialPreload();
}


void Minigame::NewMinigame(bool realLife) {
	mReceivedFirstEvent = false;
	mTimedOut = false;

	// is this the real life?
	mIsRealLife = realLife;

	// start our timer
	mMinigameTimer.Reset();
	mMinigameTimer.Start();

	this->_NewMinigame();
}


void Minigame::EndMinigame() {
	this->_EndMinigame();

	// and stop listening for sounds
	Mix_ChannelFinished(NULL);
}


bool Minigame::IsTimed() {
	return mIsTimed;
}


float Minigame::FractionTimeRemaining() {
	return (float)mMinigameTimer.GetTicks() / (float)MINIGAME_DEFAULT_LENGTH;
}


bool Minigame::HasTimedOut() {
	return mTimedOut;
}


void Minigame::Process(Uint32 ticks) {
	if (!mReceivedFirstEvent) {
		/**
		 * Make sure the first event received is a button-press, not a button-release
		 * held over from a previous minigame.
		 */
		if (mTheButton->PressedThisFrame() || mTheButton->ReleasedThisFrame()) {
			mReceivedFirstEvent = true;
			if (mTheButton->ReleasedThisFrame()) {
				mTheButton->ConsumeButtonReleased();
			}
		}
	}

	this->_Process(ticks);
}


void Minigame::Update(Uint32 ticks) {
	if (mIsTimed) {
		if (mMinigameTimer.GetTicks() >= MINIGAME_DEFAULT_LENGTH) {
			mTimedOut = true;
		}
	}

	this->_Update(ticks);
}


void Minigame::Render(Uint32 ticks) {
	this->_Render(ticks);
}


int Minigame::Cleanup() {
	return this->_Cleanup();
}
