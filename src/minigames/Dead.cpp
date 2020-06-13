#include <iostream>

#include "common.h"
#include "DungeonMaster.h"
#include "minigames/Dead.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

const int DEAD_NUM_SNOWFLAKES_W = 21;
const int DEAD_SNOWFLAKE_W = 6;
const int DEAD_NUM_SNOWFLAKES_H = 3;
const int DEAD_SNOWFLAKE_H = 6;
const Uint32 DEAD_NUM_SNOWFLAKES = DEAD_NUM_SNOWFLAKES_W * DEAD_NUM_SNOWFLAKES_H;
const int DEAD_MAX_SNOWFLAKE_Y = 143 * PIXELH;
const Uint32 DEAD_NUM_FRAME_FLAKES = 23 + 22 + 22 + 21;

// holding onto snowflakes
const Uint32 DEAD_MAX_NUM_HOLDING = 20;

// timing
const Uint32 DEAD_TIME_BETWEEN_DROP = 125;	// in milliseconds
const Uint32 DEAD_DELAY_BEFORE_END = 2000;	// in milliseconds


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Dead::FindAvailableSnowflake() {
	for (int i = DEAD_NUM_SNOWFLAKES_H - 1; i >= 0; --i) {
		for (int j = DEAD_NUM_SNOWFLAKES_W - 1; j >= 0; --j) {
			int ind = i * DEAD_NUM_SNOWFLAKES_W + j;
			if (mMainSnowflakes[ind].active) {
				if (!mMainSnowflakes[ind].falling) {
					this->DropSnowflake(&mMainSnowflakes[ind]);

					// modify some counters
					mDM->ModifyPlayerScore(-mSnowflakePtVal);
					--mNumFlakesNotDropped;
					mLastDropSnowflake = SDL_GetTicks();
					return;
				}
			}
		}
	}
	// we only get here if there are no more "main" flakes to drop

	// check if we should drop bits of the frame
	if (mIsRealLife) {
		if (!mRandomFrameIndices.empty()) {
			// drop the next random piece of the frame
			int i = mRandomFrameIndices.back();
			mRandomFrameIndices.pop_back();
			this->DropSnowflake(&mFrameSnowflakes[i]);

			// modify some counters
			mDM->ModifyPlayerScore(-mSnowflakePtVal);
			--mNumFlakesNotDropped;
			mLastDropSnowflake = SDL_GetTicks();
			return;
		}
	}
}


void Dead::DropSnowflake(Snowflake* flake) {
	flake->falling = true;
	flake->hover = false;
	flake->xdir = (rand() % 2) ? 1 : -1;
	flake->dx = rand() % 2 + 2;
	flake->dy = rand() % 2 + 2;
	flake->numframes = rand() % 20 + 10;
}


void Dead::UpdateAllSnowflakes() {
	// update main snowflakes
	for (int i = 0; i < DEAD_NUM_SNOWFLAKES_H; ++i) {
		for (int j = 0; j < DEAD_NUM_SNOWFLAKES_W; ++j) {
			int ind = i * DEAD_NUM_SNOWFLAKES_W + j;
			this->UpdateSnowflake(&mMainSnowflakes[ind]);
		}
	}

	// drop any frame flakes, if necessary
	if (mIsRealLife) {
		for (Uint32 i = 0; i < DEAD_NUM_FRAME_FLAKES; ++i) {
			this->UpdateSnowflake(&mFrameSnowflakes[i]);
		}
	}
}


void Dead::UpdateSnowflake(Snowflake* flake) {
	if (flake->active) {
		if (flake->falling) {
			if (flake->numframes == 0) {	// if we've reached a transition
				// if flake was hovering, choose another direction
				if (flake->hover) {
					flake->hover = false;
					flake->xdir *= -1;
					flake->numframes = rand() % 20 + 10;
				} else {	// was moving, hover instead
					flake->hover = true;
					flake->numframes = 5;
				}

			} else {	// no transition, keep falling
				--flake->numframes;
				flake->rect.x += flake->xdir * flake->dx;
				flake->rect.y += flake->dy;
			}
		}

		// check if flake shouldn't be active
		this->CheckFlakeBounds(flake);
	}
}


