#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "minigames/Pray.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// CONSTANT AND STATIC VARIABLES
///////////////////////////////////////////////////////////////////////////////

const Uint32 PRAY_DONATE_TEXT_X = 39;
const Uint32 PRAY_DONATE_POINTS_X = 82;
const Uint32 PRAY_DONATE_POINTS_Y = 110;
const Uint32 PRAY_DONATION_INC = 5;			// how much each donation is worth
const Uint32 PRAY_DONATION_INC_TIME = 500;	// in milliseconds
const Uint32 PRAY_MAX_DONATION = 9999;

// sound effects
const char* PRAY_DONATE_SNDFILE = "sounds/pray/donation.ogg";
const char* PRAY_FINISHED_SNDFILE = "sounds/pray/praycomplete.ogg";
static bool mPrayerFinishedSfxPlayed;
static int mPrayerFinishedSfxChannel;


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Pray::CatchFinishedPraySounds(int channel) {
	if (channel == mPrayerFinishedSfxChannel) {
		mPrayerFinishedSfxPlayed = true;
	}
}


int Pray::LoadSoundEffects() {
	char str[256];
	GetAssetName(str, PRAY_DONATE_SNDFILE);
	mDonateBeep = Mix_LoadWAV(str);
	GetAssetName(str, PRAY_FINISHED_SNDFILE);
	mVictoryBeep = Mix_LoadWAV(str);

	if (!mDonateBeep || !mVictoryBeep) {
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


int Pray::LoadHandVisuals() {
	char path[256];
	SDL_Surface* sfcTarget;

	for (int i = 0; i < 2; ++i) {
		GetAssetName(path, i == 0 ? "images/pray/prayapart.png" : "images/pray/praytogether.png");

		SDL_Surface* temp = IMG_Load(path);
		if (!temp) {
			char msg[256];
			sprintf(msg, "ERROR: Could not load PRAY image!\n", IMG_GetError());
			mLog->LogMsg(msg);
			return RETURN_ERROR;
		}

		// convert file to usable format
		sfcTarget = SDL_DisplayFormat(temp);
		if (!sfcTarget) {
			char msg[256];
			sprintf(msg, "ERROR: Could not convert PRAY image to proper format (%s)!\n", SDL_GetError());
			mLog->LogMsg(msg);
			return RETURN_ERROR;
		}

		// throw away the temp image we used to load .png
		GarbageGobbler::Instance()->Discard(temp);

		// set the colour key, which we've set to be pure red
		SDL_SetColorKey(sfcTarget, SDL_SRCCOLORKEY, SDL_MapRGB(sfcTarget->format, SDL_CLR_RED));

		if (i == 0) {
			mPrayHandsApart = sfcTarget;
		} else {
			mPrayHandsTogether = sfcTarget;
		}
	}

	return RETURN_SUCCESS;
}


void Pray::ClearDonatedPoints() {
	SDL_Rect rect;
	GetMiniFramePixel(PRAY_DONATE_POINTS_X, PRAY_DONATE_POINTS_Y, &rect);
	rect.w = 160;
	rect.h = 30;
	SDL_FillRect(mBackground, &rect, SDL_MapRGB(mBackground->format, SDL_CLR_BLACK));
}


void Pray::RenderDonatedPoints() {
	// generate a string showing the donation
	char ptStr[8];
	sprintf(ptStr, "%d", mDonatedPoints);
	SDL_Surface* text = TTF_RenderText_Shaded(SMALL_FONT, ptStr, SDL_CLR_WHITE, SDL_CLR_BLACK);
	if (!text) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render PRAY points (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
	}

	// blit the donation to the background
	SDL_Rect rect;
	GetMiniFramePixel(PRAY_DONATE_POINTS_X, PRAY_DONATE_POINTS_Y, &rect);
	SDL_BlitSurface(text, NULL, mBackground, &rect);

	// free our temp surface
	GarbageGobbler::Instance()->Discard(text);
}


void Pray::RenderHands() {
	if (mIsPraying) {
  		SDL_BlitSurface(mPrayHandsTogether, NULL, mScreen, NULL);
	} else {
		SDL_BlitSurface(mPrayHandsApart, NULL, mScreen, NULL);
	}
}


int Pray::_InitialPreload() {
	// set title and instructions
	mMinigameName = "PRAY";
	mInstructions.push_back("hold your skinny hands to");
	mInstructions.push_back("give points to deity!");
	mInstructions.push_back("get an afterlife multiplier!");

	this->LoadHandVisuals();

	// render the background image
	mBackground = DrawMinigameFrame(SDL_CLR_BLACK, SDL_CLR_WHITE);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create PRAY background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// create PRAY instructions
	vector<SDL_Surface*> inst;
	inst.push_back(TTF_RenderText_Solid(NORMAL_FONT, "PRAY", SDL_CLR_WHITE));
	inst.push_back(TTF_RenderText_Solid(SMALL_FONT, "Donation: ", SDL_CLR_WHITE));

	// blit the instructory text to our surface
	mBgSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 126 * PIXELW, ATARI_PIXEL_H / 2 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	SDL_FillRect(mBgSfc, NULL, SDL_MapRGB(mBgSfc->format, SDL_CLR_BLACK));
	if (!mBgSfc) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create PRAY instructions surface (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	SDL_Rect rect;
	for (unsigned int i = 0; i < inst.size(); ++i) {
		if (!inst[i]) {
			char msg[256];
			sprintf(msg, "ERROR: Could not render PRAY text (%s)!\n", TTF_GetError());
			mLog->LogMsg(msg);
			return RETURN_ERROR;
		}

		// blit text to background
		if (i == 0) {	// PRAY title
			rect.x = mBgSfc->w / 2 - inst[0]->w / 2;
			rect.y = PIXELH;
			SDL_BlitSurface(inst[0], NULL, mBgSfc, &rect);

		} else if (i == 1) {	// "Donation" tally
			GetMiniFramePixel(PRAY_DONATE_TEXT_X, PRAY_DONATE_POINTS_Y, &rect);
			rect.w = inst[1]->w;
			rect.h = inst[1]->h;
			SDL_BlitSurface(inst[1], NULL, mBackground, &rect);
		}

		// free temporary surfaces
		GarbageGobbler::Instance()->Discard(inst[i]);
	}

	// create the instruction text
	mInstructionSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 126 * PIXELW, ATARI_PIXEL_H / 2 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	SDL_FillRect(mInstructionSfc, NULL, SDL_MapRGB(mInstructionSfc->format, SDL_CLR_BLACK));
	if (!mInstructionSfc) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create PRAY instruction surface (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	SDL_Surface* temp = TTF_RenderText_Solid(SMALL_FONT, "hold to donate", SDL_CLR_WHITE);
	if (!temp) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render PRAY-instruction text (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	rect.x = mInstructionSfc->w / 2 - temp->w / 2;
	rect.y = mInstructionSfc->h / 2 - temp->h / 2;
	SDL_BlitSurface(temp, NULL, mInstructionSfc, &rect);
	GarbageGobbler::Instance()->Discard(temp);

	// create the text that will be flashed when prayer time is over
	mFinishedSfc = SDL_CreateRGBSurface(SDL_HWSURFACE, 126 * PIXELW, ATARI_PIXEL_H / 2 * PIXELH, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	SDL_FillRect(mFinishedSfc, NULL, SDL_MapRGB(mFinishedSfc->format, SDL_CLR_BLACK));
	if (!mFinishedSfc) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create PRAY finished surface (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	temp = TTF_RenderText_Solid(SMALL_FONT, "praise be!", SDL_CLR_WHITE);
	if (!temp) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render PRAY-finished text (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	rect.x = mFinishedSfc->w / 2 - temp->w / 2;
	rect.y = mFinishedSfc->h / 2 - temp->h / 2;
	SDL_BlitSurface(temp, NULL, mFinishedSfc, &rect);
	GarbageGobbler::Instance()->Discard(temp);

	// load sound effects
	if (this->LoadSoundEffects() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load PRAY sound effects(%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// defaults
	mGameType = MINIGAME_BAD;

	return RETURN_SUCCESS;
}


void Pray::_NewMinigame() {
	// defaults
	mDonatedPoints = 0;
	mFinishedPraying = false;
	mIsPraying = false;
	mLastAddPoints = SDL_GetTicks();
	mIsTimed = false;

	// cover up over any previous donation
	this->ClearDonatedPoints();

	// redraw the instructions to the background
	SDL_Rect rect;
	GetMiniFramePixel(0, 0, &rect);
	SDL_BlitSurface(mBgSfc, NULL, mBackground, &rect);
	this->RenderDonatedPoints();

	// sound effects
	mPrayerFinishedSfxChannel = -1;
	mPrayerFinishedSfxPlayed = false;
	Mix_ChannelFinished(CatchFinishedPraySounds);
}


void Pray::_EndMinigame() {
	// cover up any donation we may have made in the past
	this->ClearDonatedPoints();
}


void Pray::_Process(Uint32 ticks) {
	if (!mFinishedPraying) {
		if (mTheButton->PressedThisFrame()) {	// start praying
			mLastAddPoints = SDL_GetTicks();
			mIsPraying = true;

		} else if (mTheButton->ReleasedThisFrame()) {	// finished praying
			mFinishedPraying = true;
			mIsPraying = false;

			// flash the "finished praying" message
			SDL_Rect rect;
			GetMiniFramePixel(0, 20, &rect);
			SDL_BlitSurface(mFinishedSfc, NULL, mBackground, &rect);

			// play the victory sound
			mPrayerFinishedSfxChannel = Mix_PlayChannel(1, mVictoryBeep, 0);
		}
	}
}


void Pray::_Update(Uint32 ticks) {
	if (mPrayerFinishedSfxPlayed) {
		this->NotifyDMMinigameComplete();
	}

	// see if we should add points to the donation
	if (mTheButton->IsDown() && !mFinishedPraying) {
		Uint32 now = SDL_GetTicks();

		// enough time has passed...donate!
		if (now - mLastAddPoints >= PRAY_DONATION_INC_TIME) {
			mDonatedPoints += PRAY_DONATION_INC;
			if (mDonatedPoints > PRAY_MAX_DONATION) {
				mDonatedPoints = PRAY_MAX_DONATION;
			}

			// add to donation
			mDM->ModifyPlayerScore(-(int)PRAY_DONATION_INC);

			// play a holy sound!
			Mix_PlayChannel(-1, mDonateBeep, 0);

			mLastAddPoints = now;
			this->RenderDonatedPoints();
		}
	}
}


void Pray::_Render(Uint32 ticks) {
	SDL_Rect rect;
	if (mIsPraying) {	// clear it out in black when we're praying
		GetMiniFramePixel(0, 20, &rect);
		rect.w = mInstructionSfc->w;
		rect.h = mInstructionSfc->h;
		SDL_FillRect(mBackground, &rect, SDL_MapRGB(mBackground->format, SDL_CLR_BLACK));
	} else {
		if (!mFinishedPraying) {	// show instruction text at the beginning
			GetMiniFramePixel(0, 20, &rect);
			SDL_BlitSurface(mInstructionSfc, NULL, mBackground, &rect);
		}
	}

	// render the background
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	this->RenderHands();
}


int Pray::_Cleanup() {
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Pray::Pray() {

}


Pray::~Pray() {
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Pray::Instance() {
	static Pray mInstance;
	return &mInstance;
}
