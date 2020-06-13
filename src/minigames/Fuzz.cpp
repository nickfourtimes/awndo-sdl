#include <iostream>

#include "common.h"
#include "GarbageGobbler.h"
#include "minigames/Fuzz.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

const int FUZZ_NUM_FUZZIES = 5;	// number of random static screens to generate
const Uint32 FUZZ_TIME_ACTIVE = 500;	// in milliseconds
const Uint32 FUZZ_TIME_PER_SCREEN = FUZZ_TIME_ACTIVE / FUZZ_NUM_FUZZIES;	// time to flash each random screen


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Fuzz::PointAtNextFuzzy() {
	if (mSurfaceOrder.empty()) {
		return;
	}

	int ind = mSurfaceOrder.back();
	mSurfaceOrder.pop_back();
	mCurrentSfc = mFuzzySurfaces[ind];
	mLastScreenSwitch = SDL_GetTicks();
}


int Fuzz::_InitialPreload() {
	for (int i = 0; i < FUZZ_NUM_FUZZIES; ++i) {
		// create a surface on which to draw
		SDL_Surface* temp = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
		if (!temp) {
			char msg[256];
			sprintf(msg, "ERROR: Could not create FUZZ surface (%s)!\n", SDL_GetError());
			mLog->LogMsg(msg);
			return RETURN_ERROR;
		}

		// fill each "pixel" individually with black or white
		for (int row = 0; row < ATARI_PIXEL_H; ++row) {
			for (int col = 0; col < ATARI_PIXEL_W; ++col) {
				SDL_Rect rect;
				rect.x = col * PIXELW;
				rect.y = row * PIXELH;
				rect.w = PIXELW;
				rect.h = PIXELH;
				if (rand() % 2 == 0) {
					SDL_FillRect(temp, &rect, SDL_MapRGB(temp->format, SDL_CLR_BLACK));
				} else {
					SDL_FillRect(temp, &rect, SDL_MapRGB(temp->format, SDL_CLR_WHITE));
				}
			}
		}

		// store this randomly-filled surface
		mFuzzySurfaces.push_back(temp);
	}

	// load static noise
	char str[256];
	GetAssetName(str, "sounds/fuzz/static.ogg");
	mNoise = Mix_LoadWAV(str);
	if (!mNoise) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load FUZZ sound effect (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_NEUTRAL;

	return RETURN_SUCCESS;
}


void Fuzz::_NewMinigame() {
	// generate a random ordering for the random screens
	mSurfaceOrder.clear();
	for (int i = 0; i < FUZZ_NUM_FUZZIES; ++i) {
		mSurfaceOrder.push_back(i);
	}

	// FISHER-YATES SHUFFLE!!!
	int n = FUZZ_NUM_FUZZIES;
	while (--n > 0) {
		int k = rand() % (n + 1);
		int temp = mSurfaceOrder[k];
		mSurfaceOrder[k] = mSurfaceOrder[n];
		mSurfaceOrder[n] = temp;
	}

	// start the clock
	mTimeStarted = mLastScreenSwitch = SDL_GetTicks();

	// defaults
	mIsTimed = false;

	// play the static sound
	mNoiseChannel = Mix_PlayChannel(-1, mNoise, -1);

	this->PointAtNextFuzzy();
}


void Fuzz::_EndMinigame() {
	Mix_HaltChannel(mNoiseChannel);
}


void Fuzz::_Process(Uint32 ticks) {
}


void Fuzz::_Update(Uint32 ticks) {
	Uint32 now = SDL_GetTicks();

	// check if we should switch to a new static screen
	if (now - mLastScreenSwitch > FUZZ_TIME_PER_SCREEN) {
		this->PointAtNextFuzzy();
	}

	// check if we're finished showing static
	if (now - mTimeStarted > FUZZ_TIME_ACTIVE) {
		this->NotifyDMMinigameComplete();
	}
}


void Fuzz::_Render(Uint32 ticks) {
	// render the current fuzzy surface to the screen
	SDL_BlitSurface(mCurrentSfc, NULL, mScreen, NULL);
}


int Fuzz::_Cleanup() {
	// clean up all the static surfaces
	for (int i = 0; i < FUZZ_NUM_FUZZIES; ++i) {
		GarbageGobbler::Instance()->Discard(mFuzzySurfaces.back());
		mFuzzySurfaces.pop_back();
	}

	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Fuzz::Fuzz() {
}


Fuzz::~Fuzz() {
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Fuzz::Instance() {
	static Fuzz mInstance;
	return &mInstance;
}
