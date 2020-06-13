#include "SDL_image.h"

#include "DungeonMaster.h"
#include "Log.h"
#include "TheButton.h"
#include "minigames/Work.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// placement and spacing
const int WORK_PEGS_PER_SIDE = 10;	// how many dots along one side
const int WORK_PEG_W = 6 * PIXELW;	// size of a peg
const int WORK_PEG_H = 6 * PIXELH;
const int WORK_PEG_OFFSET_X = 384 - 271 + 12 * PIXELW;	// spacing between pegs
const int WORK_PEG_OFFSET_Y = 240 - 207 + 12 * PIXELH;

const float WORK_POINTS_PER_PEG = 2;	// points


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Work::RenderAllPegs() {
	for (int i = 0; i < WORK_PEGS_PER_SIDE; ++i) {
		for (int j = 0; j < WORK_PEGS_PER_SIDE; ++j) {
			if (mWorkPegs[i][j]) {
				SDL_Rect rect;
				rect.x = WORK_PEG_OFFSET_X + 2 * j * WORK_PEG_W;
				rect.y = WORK_PEG_OFFSET_Y + 2 * i * WORK_PEG_H;
				rect.w = WORK_PEG_W;
				rect.h = WORK_PEG_H;
				SDL_FillRect(mScreen, &rect, SDL_MapRGB(mScreen->format, SDL_CLR_WHITE));
			}
		}
	}
}


int Work::_InitialPreload() {
	// set title and instructions
	mMinigameName = "WORK";
	mInstructions.push_back("do all your things!");
	mInstructions.push_back("fear the deadline!");

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_BLACK, SDL_CLR_WHITE);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create WORK background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// create our array of bool values
	mWorkPegs = new bool* [WORK_PEGS_PER_SIDE];
	for (int i = 0; i < WORK_PEGS_PER_SIDE; ++i) {
		mWorkPegs[i] = new bool[WORK_PEGS_PER_SIDE];
	}

	// load the sound effects
	char str[256];
	GetAssetName(str, "sounds/work/peg.ogg");
	mPegSound = Mix_LoadWAV(str);
	if (!mPegSound) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load WORK sounds (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_BAD;

	return RETURN_SUCCESS;
}


void Work::_NewMinigame() {
	// repopulate all the pegs
	for (int i = 0; i < WORK_PEGS_PER_SIDE; ++i) {
		for (int j = 0; j < WORK_PEGS_PER_SIDE; ++j) {
			mWorkPegs[i][j] = true;
		}
	}

	// how many pegs in total
	mNumPegsRemaining = WORK_PEGS_PER_SIDE * WORK_PEGS_PER_SIDE;

	// place the player
	mPlayerRow = mPlayerCol = 0;

	// defaults
	mIsTimed = true;
}


void Work::_EndMinigame() {

}


void Work::_Process(Uint32 ticks) {
	if (mNumPegsRemaining > 0 && mTheButton->PressedThisFrame()) {
		// reduce the number of pegs
		mWorkPegs[mPlayerRow][mPlayerCol] = false;
		--mNumPegsRemaining;

		// play peg sounds
		Mix_PlayChannel(-1, mPegSound, 0);

		// add to score
		mDM->ModifyPlayerScore(WORK_POINTS_PER_PEG);

		// update the player position
		mPlayerCol = (mPlayerCol + 1) % WORK_PEGS_PER_SIDE;
		if (mPlayerCol == 0) {
			++mPlayerRow;
		}
	}
}


void Work::_Update(Uint32 ticks) {
	if (mNumPegsRemaining <= 0) {
		this->NotifyDMMinigameComplete();
	}
}


void Work::_Render(Uint32 ticks) {
	// draw our background image
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// draw all the work pegs
	this->RenderAllPegs();
}


int Work::_Cleanup() {
	// free up memory
	for (int i = 0; i < WORK_PEGS_PER_SIDE; ++i) {
		delete[] mWorkPegs[i];
	}
	delete[] mWorkPegs;

	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Work::Work() {

}


Work::~Work() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Work::Instance() {
	static Work mInstance;
	return &mInstance;
}
