#include <SDL_ttf.h>

#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "minigames/Play.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// player sprites
const int PLAY_PLAYER_SIZE_FACTOR = 2;
const int PLAY_PLAYERW = PLAY_PLAYER_SIZE_FACTOR * 3 * PIXELW;
const int PLAY_PLAYERH = PLAY_PLAYER_SIZE_FACTOR * 3 * PIXELH;

// movement
const int PLAY_NUM_PIXELS_PER_MOVE = 2;
const Uint32 PLAY_DEFAULT_MOVE_DELAY = 75;	// in milliseconds

// pickups
const Uint32 PLAY_NUM_GOOD_PICKUPS = 15;
const Uint32 PLAY_NUM_BAD_PICKUPS = 15;
const float PLAY_O_SCORE = 20;
const float PLAY_X_SCORE = -10;

// sound effects
const char* PLAY_GOOD_PICKUP_SND = "sounds/play/goodpickup.ogg";
const char* PLAY_BAD_PICKUP_SND = "sounds/play/badpickup.ogg";
const char* PLAY_CRASH_SND = "sounds/play/crash.ogg";
static bool mPlayCrashSfxPlayed;
static int mPlayCrashSfxChannel;


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Play::CatchFinishedPlaySounds(int channel) {
	if (channel == mPlayCrashSfxChannel) {
		mPlayCrashSfxPlayed = true;
	}
}


