#include <iostream>
#include <math.h>

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "minigames/Born.h"
#include "minigames/Dead.h"
#include "minigames/Fuck.h"
#include "minigames/Fuzz.h"
#include "minigames/Kids.h"
#include "minigames/Play.h"
#include "minigames/Pray.h"
#include "minigames/Shop.h"
#include "minigames/Time.h"
#include "minigames/Work.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// drawing
const int TIME_TICK_SIZE = 10;
const int TIME_TICK_RADIUS	= 200;
const int TIME_FONT_SIZE = 48;
const int TIME_LABEL_OFFSET = 20;
const int TIME_GREY_COLOUR = 75;

// timing
const Uint32 TIME_BETWEEN_TICKS = 750;	// in milliseconds


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

int Time::LoadAllClockHands() {
	for (Uint32 i = 0; i < TIME_NUM_CLOCKTICKS; ++i) {
		SDL_Surface* temp;

		// load the hand .png from file
		char path[256];
		switch (i) {
		case 0:
			GetAssetName(path, "images/time/lifehand.png");
			break;
		case 1:
			GetAssetName(path, "images/time/bornhand.png");
			break;
		case 2:
			GetAssetName(path, "images/time/workhand.png");
			break;
		case 3:
			GetAssetName(path, "images/time/playhand.png");
			break;
		case 4:
			GetAssetName(path, "images/time/shophand.png");
			break;
		case 5:
			GetAssetName(path, "images/time/quithand.png");
			break;
		case 6:
			GetAssetName(path, "images/time/fuckhand.png");
			break;
		case 7:
			GetAssetName(path, "images/time/kidshand.png");
			break;
		case 8:
			GetAssetName(path, "images/time/prayhand.png");
			break;
		case 9:
			GetAssetName(path, "images/time/deadhand.png");
			break;
		default:
			char msg[256];
			sprintf(msg, "ERROR: LOADING UNKNOWN CLOCK HAND WTF??\n");
			mLog->LogMsg(msg);
			return RETURN_ERROR;
			break;
		}
		temp = IMG_Load(path);
		if (!temp) {
			char msg[256];
			sprintf(msg, "ERROR: Could not load TIME image (%s)!\n", IMG_GetError());
			mLog->LogMsg(msg);
			return RETURN_ERROR;
		}

		// convert file to usable format
		mClockHands[i] = SDL_DisplayFormat(temp);
		if (!mClockHands[i]) {
			char msg[256];
			sprintf(msg, "ERROR: Could not convert TIME image to proper format (%s)!\n", SDL_GetError());
			mLog->LogMsg(msg);
			return RETURN_ERROR;
		}

		// throw away the temp image we used to load .png
		GarbageGobbler::Instance()->Discard(temp);

		// set the colour key, which we've set to be pure red
		SDL_SetColorKey(mClockHands[i], SDL_SRCCOLORKEY, SDL_MapRGB(mClockHands[i]->format, SDL_CLR_RED));
	}
	
	return RETURN_SUCCESS;
}


void Time::TickClock() {
	mCurrentClockInd = (mCurrentClockInd + 1) % TIME_NUM_CLOCKTICKS;

	// play sound effect
	Mix_PlayChannel(-1, mTickSound, 0);

	mLastTick = SDL_GetTicks();
}


void Time::RenderClockTicks() {
	SDL_Color clr, grey = { TIME_GREY_COLOUR, TIME_GREY_COLOUR, TIME_GREY_COLOUR, 255 };
	SDL_Rect rect;
	rect.w = rect.h = TIME_TICK_SIZE;

	// start at top, move clockwise
	for (int i = 0; i < 10; ++i) {
		float theta = (float)i * 36.0f * PI / 180.0f;
		rect.x = SCREEN_W / 2 - TIME_TICK_SIZE / 2 + TIME_TICK_RADIUS * sin(theta);
		rect.y = SCREEN_H / 2 - TIME_TICK_SIZE / 2 - TIME_TICK_RADIUS * cos(theta);

		// colour the ticks based on whether they are active
		if (mActiveMinigames[i]) {
			clr = SDL_CLR_WHITE;
		} else {
			clr = grey;
		}
		SDL_FillRect(mBackground, &rect, SDL_MapRGB(mBackground->format, clr));
	}
}


