#include <iostream>

#include <SDL_ttf.h>

#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "minigames/Shop.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// player
const int SHOP_PLAYER_STARTX = (33 + 30) * PIXELW;
const int SHOP_PLAYER_STARTY = (17 + 80) * PIXELH;
const int SHOP_PLAYER_SHOPY = (17 + 60) * PIXELH;
const int SHOP_PLAYER_BASE_SZ = 4;
const int SHOP_PLAYER_MAX_SZ = 16;
const int SHOP_PLAYER_GROW_STEP = 2;
const int SHOP_PLAYER_SHRINK_STEP = 1;

// list
const int SHOP_ITEM_MINIFRAME_Y = 60;
const int SHOP_LIST_NUM_ITEMS = 21;
const int SHOP_ITEM_MOVE_STEP = 6;

// timing
const Uint32 SHOP_CLOCK_TICK_TIME = 100;	// in milliseconds
const int SHOP_MODE_CHANGE_TICKS = 15;
const int SHOP_ITEM_MOVE_TICKS = 5;
const int SHOP_SHRINK_TICKS = 10;

// points
const float SHOP_MODE_PTS = 10;
const float SHOP_UNMODE_PTS = -20;

// sound effects
const char* SHOP_MODE_MOVE_SOUND = "sounds/shop/modemove.ogg";
const char* SHOP_MODE_SOUND = "sounds/shop/mode.ogg";
const char* SHOP_UNMODE_SOUND = "sounds/shop/unmode.ogg";


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

Mode GetRandomMode() {
	switch (rand() % 3) {
	case 0:
		return MODE_A;
	case 1:
		return MODE_B;
	case 2:
		return MODE_C;
	}

	return MODE_A;	// should never hit this
}


void Shop::ClockTick() {
	// check on moving the items
	if (++mItemMoveTicks > SHOP_ITEM_MOVE_TICKS) {
		this->MoveAllItems();
	}

	// check on changing the mode
	if (++mModeChangeTicks > SHOP_MODE_CHANGE_TICKS) {
		this->RandomlyChooseMode();
	}

	// check on item release
	if (++mShrinkTicks > SHOP_SHRINK_TICKS) {
		this->PlayerShrinks();
	}

	mLastClockTick = SDL_GetTicks();
}


int Shop::GenerateItemSprites() {
	// allocate them in memory
	mItemSfc[0] = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mItemSfc[1] = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mItemSfc[2] = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);

	if (!mItemSfc[0] || !mItemSfc[1] || !mItemSfc[2]) {
		return RETURN_ERROR;
	}

	// create A
	SDL_Rect rect;
	SDL_FillRect(mItemSfc[0], NULL, SDL_MapRGB(mItemSfc[0]->format, SDL_CLR_WHITE));
	rect.x = rect.y = 0;
	rect.w = PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mItemSfc[0], &rect, SDL_MapRGB(mItemSfc[0]->format, SDL_CLR_BLACK));
	rect.x += 3 * PIXELW;
	SDL_FillRect(mItemSfc[0], &rect, SDL_MapRGB(mItemSfc[0]->format, SDL_CLR_BLACK));
	rect.x = PIXELW;
	rect.y = PIXELH;
	rect.w = 2 * PIXELW;
	SDL_FillRect(mItemSfc[0], &rect, SDL_MapRGB(mItemSfc[0]->format, SDL_CLR_BLACK));
	rect.y += 2 * PIXELH;
	SDL_FillRect(mItemSfc[0], &rect, SDL_MapRGB(mItemSfc[0]->format, SDL_CLR_BLACK));

	// create B
	SDL_FillRect(mItemSfc[1], NULL, SDL_MapRGB(mItemSfc[1]->format, SDL_CLR_WHITE));
	rect.x = 3 * PIXELW;
	rect.y = 0;
	rect.w = PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mItemSfc[1], &rect, SDL_MapRGB(mItemSfc[1]->format, SDL_CLR_BLACK));
	rect.x = PIXELW;
	rect.y = PIXELH;
	SDL_FillRect(mItemSfc[1], &rect, SDL_MapRGB(mItemSfc[1]->format, SDL_CLR_BLACK));
	rect.x += PIXELW;
	rect.y += PIXELH;
	SDL_FillRect(mItemSfc[1], &rect, SDL_MapRGB(mItemSfc[1]->format, SDL_CLR_BLACK));

	// create C
	SDL_FillRect(mItemSfc[2], NULL, SDL_MapRGB(mItemSfc[2]->format, SDL_CLR_WHITE));
	rect.x = rect.y = 0;
	rect.w = PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mItemSfc[2], &rect, SDL_MapRGB(mItemSfc[2]->format, SDL_CLR_BLACK));
	rect.x = PIXELW;
	rect.y = PIXELH;
	rect.w = 3 * PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mItemSfc[2], &rect, SDL_MapRGB(mItemSfc[2]->format, SDL_CLR_BLACK));

	return RETURN_SUCCESS;
}