void Dead::CheckFlakeBounds(Snowflake* flake) {
	if (mIsRealLife) { // drop outside the frame
		if (flake->rect.x < 0 ||
			flake->rect.x >= SCREEN_W - DEAD_SNOWFLAKE_W ||
			flake->rect.y >= SCREEN_H - DEAD_SNOWFLAKE_H) {
			// flake is out of screen bounds
			flake->active = false;
			--mNumFlakesActive;
		}
	} else {	// tutorial, so keep things within the frame
		if (flake->rect.x < 33 * PIXELW ||
			flake->rect.x >= 159 * PIXELW - DEAD_SNOWFLAKE_W ||
			flake->rect.y >= 143 * PIXELH - DEAD_SNOWFLAKE_H) {
			// flake is out of frame bounds
			flake->active = false;
			--mNumFlakesActive;
		}
	}
}


void Dead::RenderAllSnowflakes() {
	// draw main snowflakes
	for (int i = 0; i < DEAD_NUM_SNOWFLAKES_H; ++i) {
		for (int j = 0; j < DEAD_NUM_SNOWFLAKES_W; ++j) {
			int ind = i * DEAD_NUM_SNOWFLAKES_W + j;
			if (mMainSnowflakes[ind].active) {
				mMainSnowflakes[ind].rect.w = DEAD_SNOWFLAKE_W * PIXELW;
				mMainSnowflakes[ind].rect.h = DEAD_SNOWFLAKE_H * PIXELH;
				SDL_FillRect(mScreen, &mMainSnowflakes[ind].rect, SDL_MapRGB(mScreen->format, SDL_CLR_WHITE));
			}
		}
	}

	// draw frame snowflakes
	for (Uint32 i = 0; i < DEAD_NUM_FRAME_FLAKES; ++i) {
		if (mFrameSnowflakes[i].active) {
			mFrameSnowflakes[i].rect.w = DEAD_SNOWFLAKE_W * PIXELW;
			mFrameSnowflakes[i].rect.h = DEAD_SNOWFLAKE_H * PIXELH;
			SDL_FillRect(mScreen, &mFrameSnowflakes[i].rect, SDL_MapRGB(mScreen->format, SDL_CLR_WHITE));
		}
	}
}


int Dead::_InitialPreload() {
	// set title and instructions
	mMinigameName = "DEAD";
	mInstructions.push_back("let go.");

	// render the background image
	mBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create DEAD background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	SDL_FillRect(mBackground, NULL, SDL_MapRGB(mBackground->format, SDL_CLR_BLACK));

	// defaults
	mGameType = MINIGAME_BAD;

	return RETURN_SUCCESS;
}


