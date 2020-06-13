#include <SDL_image.h>
#include <SDL_ttf.h>

#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "minigames/Fuck.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// timing
const Uint32 SEX_TICK_TIME = 40;	// ticking sex clock (in milliseconds)
const Uint32 SEX_START_TICKS_PER_LOWER = 1;	// how many ticks between lowering?
const Uint32 SEX_MIN_TICKS_PER_LOWER = 1;
const Uint32 SEX_START_TICKS_PER_RELEASE = 14;	// how many ticks between release
const Uint32 SEX_MIN_TICKS_PER_RELEASE = 10;

// paddles
const int SEX_PADDLE_W = 30 * PIXELW;		// width of paddles
const int SEX_PADDLE_H = 8 * PIXELH;		// height of paddles
const int SEX_ME_OUT_X = 136;					// player paddle X positions
const int SEX_ME_IN_X = 136 + SEX_PADDLE_W;
const int SEX_YOU_OUT_X = 632 - SEX_PADDLE_W;	// partner paddle X positions
const int SEX_YOU_IN_X = 632 - 2 * SEX_PADDLE_W;
const int SEX_PADDLE_AROUSAL_LIFT = 16;		// how high we lift paddles on arousal

// pellets
const float SEX_PELLET_POINTS = 2;	// points earned per pellet caught
const float SEX_AROUSE_POINTS = 5;
const int SEX_PELLET_MAX_Y = 139 * PIXELH;	// furthest down pellet can go
const int SEX_PELLET_PER_PERSON = 10;	// how many pellets we track per paddle
const int SEX_PELLET_DROP_DIST = 4;	// how far a pellet drops per turn

// probabilities
const int SEX_PLAYER_IN_ALPHA = 950;	// probability in 1000 that player pellet is "in"
const int SEX_AROUSE_PELLET_ODDS = 4;	// 1-in-X odds that pellet will arouse

// arousal
const Uint32 SEX_MAX_NO_AROUSAL = SEX_AROUSE_PELLET_ODDS;	// ensure we get aroused often enough
const int SEX_AROUSALS_TO_ORGASM = 4;		// how many times do we get aroused before we orgasm?
const Uint32 SEX_TICKS_AFTER_ORGASM = 3000;	// how much time we have left after orgasm happens
const float SEX_ORGASM_POINTS = 75;
const float SEX_DOUBLE_ORGASM_POINTS = 150;

// sound effects
const char* SEX_PELLET_SOUND = "sounds/fuck/pellet.ogg";
const char* SEX_AROUSE_SOUND = "sounds/fuck/arouse.ogg";
const char* SEX_ORGASM_SOUND = "sounds/fuck/orgasm.ogg";
const char* SEX_DOUBLE_ORGASM_SOUND = "sounds/fuck/doubleorgasm.ogg";


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Fuck::SexTick() {
	mLoweringTicks = (mLoweringTicks + 1) % mTicksPerLower;
	mReleaseTicks = (mReleaseTicks + 1) % mTicksPerRelease;

	if (mLoweringTicks == 0) {
		this->LowerPellets();
	}

	if (mReleaseTicks == 0) {
		this->ReleasePellet();
	}
}


int Fuck::GeneratePellets() {
	// create the surfaces
	mNormalPelletSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	mArousePelletSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 4 * PIXELW, 4 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mNormalPelletSfc || !mArousePelletSfc) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create FUCK PELLETS (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// colour the surfaces
	SDL_Rect rect;
	SDL_FillRect(mNormalPelletSfc, NULL, SDL_MapRGB(mNormalPelletSfc->format, SDL_CLR_BLACK));	// "normal" is just black square
	SDL_FillRect(mArousePelletSfc, NULL, SDL_MapRGB(mArousePelletSfc->format, SDL_CLR_WHITE));	// "arouse" is black circle on white
	rect.x = PIXELW;
	rect.y = 0;
	rect.w = 2 * PIXELW;
	rect.h = PIXELH;
	SDL_FillRect(mArousePelletSfc, &rect, SDL_MapRGB(mArousePelletSfc->format, SDL_CLR_BLACK));	// top bar
	rect.y = 3 * PIXELH;
	SDL_FillRect(mArousePelletSfc, &rect, SDL_MapRGB(mArousePelletSfc->format, SDL_CLR_BLACK));	// bottom bar
	rect.x = 0;
	rect.y = PIXELH;
	rect.w = PIXELW;
	rect.h = 2 * PIXELH;
	SDL_FillRect(mArousePelletSfc, &rect, SDL_MapRGB(mArousePelletSfc->format, SDL_CLR_BLACK));	// left bar
	rect.x = 3 * PIXELW;
	SDL_FillRect(mArousePelletSfc, &rect, SDL_MapRGB(mArousePelletSfc->format, SDL_CLR_BLACK));	// right bar

	return RETURN_SUCCESS;
}