int Shop::LoadSoundEffects() {
	char str[256];
	GetAssetName(str, SHOP_MODE_MOVE_SOUND);
	mModeMoveSnd = Mix_LoadWAV(str);
	GetAssetName(str, SHOP_MODE_SOUND);
	mModeSnd = Mix_LoadWAV(str);
	GetAssetName(str, SHOP_UNMODE_SOUND);
	mUnmodeSnd = Mix_LoadWAV(str);

	if (!mModeSnd || !mUnmodeSnd) {
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


void Shop::RandomlyChooseMode() {
	mCurrentMode = GetRandomMode();
	mModeChangeTicks = 0;
}


void Shop::IntroduceNewItem() {
	mItemList[SHOP_LIST_NUM_ITEMS - 1].mode = GetRandomMode();
	mItemList[SHOP_LIST_NUM_ITEMS - 1].active = true;
}


void Shop::MoveAllItems() {
	// play a sound effect
	Mix_PlayChannel(-1, mModeMoveSnd, 0);

	// move everything to the left
	for (int i = 0; i < SHOP_LIST_NUM_ITEMS - 1; ++i) {
		mItemList[i].mode = mItemList[i + 1].mode;
		mItemList[i].active = mItemList[i + 1].active;
	}

	// generate a new item at the end of the list
	this->IntroduceNewItem();

	mItemMoveTicks = 0;
}


void Shop::CheckPlayerItems() {
	if (mIsShopping) {
		for (int i = 0; i < SHOP_LIST_NUM_ITEMS; ++i) {
			if (mItemList[i].active) {
				if (RectanglesOverlap(mPlayer, mItemList[i].pos)) {
					if (mItemList[i].mode == mCurrentMode) {
						// points for a-la-mode items
						mDM->ModifyPlayerScore(SHOP_MODE_PTS);

						// play a happy sound
						Mix_PlayChannel(-1, mModeSnd, 0);

					} else { // NOT a-la-mode
						mDM->ModifyPlayerScore(SHOP_UNMODE_PTS);

						// play a not happy sound
						Mix_PlayChannel(-1, mUnmodeSnd, 0);
					}

					// remove item
					mItemList[i].active = false;

					// player gets more stuff
					this->PlayerAccumulates();
				}
			}
		}
	}
}


void Shop::PlayerAccumulates() {
	if (mPlayer.w < SHOP_PLAYER_MAX_SZ * PIXELW) {
		mPlayer.w += SHOP_PLAYER_GROW_STEP * PIXELW;
		mPlayer.h += SHOP_PLAYER_GROW_STEP * PIXELH;
		mShrinkTicks = 0;
	}
}


void Shop::PlayerShrinks() {
	// check if we CAN shrink
	if (mPlayer.w > SHOP_PLAYER_BASE_SZ * PIXELW) {
		mPlayer.w -= SHOP_PLAYER_SHRINK_STEP * PIXELW;
		mPlayer.h -= SHOP_PLAYER_SHRINK_STEP * PIXELH;
	}

	mShrinkTicks = 0;
}


void Shop::RenderCurrentMode() {
	SDL_Rect rect;
	GetMiniFramePixel(30, 41, &rect);

	switch (mCurrentMode) {
	case MODE_A:
		SDL_BlitSurface(mItemSfc[0], NULL, mScreen, &rect);
		break;
	case MODE_B:
		SDL_BlitSurface(mItemSfc[1], NULL, mScreen, &rect);
		break;
	case MODE_C:
		SDL_BlitSurface(mItemSfc[2], NULL, mScreen, &rect);
		break;
	}
}


void Shop::RenderAllItems() {
	for (int i = 0; i < SHOP_LIST_NUM_ITEMS; ++i) {
		if (mItemList[i].active) {
			switch (mItemList[i].mode) {
			case MODE_A:
				SDL_BlitSurface(mItemSfc[0], NULL, mScreen, &mItemList[i].pos);
				break;
			case MODE_B:
				SDL_BlitSurface(mItemSfc[1], NULL, mScreen, &mItemList[i].pos);
				break;
			case MODE_C:
				SDL_BlitSurface(mItemSfc[2], NULL, mScreen, &mItemList[i].pos);
				break;
			}
		}
	}
}


int Shop::_InitialPreload() {
	// set title and instructions
	mMinigameName = "SHOP";
	mInstructions.push_back("buy the current fashion!");
	mInstructions.push_back("don't buy bad fashion!");
	mInstructions.push_back("moderation is key!");

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_BLACK, SDL_CLR_WHITE);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create SHOP background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// render some text to our background
	SDL_Surface* temp = TTF_RenderText_Solid(SMALL_FONT, "Mode:", SDL_CLR_WHITE);
	if (!temp) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render SHOP background text (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	SDL_Rect rect;
	GetMiniFramePixel(1, 40, &rect);
	SDL_BlitSurface(temp, NULL, mBackground, &rect);
	GarbageGobbler::Instance()->Discard(temp);

	// draw all our item types
	if (this->GenerateItemSprites() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create SHOP item sprites (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// create our item list
	mItemList = new ShopItem[SHOP_LIST_NUM_ITEMS];
	for (int i = 0; i < SHOP_LIST_NUM_ITEMS; ++i) {
		GetMiniFramePixel(i * SHOP_ITEM_MOVE_STEP, SHOP_ITEM_MINIFRAME_Y, &mItemList[i].pos);
		mItemList[i].pos.w = mItemSfc[0]->w;
		mItemList[i].pos.h = mItemSfc[0]->h;
		mItemList[i].active = true;
	}

	// load all our sound effects
	if (this->LoadSoundEffects() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load SHOP sound effects (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_BAD;

	return RETURN_SUCCESS;
}


void Shop::_NewMinigame() {
	// choose a starting mode
	this->RandomlyChooseMode();

	// place the player
	mPlayer.x = SHOP_PLAYER_STARTX;
	mPlayer.y = SHOP_PLAYER_STARTY;
	mPlayer.w = SHOP_PLAYER_BASE_SZ * PIXELW;
	mPlayer.h = SHOP_PLAYER_BASE_SZ * PIXELH;

	// randomly fill up item list
	for (int i = 0; i < SHOP_LIST_NUM_ITEMS; ++i) {
		mItemList[i].mode = GetRandomMode();
		mItemList[i].active = true;
	}

	// defaults
	mIsTimed = true;
	mLastClockTick = SDL_GetTicks();
	mItemMoveTicks = mModeChangeTicks = mShrinkTicks = 0;
}


void Shop::_EndMinigame() {
}


void Shop::_Process(Uint32 ticks) {
	if (mTheButton->PressedThisFrame()) {
		mIsShopping = true;
		mPlayer.y = SHOP_PLAYER_SHOPY;
	} else if (mTheButton->ReleasedThisFrame()) {
		mIsShopping = false;
		mPlayer.y = SHOP_PLAYER_STARTY;
	}
}


void Shop::_Update(Uint32 ticks) {
	Uint32 now = SDL_GetTicks();

	// the ticking clock
	if (now - mLastClockTick > SHOP_CLOCK_TICK_TIME) {
		this->ClockTick();
	}

	// check if the player got any items
	this->CheckPlayerItems();
}


void Shop::_Render(Uint32 ticks) {
	// draw our background image
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// draw whatever the current mode is
	this->RenderCurrentMode();

	// draw all the items
	this->RenderAllItems();

	// render the player
	SDL_FillRect(mScreen, &mPlayer, SDL_MapRGB(mScreen->format, SDL_CLR_WHITE));
}


int Shop::_Cleanup() {
	delete[] mItemList;

	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Shop::Shop() {

}


Shop::~Shop() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Shop::Instance() {
	static Shop mInstance;
	return &mInstance;
}
