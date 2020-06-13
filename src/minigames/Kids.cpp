#include "common.h"
#include "DungeonMaster.h"
#include "minigames/Kids.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// position
const Uint32 KIDS_CHILD_STARTX = 10;
const Uint32 KIDS_CHILD_STARTY = 58;
const Uint32 KIDS_PARENT_STARTX = 32;
const Uint32 KIDS_PARENT_STARTY = 56;
const int KIDS_MIN_Y = 17 * PIXELH;
const int KIDS_MAX_Y = 143 * PIXELH;

// movement
const int KIDS_PARENT_LIFT_DIST = 8 * PIXELH;
const int KIDS_PARENT_FALL_DIST = 4 * PIXELH;
const int KIDS_CHILD_FALL_DIST = 4 * PIXELH;
const int KIDS_MISSILE_MOVE = 4 * PIXELW;

// ticking clock
const Uint32 KIDS_CLOCK_TICK = 125;		// in milliseconds
const int KIDS_CHILD_FALL_NUM_TICKS = 2;
const int KIDS_PARENT_FALL_NUM_TICKS = 2;
const int KIDS_MISSILE_MOVE_TICKS = 1;
const int KIDS_MISSILE_TOPBOTTOM_RELEASE_TICKS = 8;
const int KIDS_MISSILE_MIDDLE_RELEASE_TICKS = 10;

// child
const float KIDS_CHILD_HIT_PTS = -50;
const int KIDS_VERTICAL_TETHER = 30 * PIXELH;

// parent
const float KIDS_PARENT_HIT_PTS = -5;

// missiles
const int KIDS_MAX_MISSILES = 30;

// sound effects
const char* KIDS_PARENT_SWOOP_SFX = "sounds/kids/parentswoop.ogg";
const char* KIDS_CHILD_HIT_SFX = "sounds/kids/kidhit.ogg";
const char* KIDS_PARENT_HIT_SFX = "sounds/kids/parenthit.ogg";
const char* KIDS_MISSILE_MOVE_SFX = "sounds/kids/missilemove.ogg";


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Kids::TickClock() {
	// check child
	if (++mChildFallTicks >= KIDS_CHILD_FALL_NUM_TICKS) {
		this->MoveChild(KIDS_CHILD_FALL_DIST);
	}

	// check parent
	if (++mParentFallTicks >= KIDS_PARENT_FALL_NUM_TICKS) {
		this->MoveParent(KIDS_PARENT_FALL_DIST);
	}

	// check missile move
	if (++mMissileMoveTicks >= KIDS_MISSILE_MOVE_TICKS) {
		this->MoveMissiles();
	}

	// check top/bottom missile release
	if (++mMissileTopBottomReleaseTicks >= KIDS_MISSILE_TOPBOTTOM_RELEASE_TICKS) {
		mMissileTopBottomReleaseTicks = 0;
		this->ReleaseTopBottomMissiles();
	}

	// check middle missile release
	if (++mMissileMiddleReleaseTicks >= KIDS_MISSILE_MIDDLE_RELEASE_TICKS) {
		mMissileMiddleReleaseTicks = 0;
		this->ReleaseMiddleMissiles();
	}
}


int Kids::GeneratePersonSprites() {
	mChildSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mParentSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 8 * PIXELW, 8 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mChildSfc || !mParentSfc) {
		return RETURN_ERROR;
	}

	// colour in the child
	SDL_Rect rect;
	SDL_FillRect(mChildSfc, NULL, SDL_MapRGB(mChildSfc->format, SDL_CLR_WHITE));
	rect.x = PIXELW;
	rect.y = 0;
	rect.w = 2 * PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mChildSfc, &rect, SDL_MapRGB(mChildSfc->format, SDL_CLR_BLACK));
	rect.y += 3 * PIXELH;
	SDL_FillRect(mChildSfc, &rect, SDL_MapRGB(mChildSfc->format, SDL_CLR_BLACK));
	rect.x = 0;
	rect.y = PIXELH;
	rect.w = PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mChildSfc, &rect, SDL_MapRGB(mChildSfc->format, SDL_CLR_BLACK));
	rect.x += 3 * PIXELW;
	SDL_FillRect(mChildSfc, &rect, SDL_MapRGB(mChildSfc->format, SDL_CLR_BLACK));

	// colour in the parent
	SDL_FillRect(mParentSfc, NULL, SDL_MapRGB(mParentSfc->format, SDL_CLR_WHITE));
	rect.x = 2 * PIXELW;
	rect.y = 0;
	rect.w = 4 * PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mParentSfc, &rect, SDL_MapRGB(mParentSfc->format, SDL_CLR_BLACK));
	rect.y += 6 * PIXELH;
	SDL_FillRect(mParentSfc, &rect, SDL_MapRGB(mParentSfc->format, SDL_CLR_BLACK));
	rect.x = 0;
	rect.y = 2 * PIXELH;
	rect.w = 2 * PIXELW;
	rect.h = 4 * PIXELH;
	SDL_FillRect(mParentSfc, &rect, SDL_MapRGB(mParentSfc->format, SDL_CLR_BLACK));
	rect.x += 6 * PIXELW;
	SDL_FillRect(mParentSfc, &rect, SDL_MapRGB(mParentSfc->format, SDL_CLR_BLACK));

	return RETURN_SUCCESS;
}