int Play::GeneratePlayerSprites() {
	// draw the player's sprites
	mPlayerSfc[UP] = SDL_CreateRGBSurface(SDL_HWSURFACE, PLAY_PLAYERW, PLAY_PLAYERH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mPlayerSfc[DN] = SDL_CreateRGBSurface(SDL_HWSURFACE, PLAY_PLAYERW, PLAY_PLAYERH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mPlayerSfc[LT] = SDL_CreateRGBSurface(SDL_HWSURFACE, PLAY_PLAYERW, PLAY_PLAYERH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mPlayerSfc[RT] = SDL_CreateRGBSurface(SDL_HWSURFACE, PLAY_PLAYERW, PLAY_PLAYERH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	for (int i = 0; i < 4; ++i) {
		if (!mPlayerSfc[i]) {
			return RETURN_ERROR;
		}

		// quicker to fill them all in with black, then "punch out" the white bits. RACIALISM!
		SDL_FillRect(mPlayerSfc[i], NULL, SDL_MapRGB(mPlayerSfc[i]->format, SDL_CLR_BLACK));
	}

	SDL_Rect rect;
	rect.w = PLAY_PLAYER_SIZE_FACTOR * PIXELW;
	rect.h = PLAY_PLAYER_SIZE_FACTOR * PIXELH;

	// up-facing
	rect.x = rect.y = 0;
	SDL_FillRect(mPlayerSfc[UP], &rect, SDL_MapRGB(mPlayerSfc[UP]->format, SDL_CLR_WHITE));
	rect.x = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELW;
	SDL_FillRect(mPlayerSfc[UP], &rect, SDL_MapRGB(mPlayerSfc[UP]->format, SDL_CLR_WHITE));
	rect.x = PLAY_PLAYER_SIZE_FACTOR * PIXELW;
	rect.y = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELH;
	SDL_FillRect(mPlayerSfc[UP], &rect, SDL_MapRGB(mPlayerSfc[UP]->format, SDL_CLR_WHITE));

	// down-facing
	rect.x = PLAY_PLAYER_SIZE_FACTOR * PIXELW;
	rect.y = 0;
	SDL_FillRect(mPlayerSfc[DN], &rect, SDL_MapRGB(mPlayerSfc[DN]->format, SDL_CLR_WHITE));
	rect.x = 0;
	rect.y = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELH;
	SDL_FillRect(mPlayerSfc[DN], &rect, SDL_MapRGB(mPlayerSfc[DN]->format, SDL_CLR_WHITE));
	rect.x = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELW;
	SDL_FillRect(mPlayerSfc[DN], &rect, SDL_MapRGB(mPlayerSfc[DN]->format, SDL_CLR_WHITE));

	// left-facing
	rect.x = rect.y = 0;
	SDL_FillRect(mPlayerSfc[LT], &rect, SDL_MapRGB(mPlayerSfc[LT]->format, SDL_CLR_WHITE));
	rect.y = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELH;
	SDL_FillRect(mPlayerSfc[LT], &rect, SDL_MapRGB(mPlayerSfc[LT]->format, SDL_CLR_WHITE));
	rect.x = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELW;
	rect.y = PLAY_PLAYER_SIZE_FACTOR * PIXELH;
	SDL_FillRect(mPlayerSfc[LT], &rect, SDL_MapRGB(mPlayerSfc[LT]->format, SDL_CLR_WHITE));

	// right-facing
	rect.x = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELW;
	rect.y = 0;
	SDL_FillRect(mPlayerSfc[RT], &rect, SDL_MapRGB(mPlayerSfc[RT]->format, SDL_CLR_WHITE));
	rect.y = PLAY_PLAYER_SIZE_FACTOR * 2 * PIXELH;
	SDL_FillRect(mPlayerSfc[RT], &rect, SDL_MapRGB(mPlayerSfc[RT]->format, SDL_CLR_WHITE));
	rect.x = 0;
	rect.y = PLAY_PLAYER_SIZE_FACTOR * PIXELH;
	SDL_FillRect(mPlayerSfc[RT], &rect, SDL_MapRGB(mPlayerSfc[RT]->format, SDL_CLR_WHITE));

	return RETURN_SUCCESS;
}


int Play::GeneratePickupSprites() {
	mGoodPickupSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mBadPickupSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mGoodPickupSfc || !mBadPickupSfc) {
		return RETURN_ERROR;
	}

	// colour in the good pickup
	SDL_Rect rect;
	SDL_FillRect(mGoodPickupSfc, NULL, SDL_MapRGB(mGoodPickupSfc->format, SDL_CLR_WHITE));
	rect.x = PIXELW;
	rect.y = 0;
	rect.w = 2 * PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mGoodPickupSfc, &rect, SDL_MapRGB(mGoodPickupSfc->format, SDL_CLR_BLACK));
	rect.y += 3 * PIXELH;
	SDL_FillRect(mGoodPickupSfc, &rect, SDL_MapRGB(mGoodPickupSfc->format, SDL_CLR_BLACK));
	rect.x = 0;
	rect.y = PIXELH;
	rect.w = PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mGoodPickupSfc, &rect, SDL_MapRGB(mGoodPickupSfc->format, SDL_CLR_BLACK));
	rect.x += 3 * PIXELW;
	SDL_FillRect(mGoodPickupSfc, &rect, SDL_MapRGB(mGoodPickupSfc->format, SDL_CLR_BLACK));

	// colour in the bad pickup
	SDL_FillRect(mBadPickupSfc, NULL, SDL_MapRGB(mBadPickupSfc->format, SDL_CLR_BLACK));
	rect.x = PIXELW;
	rect.y = 0;
	rect.w = 2 * PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mBadPickupSfc, &rect, SDL_MapRGB(mBadPickupSfc->format, SDL_CLR_WHITE));
	rect.y += 3 * PIXELH;
	SDL_FillRect(mBadPickupSfc, &rect, SDL_MapRGB(mBadPickupSfc->format, SDL_CLR_WHITE));
	rect.x = 0;
	rect.y = PIXELH;
	rect.w = PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mBadPickupSfc, &rect, SDL_MapRGB(mBadPickupSfc->format, SDL_CLR_WHITE));
	rect.x += 3 * PIXELW;
	SDL_FillRect(mBadPickupSfc, &rect, SDL_MapRGB(mBadPickupSfc->format, SDL_CLR_WHITE));

	return RETURN_SUCCESS;
}