int Fuck::GenerateOrgasmSurface() {
	mOrgasmSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 62 * PIXELW, 126 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mOrgasmSfc) {
		return RETURN_ERROR;
	}

	// create a happy orgasm sign
	SDL_Surface* temp = TTF_RenderText_Solid(SMALL_FONT, "ORGASM!", SDL_CLR_WHITE);
	if (!temp) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render FUCK orgasm text (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// colour in the surface
	SDL_FillRect(mOrgasmSfc, NULL, SDL_MapRGB(mOrgasmSfc->format, SDL_CLR_BLACK));
	SDL_Rect rect;
	rect.x = mOrgasmSfc->w / 2 - temp->w / 2;
	rect.y = mOrgasmSfc->h / 2 - temp->h / 2;
	SDL_BlitSurface(temp, NULL, mOrgasmSfc, &rect);

	// dispose of temp surface
	GarbageGobbler::Instance()->Discard(temp);

	return RETURN_SUCCESS;
}


int Fuck::LoadSoundEffects() {
	char str[256];

	GetAssetName(str, SEX_PELLET_SOUND);
	mPelletSound = Mix_LoadWAV(str);
	GetAssetName(str, SEX_AROUSE_SOUND);
	mArouseSound = Mix_LoadWAV(str);
	GetAssetName(str, SEX_ORGASM_SOUND);
	mOrgasmSound = Mix_LoadWAV(str);
	GetAssetName(str, SEX_DOUBLE_ORGASM_SOUND);
	mDoubleOrgasmSound = Mix_LoadWAV(str);

	if (!mPelletSound || !mArouseSound || !mOrgasmSound || !mDoubleOrgasmSound) {
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


void Fuck::ReleasePellet() {
	int ind;

	// everything below is a magic number, really
	if (mPelletPlayerSide) {	// release a pellet on the player's side
		// no need to release a pellet if we're orgasming
		if (mPlayerOrgasm) {
			mPelletPlayerSide = !mPelletPlayerSide;
			return;
		}

		ind = mFreePlayerPellets.front();
		mFreePlayerPellets.pop();

		if (rand() % 1000 <= SEX_PLAYER_IN_ALPHA) {
			// pellet is "in"...i.e., while the player thrusts
			mPlayerPellets[ind].in = true;
			GetMiniFramePixel(45, 0, &mPlayerPellets[ind].rect);
		} else {
			// pellet is "out"...i.e., while the player pulls back
			mPlayerPellets[ind].in = false;
			GetMiniFramePixel(14, 0, &mPlayerPellets[ind].rect);
		}
		mPlayerPellets[ind].active = true;
		if (rand() % SEX_AROUSE_PELLET_ODDS != 0)
			mPlayerPellets[ind].type = NORMAL_PELLET;
		else
			mPlayerPellets[ind].type = AROUSE_PELLET;

		// check if we haven't gone too long without arousal
		if (mPlayerPellets[ind].type != AROUSE_PELLET) {
			if (++mPlayerTurnsNoArouse >= SEX_MAX_NO_AROUSAL) {
				mPlayerTurnsNoArouse = 0;
				mPlayerPellets[ind].type = AROUSE_PELLET;
			}
		}

	} else {	// release a pellet on the partner's side
		// no need to release a pellet if we're orgasming
		if (mPartnerOrgasm) {
			mPelletPlayerSide = !mPelletPlayerSide;
			return;
		}

		ind = mFreePartnerPellets.front();
		mFreePartnerPellets.pop();

		if (rand() % 1000 > SEX_PLAYER_IN_ALPHA) {
			// pellet is "in"...i.e., while the partner thrusts
			mPartnerPellets[ind].in = true;
			GetMiniFramePixel(77, 0, &mPartnerPellets[ind].rect);
		} else {
			// pellet is "out"...i.e., while the partner pulls back
			mPartnerPellets[ind].in = false;
			GetMiniFramePixel(108, 0, &mPartnerPellets[ind].rect);
		}
		mPartnerPellets[ind].active = true;
		if (rand() % SEX_AROUSE_PELLET_ODDS != 0)
			mPartnerPellets[ind].type = NORMAL_PELLET;
		else
			mPartnerPellets[ind].type = AROUSE_PELLET;

		// check if we haven't gone too long without arousal
		if (mPartnerPellets[ind].type != AROUSE_PELLET) {
			if (++mPartnerTurnsNoArouse >= SEX_MAX_NO_AROUSAL) {
				mPartnerTurnsNoArouse = 0;
				mPartnerPellets[ind].type = AROUSE_PELLET;
			}
		}
	}

	// switch sides
	mPelletPlayerSide = !mPelletPlayerSide;
}


void Fuck::LowerPellets() {
	// move player pellets downwards
	if (!mPlayerOrgasm) {
		for (int i = 0; i < SEX_PELLET_PER_PERSON; ++i) {
			if (mPlayerPellets[i].active) {
				mPlayerPellets[i].rect.y += SEX_PELLET_DROP_DIST * PIXELH;
				this->CheckPellet(i, true);
			}
		}
	}

	// move partner pellets downwards
	if (!mPartnerOrgasm) {
		for (int i = 0; i < SEX_PELLET_PER_PERSON; ++i) {
			if (mPartnerPellets[i].active) {
				mPartnerPellets[i].rect.y += SEX_PELLET_DROP_DIST * PIXELH;
				this->CheckPellet(i, false);
			}
		}
	}
}


void Fuck::CheckPellet(int ind, bool isPlayer) {
	// get our comparisons ready
	SexPellet* pArray;
	queue<int>* q;
	SDL_Rect* paddle;
	if (isPlayer) {
		paddle = &mPlayer;
		pArray = mPlayerPellets;
		q = &mFreePlayerPellets;
	} else {
		paddle = &mPartner;
		pArray = mPartnerPellets;
		q = &mFreePartnerPellets;
	}

	// check if pellet went out of bounds
	if (pArray[ind].rect.y > SEX_PELLET_MAX_Y) {
		pArray[ind].active = false;
		q->push(ind);
		return;
	}

	// check if pellet and paddle are on same plane
	if ((pArray[ind].in && mThrusting) || (!pArray[ind].in && !mThrusting)) {
		if (pArray[ind].rect.y < paddle->y &&
			pArray[ind].rect.y + mNormalPelletSfc->h > paddle->y) {	// if so, paddle gets pellet
			switch (pArray[ind].type) {
			case NORMAL_PELLET:
				mDM->ModifyPlayerScore((float)SEX_PELLET_POINTS);
				Mix_PlayChannel(-1, mPelletSound, 0);
				break;
			case AROUSE_PELLET:
				Mix_PlayChannel(-1, mArouseSound, 0);
				if (isPlayer) {
					this->ArousePlayer();
				} else {
					this->ArousePartner();
				}
				break;
			}
			pArray[ind].active = false;
			q->push(ind);
		} //end if(paddle catches pellet)
	}
}


void Fuck::ArousePlayer() {
	mPlayer.y -= SEX_PADDLE_AROUSAL_LIFT * PIXELH;

	if (++mPlayerNumArouse >= SEX_AROUSALS_TO_ORGASM) {
		// if the partner has already orgasm'd, get double points!
		if (mPartnerOrgasm) {
			mDM->ModifyPlayerScore(SEX_DOUBLE_ORGASM_POINTS);
			Mix_PlayChannel(-1, mDoubleOrgasmSound, 0);
		} else {	// otherwise, simple orgasm gets normal points
			mDM->ModifyPlayerScore(SEX_ORGASM_POINTS);
			Mix_PlayChannel(-1, mOrgasmSound, 0);
		}

		mPlayerOrgasm = true;
		this->OrgasmShortenTime();
	}

	// should we speed up lowering pellets?
	if (--mTicksPerLower < SEX_MIN_TICKS_PER_LOWER) {
		mTicksPerLower = SEX_MIN_TICKS_PER_LOWER;
	}

	// should we speed up releasing pellets?
	if (--mTicksPerRelease < SEX_MIN_TICKS_PER_RELEASE) {
		mTicksPerRelease = SEX_MIN_TICKS_PER_RELEASE;
	}
}


void Fuck::ArousePartner() {
	mPartner.y -= SEX_PADDLE_AROUSAL_LIFT * PIXELH;
	if (++mPartnerNumArouse >= SEX_AROUSALS_TO_ORGASM) {
		// if the player has already orgasm'd, get double points!
		if (mPlayerOrgasm) {
			mDM->ModifyPlayerScore(SEX_DOUBLE_ORGASM_POINTS);
			Mix_PlayChannel(-1, mDoubleOrgasmSound, 0);
		} else {	// otherwise, simple orgasm gets normal points
			mDM->ModifyPlayerScore(SEX_ORGASM_POINTS);
			Mix_PlayChannel(-1, mOrgasmSound, 0);
		}

		mPartnerOrgasm = true;
		this->OrgasmShortenTime();
	}
}


void Fuck::OrgasmShortenTime() {
	if (mMinigameTimer.GetTicks() < MINIGAME_DEFAULT_LENGTH - SEX_TICKS_AFTER_ORGASM) {
		mMinigameTimer.SetTicks(MINIGAME_DEFAULT_LENGTH - SEX_TICKS_AFTER_ORGASM);
	}
}


void Fuck::RenderMates() {
	// draw player
	if (!mPlayerOrgasm) {
		SDL_FillRect(mScreen, &mPlayer, SDL_MapRGB(mScreen->format, SDL_CLR_BLACK));
	}

	// draw partner
	if (!mPartnerOrgasm) {
		SDL_FillRect(mScreen, &mPartner, SDL_MapRGB(mScreen->format, SDL_CLR_BLACK));
	}
}


void Fuck::RenderPellets() {
	// player pellets
	if (!mPlayerOrgasm) {
		for (int i = 0; i < SEX_PELLET_PER_PERSON; ++i) {
			if (mPlayerPellets[i].active) {
				switch (mPlayerPellets[i].type) {
				case NORMAL_PELLET:
					SDL_BlitSurface(mNormalPelletSfc, NULL, mScreen, &mPlayerPellets[i].rect);
					break;
				case AROUSE_PELLET:
					SDL_BlitSurface(mArousePelletSfc, NULL, mScreen, &mPlayerPellets[i].rect);
					break;
				}
			}
		}
	}

	// partner pellets
	if (!mPartnerOrgasm) {
		for (int i = 0; i < SEX_PELLET_PER_PERSON; ++i) {
			if (mPartnerPellets[i].active) {
				switch (mPartnerPellets[i].type) {
				case NORMAL_PELLET:
					SDL_BlitSurface(mNormalPelletSfc, NULL, mScreen, &mPartnerPellets[i].rect);
					break;
				case AROUSE_PELLET:
					SDL_BlitSurface(mArousePelletSfc, NULL, mScreen, &mPartnerPellets[i].rect);
					break;
				}
			}
		}
	}
}


int Fuck::_InitialPreload() {
	// set title and instructions
	mMinigameName = "FUCK";
	mInstructions.push_back("square pellets make points!");
	mInstructions.push_back("circle pellets make sexy!");
	mInstructions.push_back("double orgasm for most points!");

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_WHITE, SDL_CLR_BLACK);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create Sex background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// draw the line down the centre of the frame
	SDL_Rect rect;
	rect.x = 380;
	rect.y = 51;
	rect.w = 2 * PIXELW;
	rect.h = 378;
	SDL_FillRect(mBackground, &rect, SDL_MapRGB(mBackground->format, SDL_CLR_BLACK));

	// draw some tick-marks to show orgasm potential
	for (int i = 0; i < 5; ++i) {
		GetMiniFramePixel(60, MINIFRAME_SZ - 12 - i * SEX_PADDLE_AROUSAL_LIFT, &rect);
		rect.w = 6 * PIXELW;
		rect.h = 2 * PIXELH;
		SDL_FillRect(mBackground, &rect, SDL_MapRGB(mBackground->format, SDL_CLR_BLACK));
	}

	// draw text to show where orgasm happens
	SDL_Surface* temp = TTF_RenderText_Solid(SMALL_FONT, "orgasm", SDL_CLR_BLACK);
	if (!temp) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render FUCK background text (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	GetMiniFramePixel(18, 55, &rect);
	rect.x -= 2;
	SDL_BlitSurface(temp, NULL, mBackground, &rect);
	rect.x += 253;
	SDL_BlitSurface(temp, NULL, mBackground, &rect);
	GarbageGobbler::Instance()->Discard(temp);

	// load the good and bad pellets
	if (this->GeneratePellets() != RETURN_SUCCESS) {
		return RETURN_ERROR;
	}

	// create the orgasm sign
	if (this->GenerateOrgasmSurface() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create FUCK orgasm surface (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// load the sound effects
	if (this->LoadSoundEffects() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load FUCK sound effects (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// default
	mGameType = MINIGAME_GOOD;

	return RETURN_SUCCESS;
}


void Fuck::_NewMinigame() {
	// place me and you
	mPlayer.x = SEX_ME_OUT_X;
	mPartner.x = SEX_YOU_OUT_X;
	mPlayer.y = mPartner.y = 429 - SEX_PADDLE_H - PIXELH;
	mPlayer.w = mPartner.w = SEX_PADDLE_W;
	mPlayer.h = mPartner.h = SEX_PADDLE_H;

	// initialise our pellets and the list of free pellets
	for (int i = 0; i < SEX_PELLET_PER_PERSON; ++i) {
		mPlayerPellets[i].active = false;
		mFreePlayerPellets.push(i);

		mPartnerPellets[i].active = false;
		mFreePartnerPellets.push(i);
	}

	// get our pellet timing right
	mLastTick = mLastLower = mLastRelease = SDL_GetTicks();
	mTicksPerLower = SEX_START_TICKS_PER_LOWER;
	mTicksPerRelease = SEX_START_TICKS_PER_RELEASE;

	// defaults
	mIsTimed = true;
	mThrusting = false;
	mPlayerTurnsNoArouse = mPartnerTurnsNoArouse = 0;
	mPelletPlayerSide = true;
	mPlayerNumArouse = mPartnerNumArouse = 0;
	mPlayerOrgasm = mPartnerOrgasm = false;
}


void Fuck::_EndMinigame() {
	// clear the lists of free pellets
	while (!mFreePlayerPellets.empty()) {
		mFreePlayerPellets.pop();
	}
	while (!mFreePartnerPellets.empty()) {
		mFreePartnerPellets.pop();
	}
}


void Fuck::_Process(Uint32 ticks) {
	mThrusting = mTheButton->IsDown();

	if (mThrusting) {	// thrusting (button is down)
		mPlayer.x = SEX_ME_IN_X;
		mPartner.x = SEX_YOU_IN_X;
	} else {	// not thrusting (button is up)
		mPlayer.x = SEX_ME_OUT_X;
		mPartner.x = SEX_YOU_OUT_X;
	}
}


void Fuck::_Update(Uint32 ticks) {
	// tick the clock
	Uint32 now = SDL_GetTicks();
	if (now - mLastTick >= SEX_TICK_TIME) {
		mLastTick = now;
		this->SexTick();
	}
}


void Fuck::_Render(Uint32 ticks) {
	// render the background
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// draw the SEX PELLETS YEAH
	this->RenderPellets();

	// draw everyone involved
	this->RenderMates();

	// draw any orgasm signs we need
	SDL_Rect rect;
	if (mPlayerOrgasm) {
		GetMiniFramePixel(0, 0, &rect);
		SDL_BlitSurface(mOrgasmSfc, NULL, mScreen, &rect);
	}
	if (mPartnerOrgasm) {
		GetMiniFramePixel(64, 0, &rect);
		SDL_BlitSurface(mOrgasmSfc, NULL, mScreen, &rect);
	}
}


int Fuck::_Cleanup() {
	// get a towel
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Fuck::Fuck() {
	mPlayerPellets = new SexPellet[SEX_PELLET_PER_PERSON];
	mPartnerPellets = new SexPellet[SEX_PELLET_PER_PERSON];
}


Fuck::~Fuck() {
	// delete our pellet lists
	delete[] mPlayerPellets;
	delete[] mPartnerPellets;
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Fuck::Instance() {
	static Fuck mInstance;
	return &mInstance;
}