void Dead::_NewMinigame() {
	// figure out how much each snowflake is worth
	float totalNumFlakes;
	if (mIsRealLife) {
		totalNumFlakes = (float)(DEAD_NUM_SNOWFLAKES + DEAD_NUM_FRAME_FLAKES);
	} else {	// not dropping frame
		totalNumFlakes = (float)DEAD_NUM_SNOWFLAKES;
	}
	mSnowflakePtVal = mDM->GetPlayerScore() / totalNumFlakes;
	mNumFlakesActive = mNumFlakesNotDropped = totalNumFlakes;

	// initialise main snowflakes
	for (int i = 0; i < DEAD_NUM_SNOWFLAKES_H; ++i) {
		for (int j = 0; j < DEAD_NUM_SNOWFLAKES_W; ++j) {
			int ind = i * DEAD_NUM_SNOWFLAKES_W + j;
			mMainSnowflakes[ind].active = true;
			mMainSnowflakes[ind].falling = false;
			GetMiniFramePixel(j * DEAD_SNOWFLAKE_W, i * DEAD_SNOWFLAKE_H, &mMainSnowflakes[ind].rect);
			mMainSnowflakes[ind].rect.w = DEAD_SNOWFLAKE_W * PIXELW;
			mMainSnowflakes[ind].rect.h = DEAD_SNOWFLAKE_H * PIXELH;
		}
	}

	// initialise frame snowflakes
	int fcounter = 0;
	for (Uint32 row = 0; row < 23; ++row) {
		for (Uint32 col = 0; col < 23; ++col) {
			if (row == 0 || row == 22 || col == 0 || col == 22) {
				mFrameSnowflakes[fcounter].active = true;
				mFrameSnowflakes[fcounter].falling = false;
				mFrameSnowflakes[fcounter].rect.x = (27 + col * DEAD_SNOWFLAKE_W) * PIXELW;
				mFrameSnowflakes[fcounter].rect.y = (11 + row * DEAD_SNOWFLAKE_H) * PIXELH;
				mFrameSnowflakes[fcounter].rect.w = DEAD_SNOWFLAKE_W * PIXELW;
				mFrameSnowflakes[fcounter].rect.h = DEAD_SNOWFLAKE_H * PIXELH;
				++fcounter;
			}
		}
	}

	// want to randomly drop frame flakes, if necessary
	if (mIsRealLife) {
		mRandomFrameIndices.clear();
		for (int i = 0; i < (int)DEAD_NUM_FRAME_FLAKES; ++i) {
			mRandomFrameIndices.push_back(i);
		}

		// FISHER-YATES SHUFFLE
		int n = DEAD_NUM_FRAME_FLAKES;
		while (--n > 0) {
			int k = rand() % (n + 1);
			int temp = mRandomFrameIndices[k];
			mRandomFrameIndices[k] = mRandomFrameIndices[n];
			mRandomFrameIndices[n] = temp;
		}
	}

	// defaults
	mLastDropSnowflake = SDL_GetTicks();
	mIsTimed = false;
}


void Dead::_EndMinigame() {

}


void Dead::_Process(Uint32 ticks) {
	if (mTheButton->PressedThisFrame()) {
		// if we press the button, stop dropping and start counting time from here
		++mNumFlakesHeld;
		mLastDropSnowflake = SDL_GetTicks();
	}

	if (mTheButton->ReleasedThisFrame()) {
		// if we release the button, drop all "held" snowflakes
		for (Uint32 i = 0; i < mNumFlakesHeld; ++i) {
			this->FindAvailableSnowflake();
		}
		mNumFlakesHeld = 0;
	}
}


void Dead::_Update(Uint32 ticks) {
	this->UpdateAllSnowflakes();

	// check if it's time to drop a snowflake
	Uint32 now = SDL_GetTicks();
	if (now - mLastDropSnowflake > DEAD_TIME_BETWEEN_DROP) {
		if (mTheButton->IsDown()) {
			// button is held, so check if we can hold any more flakes
			if (mNumFlakesHeld < DEAD_MAX_NUM_HOLDING) {
				++mNumFlakesHeld;
				mLastDropSnowflake = now;
			} else {	// holding max amount of flakes; drop one anyway
				this->FindAvailableSnowflake();
			}
		} else {	// button isn't held, so we can drop a flake normally
			this->FindAvailableSnowflake();
		}
	}

	// if there are no more active snowflakes, finish up.
	if (mNumFlakesActive == 0) {
		if (mIsRealLife) {	// in real life, pause at the end for a few seconds, for art.
			if (-1 == mLastFlakeDisappeared) {
				mLastFlakeDisappeared = SDL_GetTicks();
			} else {
				if (SDL_GetTicks() - mLastFlakeDisappeared > DEAD_DELAY_BEFORE_END) {
					this->NotifyDMMinigameComplete();
				}
			}
		} else {	// in the tutorial, just end immediately
			this->NotifyDMMinigameComplete();
		}
	}
}


void Dead::_Render(Uint32 ticks) {
	// render background image
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// draw all the snowflakes
	this->RenderAllSnowflakes();
}


int Dead::_Cleanup() {
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Dead::Dead() {
	mMainSnowflakes = new Snowflake[DEAD_NUM_SNOWFLAKES];
	mFrameSnowflakes = new Snowflake[DEAD_NUM_FRAME_FLAKES];
}


Dead::~Dead() {
	delete[] mMainSnowflakes;
	delete[] mFrameSnowflakes;
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Dead::Instance() {
	static Dead mInstance;
	return &mInstance;
}