void Time::RenderClockLabels() {
	char label[32];
	SDL_Surface* textsfc;
	float theta;
	int tickInd = 0;
	SDL_Color clr, grey = { TIME_GREY_COLOUR, TIME_GREY_COLOUR, TIME_GREY_COLOUR, 255 };
	SDL_Rect rect;

	sprintf(label, "LIFE");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	rect.x = SCREEN_W / 2 - textsfc->w / 2;
	rect.y = 9;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "born");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 17.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) + TIME_TICK_SIZE / 2 + TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "work");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 19.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) + TIME_TICK_SIZE / 2 + TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "play");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) + TIME_TICK_SIZE / 2 + TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "shop");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 3.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) + TIME_TICK_SIZE / 2 + TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "QUIT");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	rect.x = SCREEN_W / 2 - textsfc->w / 2;
	rect.y = SCREEN_H - textsfc->h - 6;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "fuck");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 7.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) - textsfc->w - TIME_TICK_SIZE / 2 - TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "kids");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 9.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) - textsfc->w - TIME_TICK_SIZE / 2 - TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "pray");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 11.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) - textsfc->w - TIME_TICK_SIZE / 2 - TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

	sprintf(label, "dead");
	if (mActiveMinigames[tickInd++]) {
		clr = SDL_CLR_WHITE;
	} else {
		clr = grey;
	}
	textsfc = TTF_RenderText_Solid(SMALL_FONT, label, clr);
	theta = 13.0f * PI / 10.0f;
	rect.x = SCREEN_W / 2 + TIME_TICK_RADIUS * cos(theta) - textsfc->w - TIME_TICK_SIZE / 2 - TIME_LABEL_OFFSET;
	rect.y = SCREEN_H / 2 + TIME_TICK_RADIUS * sin(theta) - textsfc->h / 2;
	SDL_BlitSurface(textsfc, NULL, mScreen, &rect);

}


void Time::RenderClockHand() {
	SDL_BlitSurface(mClockHands[mCurrentClockInd], NULL, mScreen, NULL);
}


int Time::_InitialPreload() {
	// create the background surface
	mBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!mBackground) {
		char msg[256];
		sprintf(msg, "ERROR: Could not create Clock background (%s)!\n", SDL_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}
	SDL_FillRect(mBackground, NULL, SDL_MapRGB(mBackground->format, SDL_CLR_BLACK));

	// try to load all the "hands" .pngs
	this->LoadAllClockHands();

	// load sound effects
	char str[256];
	GetAssetName(str, "sounds/time/tick.ogg");
	mTickSound = Mix_LoadWAV(str);
	if (!mTickSound) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load TIME sound effect (%s)!\n", Mix_GetError());
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// set whether or the minigames are active
	for (Uint32 i = 0; i < TIME_NUM_CLOCKTICKS; ++i) {
		if (LIVELY) {
			mActiveMinigames[i] = true;
		} else {
			if (i == 1) {	// i.e., "BORN"
				mActiveMinigames[i] = true;
			} else {
				mActiveMinigames[i] = false;
			}
		}
	}

	// defaults
	mGameType = MINIGAME_NEUTRAL;

	return RETURN_SUCCESS;
}


void Time::_NewMinigame() {
	// defaults
	mMinigame = NULL;
	mBeginLife = false;
	mQuit = false;
	mLastTick = SDL_GetTicks();
	mCurrentClockInd = 0;
	mIsTimed = false;
}


void Time::_EndMinigame() {
}


void Time::_Process(Uint32 ticks) {
	// figure out which minigame was selected, and whither it leads
	if (TheButton::Instance()->PressedThisFrame()) {
		switch (mCurrentClockInd) {
		case 0:	// LIFE
			mBeginLife = true;
			this->NotifyDMMinigameComplete();
			break;
		case 1:	// BORN
			mMinigame = Born::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 2:	// WORK
			mMinigame = Work::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 3:	// PLAY
			mMinigame = Play::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 4:	// SHOP
			mMinigame = Shop::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 5:	// QUIT
			mQuit = true;
			mMinigame = NULL;
			this->NotifyDMMinigameComplete();
			break;
		case 6:	// FUCK
			mMinigame = Fuck::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 7:	// KIDS
			mMinigame = Kids::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 8:	// PRAY
			mMinigame = Pray::Instance();
			this->NotifyDMMinigameComplete();
			break;
		case 9:	// DEAD
			mMinigame = Dead::Instance();
			this->NotifyDMMinigameComplete();
			break;
		default:
			char msg[256];
			sprintf(msg, "ERROR: WTF HOW DID TIME GO TO THIS MINIGAME?\n");
			mLog->LogMsg(msg);
			SDL_Quit();
			break;
		}
	}
}


void Time::_Update(Uint32 ticks) {
	Uint32 now = SDL_GetTicks();
	if (now - mLastTick > TIME_BETWEEN_TICKS) {
		this->TickClock();
	}
}


void Time::_Render(Uint32 ticks) {
	// render the background (the clock) to the surface
	SDL_BlitSurface(mBackground, NULL, mScreen, NULL);

	// render all tickmarks and labels, and the clock hand
	this->RenderClockTicks();
	this->RenderClockLabels();
	this->RenderClockHand();
}


int Time::_Cleanup() {
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Time::Time() {

}


Time::~Time() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Time::Instance() {
	static Time mInstance;
	return &mInstance;
}


Minigame* Time::GetMinigame() {
	return mMinigame;
}


bool Time::BeginLife() {
	return mBeginLife;
}


bool Time::Quit() {
	return mQuit;
}