int Kids::GenerateMissileSprite() {
	mMissileSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 8 * PIXELW, 8 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mMissileSfc) {
		return RETURN_ERROR;
	}

	// colour in the missile
	SDL_Rect rect;
	SDL_FillRect(mMissileSfc, NULL, SDL_MapRGB(mMissileSfc->format, SDL_CLR_BLACK));
	rect.x = 2 * PIXELW;
	rect.y = 0;
	rect.w = 4 * PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mMissileSfc, &rect, SDL_MapRGB(mMissileSfc->format, SDL_CLR_WHITE));
	rect.y += 6 * PIXELH;
	SDL_FillRect(mMissileSfc, &rect, SDL_MapRGB(mMissileSfc->format, SDL_CLR_WHITE));
	rect.x = 0;
	rect.y = 2 * PIXELH;
	rect.w = 2 * PIXELW;
	rect.h = 4 * PIXELH;
	SDL_FillRect(mMissileSfc, &rect, SDL_MapRGB(mMissileSfc->format, SDL_CLR_WHITE));
	rect.x += 6 * PIXELW;
	SDL_FillRect(mMissileSfc, &rect, SDL_MapRGB(mMissileSfc->format, SDL_CLR_WHITE));

	return RETURN_SUCCESS;
}

int Kids::LoadSoundEffects() {
	char str[256];
	GetAssetName(str, KIDS_PARENT_SWOOP_SFX);
	mParentSwoopSnd = Mix_LoadWAV(str);
	GetAssetName(str, KIDS_CHILD_HIT_SFX);
	mChildHitSnd = Mix_LoadWAV(str);
	GetAssetName(str, KIDS_PARENT_HIT_SFX);
	mParentHitSnd = Mix_LoadWAV(str);
	GetAssetName(str, KIDS_MISSILE_MOVE_SFX);
	mMissileMoveSnd = Mix_LoadWAV(str);
	if (!mParentSwoopSnd || !mChildHitSnd || !mParentHitSnd || !mMissileMoveSnd) {
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


void Kids::MoveChild(int move) {
	mChildPos.y += mChildDirection * KIDS_CHILD_FALL_DIST;

	// make sure they don't go out of range
	if (mChildPos.y < KIDS_MIN_Y) {
		mChildPos.y = KIDS_MIN_Y;
	} else if (mChildPos.y + mChildSfc->h > KIDS_MAX_Y) {
		mChildPos.y = KIDS_MAX_Y - mChildSfc->h;
	}

	mChildFallTicks = 0;
}


void Kids::LimitChild() {
	if (mChildPos.y > mParentPos.y) {
		if (mChildPos.y - mParentPos.y > KIDS_VERTICAL_TETHER) {
			mChildPos.y = mParentPos.y + KIDS_VERTICAL_TETHER;
			mChildFallTicks = 0;
		}
	} else {
		if (mParentPos.y - mChildPos.y > KIDS_VERTICAL_TETHER) {
			mChildPos.y = mParentPos.y - KIDS_VERTICAL_TETHER;
			mChildFallTicks = 0;
		}
	}
}


void Kids::MoveParent(int move) {
	mParentPos.y += move;

	// make sure they don't go out of range
	if (mParentPos.y < KIDS_MIN_Y) {
		mParentPos.y = KIDS_MIN_Y;
	} else if (mParentPos.y + mParentSfc->h > KIDS_MAX_Y) {
		mParentPos.y = KIDS_MAX_Y - mParentSfc->h;
	}

	// see if we should change the kid's direction
	if (abs(mParentPos.y - mChildPos.y) > KIDS_VERTICAL_TETHER &&
		((move > 0 && mChildDirection < 0) ||
			(move < 0 && mChildDirection > 0))) {
		mChildDirection *= -1;
	}

	mParentFallTicks = 0;
}


void Kids::MoveMissiles() {
	// make a sound effect
	//Mix_PlayChannel(-1, mMissileMoveSnd, 0);

	for (int i = 0; i < KIDS_MAX_MISSILES; ++i) {
		if (mMissileList[i].active) {
			mMissileList[i].pos.x -= KIDS_MISSILE_MOVE;

			// check if the missile's out of bounds
			if (mMissileList[i].pos.x < 33 * PIXELW) {
				--mNumReleasedMissiles;
				mMissileList[i].active = false;
				continue;
			}

			// check if the missile intersects the parent
			if (RectanglesOverlap(mParentPos, mMissileList[i].pos)) {
				--mNumReleasedMissiles;
				mMissileList[i].active = false;
				mDM->ModifyPlayerScore(KIDS_PARENT_HIT_PTS);
				Mix_PlayChannel(-1, mParentHitSnd, 0);
				continue;
			}

			// check if the missile intersects the child
			if (RectanglesOverlap(mChildPos, mMissileList[i].pos)) {
				--mNumReleasedMissiles;
				mMissileList[i].active = false;
				mDM->ModifyPlayerScore(KIDS_CHILD_HIT_PTS);
				Mix_PlayChannel(-1, mChildHitSnd, 0);
				continue;
			}
		}
	}

	mMissileMoveTicks = 0;
}


int Kids::FindInactiveMissile() {
	for (int i = 0; i < KIDS_MAX_MISSILES; ++i) {
		if (!mMissileList[i].active) {
			return i;
		}
	}

	return -1;
}


void Kids::ReleaseTopBottomMissiles() {
	// release missiles top and bottom
	if (mNumReleasedMissiles < KIDS_MAX_MISSILES - 2) {
		int ind = this->FindInactiveMissile();
		GetMiniFramePixel(MINIFRAME_SZ - 8, 0, &mMissileList[ind].pos);
		mMissileList[ind].active = true;

		ind = this->FindInactiveMissile();
		GetMiniFramePixel(MINIFRAME_SZ - 8, MINIFRAME_SZ - 8, &mMissileList[ind].pos);
		mMissileList[ind].active = true;

		mNumReleasedMissiles += 2;
	}
}


void Kids::ReleaseMiddleMissiles() {
	if (mNumReleasedMissiles < KIDS_MAX_MISSILES - 1) {
		int ind = this->FindInactiveMissile();

		// randomly start the missile in the middle
		int startx = MINIFRAME_SZ - 8;
		int starty = 9 + rand() % 13 * 8;
		GetMiniFramePixel(startx, starty, &mMissileList[ind].pos);
		mMissileList[ind].active = true;

		++mNumReleasedMissiles;
	}
}


void Kids::RenderParentAndChild() {
	SDL_BlitSurface(mChildSfc, NULL, mScreen, &mChildPos);
	SDL_BlitSurface(mParentSfc, NULL, mScreen, &mParentPos);
}


void Kids::RenderMissiles() {
	for (int i = 0; i < KIDS_MAX_MISSILES; ++i) {
		if (mMissileList[i].active) {
			SDL_BlitSurface(mMissileSfc, NULL, mScreen, &mMissileList[i].pos);
		}
	}
}


int Kids::_InitialPreload() {
	// set title and instructions
	mMinigameName = "KIDS";
	mInstructions.push_back("fly with your kids!");
	mInstructions.push_back("kids are stupid!");
	mInstructions.push_back("don't put missiles on kids!");

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_WHITE, SDL_CLR_BLACK);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create KIDS background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// try to create our parent and child sprites
	if (this->GeneratePersonSprites() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create KIDS people sprites (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// READY MISSILES
	mMissileList = new Missile[KIDS_MAX_MISSILES];

	// try to create our missile sprites
	if (this->GenerateMissileSprite() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create KIDS missile sprite (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// try to load sound effects
	if (this->LoadSoundEffects() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load KIDS sound effects (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_GOOD;

	return RETURN_SUCCESS;
}


void Kids::_NewMinigame() {
	// place the parent and child
	GetMiniFramePixel(KIDS_CHILD_STARTX, KIDS_CHILD_STARTY, &mChildPos);
	mChildPos.w = mChildSfc->w;
	mChildPos.h = mChildSfc->h;
	GetMiniFramePixel(KIDS_PARENT_STARTX, KIDS_PARENT_STARTY, &mParentPos);
	mParentPos.w = mParentSfc->w;
	mParentPos.h = mParentSfc->h;

	// initialise movement
	mChildLastFall = mParentLastFall = SDL_GetTicks();
	mChildFallTicks = mParentFallTicks = 0;
	mMissileTopBottomReleaseTicks = mMissileMoveTicks = 0;
	mNumReleasedMissiles = 0;

	// initialise missiles
	for (int i = 0; i < KIDS_MAX_MISSILES; ++i) {
		mMissileList[i].active = false;
	}

	// defaults
	mIsTimed = true;
	mChildDirection = 1;	// falling downwards
	mLastClockTick = SDL_GetTicks();
}


void Kids::_EndMinigame() {

}


void Kids::_Process(Uint32 ticks) {
	if (mTheButton->PressedThisFrame()) {
		// play a sound effect
		Mix_PlayChannel(-1, mParentSwoopSnd, 0);

		// move the parent
		this->MoveParent(-KIDS_PARENT_LIFT_DIST);
		mParentFallTicks = 0;
	}
}


void Kids::_Update(Uint32 ticks) {
	Uint32 now = SDL_GetTicks();

	// the ticking clock
	if (now - mLastClockTick > KIDS_CLOCK_TICK) {
		this->TickClock();
		mLastClockTick = now;
	}

	// keep the child tethered to the parent
	this->LimitChild();
}


void Kids::_Render(Uint32 ticks) {
	// draw our background image
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// draw all missiles
	this->RenderMissiles();

	// draw the parent and child
	this->RenderParentAndChild();
}


int Kids::_Cleanup() {
	delete[] mMissileList;

	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Kids::Kids() {
}


Kids::~Kids() {
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Kids::Instance() {
	static Kids mInstance;
	return &mInstance;
}
