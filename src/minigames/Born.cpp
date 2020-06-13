#include "common.h"
#include "minigames/Born.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANT AND STATIC VARIABLES
///////////////////////////////////////////////////////////////////////////////

const Uint32 BORN_DILATION_STEP = 2;
const Uint32 BORN_MAX_DILATION = 63;
const Uint32 BORN_CONTRACTION_STEP = 4;
const Uint32 BORN_CONTRACTION_TIME = 175;	// in milliseconds

// sound effects
const int BORN_DILATION_BEEP1 = 5;
const int BORN_DILATION_BEEP2 = 15;
const int BORN_DILATION_BEEP3 = 25;
const char* BORN_DILATION1_SNDFILE = "sounds/born/dilation1.ogg";
const char* BORN_DILATION2_SNDFILE = "sounds/born/dilation2.ogg";
const char* BORN_DILATION3_SNDFILE = "sounds/born/dilation3.ogg";
const char* BORN_VICTORY_SNDFILE = "sounds/born/born.ogg";
static bool mBornSfxPlayed;
static int mBornSfxChannel;


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Born::CatchFinishedBornSounds(int channel) {
	if (channel == mBornSfxChannel) {
		mBornSfxPlayed = true;
	}
}


int Born::LoadSoundEffects() {
	char str[256];

	// dilation sounds
	GetAssetName(str, BORN_DILATION1_SNDFILE);
	mDilationBeeps[0] = Mix_LoadWAV(str);
	GetAssetName(str, BORN_DILATION2_SNDFILE);
	mDilationBeeps[1] = Mix_LoadWAV(str);
	GetAssetName(str, BORN_DILATION3_SNDFILE);
	mDilationBeeps[2] = Mix_LoadWAV(str);
	if (!mDilationBeeps[0] || !mDilationBeeps[1] || !mDilationBeeps[2]) {
		return RETURN_ERROR;
	}

	// victory beep
	GetAssetName(str, BORN_VICTORY_SNDFILE);
	mVictoryBeep = Mix_LoadWAV(str);
	if (!mVictoryBeep) {
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


void Born::Contract() {
	if (mDilation > BORN_CONTRACTION_STEP) {
		mDilation -= BORN_CONTRACTION_STEP;

	} else {
		mDilation = 0;
	}

	mLastContraction = SDL_GetTicks();
}


int Born::_InitialPreload() {
	// set title and instructions
	mMinigameName = "BORN";
	mInstructions.push_back("try to be born!");
	mInstructions.push_back("contractions are counter-productive!");

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_BLACK, SDL_CLR_WHITE);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not generate BORN background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// load sound effects
	if (this->LoadSoundEffects() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load BORN sound effects (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_BAD;	// birth isn't really bad, but you know. art.

	return RETURN_SUCCESS;
}


void Born::_NewMinigame() {
	// defaults
	mDilation = 0;
	mBorn = false;
	mIsTimed = false;
	mLastContraction = SDL_GetTicks();

	// sound effects
	Mix_ChannelFinished(CatchFinishedBornSounds);
	mBornSfxChannel = -1;
	mBornSfxPlayed = false;
}


void Born::_EndMinigame() {
}


void Born::_Process(Uint32 ticks) {
	if (!mBorn) {
		if (mTheButton->PressedThisFrame()) {
			// if button was pressed, dilate by one step
			mDilation += BORN_DILATION_STEP;
			if (mDilation > BORN_MAX_DILATION) {
				mDilation = BORN_MAX_DILATION;
			}
			mLastContraction = SDL_GetTicks();

			// play a sound at certain dilation distances
			if (BORN_DILATION_BEEP1 * BORN_DILATION_STEP == mDilation) {
				Mix_PlayChannel(-1, mDilationBeeps[0], 0);
			} else if (BORN_DILATION_BEEP2 * BORN_DILATION_STEP == mDilation) {
				Mix_PlayChannel(-1, mDilationBeeps[1], 0);
			} else if (BORN_DILATION_BEEP3 * BORN_DILATION_STEP == mDilation) {
				Mix_PlayChannel(-1, mDilationBeeps[2], 0);
			}
		}
	}
}


void Born::_Update(Uint32 ticks) {
	if (!mBorn) {
		// check for full birth
		if (mDilation >= BORN_MAX_DILATION) {
			mBorn = true;

			// play the victory sound
			mBornSfxChannel = Mix_PlayChannel(1, mVictoryBeep, 0);

		} else {
			// check if it's time to contract
			Uint32 now = SDL_GetTicks();
			if (now - mLastContraction >= BORN_CONTRACTION_TIME) {
				this->Contract();
			}
		}
	} else {	// am born
		if (mBornSfxPlayed) {
			this->NotifyDMMinigameComplete();
		}
	}
}


void Born::_Render(Uint32 ticks) {
	// draw our background image
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// render the dilating thingy
	SDL_Rect rect;
	GetMiniFramePixel(63 - mDilation, 0, &rect);
	rect.w = 2 * mDilation * PIXELW;
	rect.h = 126 * PIXELH;
	SDL_FillRect(mScreen, &rect, SDL_MapRGB(mScreen->format, SDL_CLR_WHITE));
}


int Born::_Cleanup() {
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Born::Born() {
}


Born::~Born() {
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Born::Instance() {
	static Born mInstance;
	return &mInstance;
}