int Play::LoadSoundEffects() {
	char str[256];

	// good pickup noise
	GetAssetName(str, PLAY_GOOD_PICKUP_SND);
	mGoodPickupSnd = Mix_LoadWAV(str);
	if (!mGoodPickupSnd) {
		return RETURN_ERROR;
	}

	// bad pickup noise
	GetAssetName(str, PLAY_BAD_PICKUP_SND);
	mBadPickupSnd = Mix_LoadWAV(str);
	if (!mBadPickupSnd) {
		return RETURN_ERROR;
	}

	// crash noise
	GetAssetName(str, PLAY_CRASH_SND);
	mCrashSnd = Mix_LoadWAV(str);
	if (!mCrashSnd) {
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


void Play::PlaceLevelWalls() {
	mWalls.clear();

	SDL_Rect rect;
	int len;

	switch (rand() % 4) {
	case 0:		// two vertical lines
		GetMiniFramePixel(22, 22, &rect);
		rect.w = 3 * PIXELW;
		rect.h = 80 * PIXELH;
		mWalls.push_back(rect);
		rect.x += (3 + 80) * PIXELW;
		mWalls.push_back(rect);
		break;

	case 1:		// two horizontal lines
		GetMiniFramePixel(22, 22, &rect);
		rect.w = 80 * PIXELW;
		rect.h = 3 * PIXELH;
		mWalls.push_back(rect);
		rect.y += (3 + 80) * PIXELH;
		mWalls.push_back(rect);
		break;

	case 2:		// broken box
		// left side
		len = 35;
		GetMiniFramePixel(22, 22, &rect);
		rect.w = 3 * PIXELW;
		rect.h = len * PIXELH;
		mWalls.push_back(rect);
		rect.y += (80 - len) * PIXELH;
		mWalls.push_back(rect);
		GetMiniFramePixel(22, 22, &rect);
		rect.w = len * PIXELW;
		rect.h = 3 * PIXELH;
		mWalls.push_back(rect);
		rect.y += 80 * PIXELH;
		mWalls.push_back(rect);

		// right side
		GetMiniFramePixel(102, 22, &rect);
		rect.w = 3 * PIXELW;
		rect.h = len * PIXELH;
		mWalls.push_back(rect);
		rect.y += (80 - len) * PIXELH;
		mWalls.push_back(rect);
		GetMiniFramePixel(102 - len, 22, &rect);
		rect.w = len * PIXELW;
		rect.h = 3 * PIXELH;
		mWalls.push_back(rect);
		rect.y += 80 * PIXELH;
		rect.w += 3 * PIXELW;
		mWalls.push_back(rect);
		break;

	case 3:		// pens
		// left side
		len = 30;
		GetMiniFramePixel(22 + len, 22, &rect);
		rect.w = 3 * PIXELW;
		rect.h = len * PIXELH;
		mWalls.push_back(rect);
		rect.y += (80 - len) * PIXELH;
		mWalls.push_back(rect);
		GetMiniFramePixel(22, 22 + len, &rect);
		rect.w = (len + 3) * PIXELW;
		rect.h = 3 * PIXELH;
		mWalls.push_back(rect);
		rect.y += (80 - 2 * len) * PIXELH;
		mWalls.push_back(rect);

		// right side
		GetMiniFramePixel(102 - len, 22, &rect);
		rect.w = 3 * PIXELW;
		rect.h = len * PIXELH;
		mWalls.push_back(rect);
		rect.y += (80 - len) * PIXELH;
		mWalls.push_back(rect);
		GetMiniFramePixel(102 - len, 22 + len, &rect);
		rect.w = len * PIXELW;
		rect.h = 3 * PIXELH;
		mWalls.push_back(rect);
		rect.y += (80 - 2 * len) * PIXELH;
		mWalls.push_back(rect);
		break;
	}
}


void Play::PlaceAllPickups() {
	mGoodPickups.clear();
	mBadPickups.clear();

	// a rectangle representing the player's start area, plus a small buffer
	SDL_Rect centreRect;
	GetMiniFramePixel(MINIFRAME_SZ / 2, MINIFRAME_SZ / 2, &centreRect);
	centreRect.x -= 8 * PIXELW;
	centreRect.y -= 8 * PIXELH;
	centreRect.w = 16 * PIXELW;
	centreRect.h = 16 * PIXELH;

	// place all the good pickups
	SDL_Rect rect;
	bool goodrect;
	for (Uint32 i = 0; i < PLAY_NUM_GOOD_PICKUPS; ++i) {
		do {
			goodrect = true;
			int x = rand() % (MINIFRAME_SZ - 4);
			int y = rand() % (MINIFRAME_SZ - 4);
			GetMiniFramePixel(x, y, &rect);
			rect.w = mGoodPickupSfc->w + 2 * PIXELW;	// add buffer so they don't touch
			rect.h = mGoodPickupSfc->h + 2 * PIXELH;

			// check that we're out of the player's starting buffer zone
			if (RectanglesOverlap(rect, centreRect)) {
				goodrect = false;
			}

			// does this rect intersect previous good pickups?
			for (unsigned int p = 0; p < mGoodPickups.size() && goodrect; ++p) {
				if (RectanglesOverlap(rect, mGoodPickups[p])) {
					goodrect = false;
				}
			}

			// does this rect intersect previous bad pickups?
			for (unsigned int p = 0; p < mBadPickups.size() && goodrect; ++p) {
				if (RectanglesOverlap(rect, mBadPickups[p])) {
					goodrect = false;
				}
			}

			// does this rect intersect any walls?
			for (unsigned int p = 0; p < mWalls.size() && goodrect; ++p) {
				if (RectanglesOverlap(rect, mWalls[p])) {
					goodrect = false;
				}
			}
		} while (!goodrect);

		// remove buffer and store rect
		rect.x += PIXELW;
		rect.y += PIXELH;
		rect.w -= 2 * PIXELW;
		rect.h -= 2 * PIXELH;
		mGoodPickups.push_back(rect);
	}

	// place all the bad pickups
	for (Uint32 i = 0; i < PLAY_NUM_BAD_PICKUPS; ++i) {
		do {
			goodrect = true;
			int x = rand() % (MINIFRAME_SZ - 4);
			int y = rand() % (MINIFRAME_SZ - 4);
			GetMiniFramePixel(x, y, &rect);
			rect.w = mBadPickupSfc->w + 2 * PIXELW;	// add buffer so they don't touch
			rect.h = mBadPickupSfc->h + 2 * PIXELH;

			// check that we're out of the player's starting buffer zone
			if (RectanglesOverlap(rect, centreRect)) {
				goodrect = false;
			}

			// does this rect intersect previous good pickups?
			for (unsigned int p = 0; p < mGoodPickups.size() && goodrect; ++p) {
				if (RectanglesOverlap(rect, mGoodPickups[p])) {
					goodrect = false;
				}
			}

			// does this rect intersect previous bad pickups?
			for (unsigned int p = 0; p < mBadPickups.size() && goodrect; ++p) {
				if (RectanglesOverlap(rect, mBadPickups[p])) {
					goodrect = false;
				}
			}

			// does this rect intersect any walls?
			for (unsigned int p = 0; p < mWalls.size() && goodrect; ++p) {
				if (RectanglesOverlap(rect, mWalls[p])) {
					goodrect = false;
				}
			}
		} while (!goodrect);

		// remove buffer and store rect
		rect.x += PIXELW;
		rect.y += PIXELH;
		rect.w -= 2 * PIXELW;
		rect.h -= 2 * PIXELH;
		mBadPickups.push_back(rect);
	}
}


void Play::MovePlayer() {
	switch (mPlayerDirection) {
	case UP:
		mPlayerFramePos.y -= PLAY_NUM_PIXELS_PER_MOVE;
		if (mPlayerFramePos.y < 0) {
			mPlayerFramePos.y = MINIFRAME_SZ - 3 * PLAY_PLAYER_SIZE_FACTOR;
		}
		break;
	case DN:
		mPlayerFramePos.y += PLAY_NUM_PIXELS_PER_MOVE;
		if (mPlayerFramePos.y >= MINIFRAME_SZ - 2 * PLAY_PLAYER_SIZE_FACTOR) {
			mPlayerFramePos.y = 0;
		}
		break;
	case LT:
		mPlayerFramePos.x -= PLAY_NUM_PIXELS_PER_MOVE;
		if (mPlayerFramePos.x < 0) {
			mPlayerFramePos.x = MINIFRAME_SZ - 3 * PLAY_PLAYER_SIZE_FACTOR;
		}
		break;
	case RT:
		mPlayerFramePos.x += PLAY_NUM_PIXELS_PER_MOVE;
		if (mPlayerFramePos.x >= MINIFRAME_SZ - 2 * PLAY_PLAYER_SIZE_FACTOR) {
			mPlayerFramePos.x = 0;
		}
		break;
	}
}


bool Play::PlayerIntersectsWall() {
	// the player as a rectangle
	SDL_Rect rect;
	GetMiniFramePixel(mPlayerFramePos.x, mPlayerFramePos.y, &rect);
	rect.w = mPlayerSfc[UP]->w;
	rect.h = mPlayerSfc[UP]->h;

	for (unsigned int i = 0; i < mWalls.size(); ++i) {
		if (RectanglesOverlap(rect, mWalls[i])) {
			return true;
		}
	}

	return false;
}


void Play::CheckPlayerPickups() {
	// the player as a rectangle
	SDL_Rect rect;
	GetMiniFramePixel(mPlayerFramePos.x, mPlayerFramePos.y, &rect);
	rect.w = mPlayerSfc[UP]->w;
	rect.h = mPlayerSfc[UP]->h;

	// check all good pickups
	for (unsigned int i = 0; i < mGoodPickups.size(); ++i) {
		if (RectanglesOverlap(rect, mGoodPickups[i])) {
			// erase this pickup from the list
			mGoodPickups.erase(mGoodPickups.begin() + i);

			// give points to the player
			mDM->ModifyPlayerScore(PLAY_O_SCORE);
			mCumulativeScore += PLAY_O_SCORE;

			// play a happy sound!
			Mix_PlayChannel(-1, mGoodPickupSnd, 0);

			// pickups don't overlap, so this is the only one we can intersect
			return;
		}
	}

	// check all bad pickups
	for (unsigned int i = 0; i < mBadPickups.size(); ++i) {
		if (RectanglesOverlap(rect, mBadPickups[i])) {
			// erase this pickup from the list
			mBadPickups.erase(mBadPickups.begin() + i);

			// give points to the player
			mDM->ModifyPlayerScore(PLAY_X_SCORE);
			mCumulativeScore += PLAY_X_SCORE;

			// play a sad sound!
			Mix_PlayChannel(-1, mBadPickupSnd, 0);

			// pickups don't overlap, so this is the only one we can intersect
			return;
		}
	}
}


int Play::_InitialPreload() {
	// set title and instructions
	mMinigameName = "PLAY";
	mInstructions.push_back("button does a rotate!");
	mInstructions.push_back("collect o's with vigour!");
	mInstructions.push_back("avoid x's with vigour!");
	mInstructions.push_back("crashing is sad!");

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_WHITE, SDL_CLR_BLACK);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create PLAY background (%s)!\n", SDL_GetError());
		return RETURN_ERROR;
	}

	// try to create the player sprites
	if (this->GeneratePlayerSprites() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render PLAY player sprites (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// try to create pickup sprites
	if (this->GeneratePickupSprites() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render PLAY pickup sprites (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// load all sound effects
	if (this->LoadSoundEffects() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load PLAY sound effects (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_GOOD;

	return RETURN_SUCCESS;
}


void Play::_NewMinigame() {
	// place the player at the starting position, roughly at centre
	mPlayerFramePos.x = mPlayerFramePos.y = MINIFRAME_SZ / 2 - 3 * PLAY_PLAYER_SIZE_FACTOR / 2;

	// create a new set of walls
	this->PlaceLevelWalls();

	// now place all the pickups
	this->PlaceAllPickups();

	// defaults
	mIsTimed = true;
	mPlayerDirection = UP;
	mTimeBetweenPlayerMove = PLAY_DEFAULT_MOVE_DELAY;
	mLastPlayerMove = SDL_GetTicks();
	mPlayerCrashed = false;
	mCumulativeScore = 0;

	// sound effects
	mPlayCrashSfxChannel = -1;
	mPlayCrashSfxPlayed = false;
	Mix_ChannelFinished(CatchFinishedPlaySounds);
}


void Play::_EndMinigame() {
}


void Play::_Process(Uint32 ticks) {
	// if we've crashed, nothing works
	if (mPlayerCrashed) {
		return;
	}

	// turn the player 90 degrees anti-clockwise if button pressed
	if (mTheButton->PressedThisFrame()) {
		switch (mPlayerDirection) {
		case UP:
			mPlayerDirection = LT;
			break;
		case DN:
			mPlayerDirection = RT;
			break;
		case LT:
			mPlayerDirection = DN;
			break;
		case RT:
			mPlayerDirection = UP;
			break;
		}
	}
}


void Play::_Update(Uint32 ticks) {
	// player crashed, do nothing but just wait it out
	if (mPlayerCrashed) {
		// if the final noise has played, finish this minigame
		if (mPlayCrashSfxPlayed) {
			this->NotifyDMMinigameComplete();
		}
		return;
	}

	Uint32 now = SDL_GetTicks();

	// check if the player needs moving
	if (now - mLastPlayerMove > mTimeBetweenPlayerMove) {
		this->MovePlayer();

		// we moved the player, so check if they hit a wall
		if (this->PlayerIntersectsWall()) {
			mPlayerCrashed = true;
			mMinigameTimer.Stop();
			mPlayCrashSfxChannel = Mix_PlayChannel(-1, mCrashSnd, 0);

			// remove ALL points allocated this round
			mDM->ModifyPlayerScore(-mCumulativeScore);
		} else {	// they haven't hit a wall, so check if they picked anything up
			this->CheckPlayerPickups();
		}

		mLastPlayerMove = now;
	}

}


void Play::_Render(Uint32 ticks) {
	// draw our background image
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// render the player
	SDL_Rect rect;
	GetMiniFramePixel(mPlayerFramePos.x, mPlayerFramePos.y, &rect);
	SDL_BlitSurface(mPlayerSfc[mPlayerDirection], NULL, mScreen, &rect);

	// draw all the walls
	for (unsigned int i = 0; i < mWalls.size(); ++i) {
		SDL_FillRect(mScreen, &mWalls[i], SDL_MapRGB(mScreen->format, SDL_CLR_BLACK));
	}

	// draw all good pickups
	for (unsigned int i = 0; i < mGoodPickups.size(); ++i) {
		SDL_BlitSurface(mGoodPickupSfc, NULL, mScreen, &mGoodPickups[i]);
	}

	// draw all bad pickups
	for (unsigned int i = 0; i < mBadPickups.size(); ++i) {
		SDL_BlitSurface(mBadPickupSfc, NULL, mScreen, &mBadPickups[i]);
	}
}


int Play::_Cleanup() {
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Play::Play() {
}


Play::~Play() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Play::Instance() {
	static Play mInstance;
	return &mInstance;
}
