#include <iostream>

#include <SDL_ttf.h>

#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "Log.h"
#include "minigames/Born.h"
#include "minigames/Dead.h"
#include "minigames/Fuck.h"
#include "minigames/Fuzz.h"
#include "minigames/Kids.h"
#include "minigames/Play.h"
#include "minigames/Pray.h"
#include "minigames/Shop.h"
#include "minigames/Text.h"
#include "minigames/Time.h"
#include "minigames/Work.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// sizing & placement
const Uint32 DM_TIME_BAR_H = 20;	// in pixels
const Uint32 DM_MINIGAME_LABEL_X = 108;
const Uint32 DM_MINIGAME_LABEL_Y = 7;
const Uint32 DM_SCORE_LABEL_START_X = 660;
const Uint32 DM_SCORE_LABEL_Y = 7;

// timing
const Uint32 DM_LIFE_LENGTH = 5 * 60 * 1000;	// in milliseconds


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

int DungeonMaster::InitMinigames() {
	mText = (Text*)Text::Instance();
	mTime = (Time*)Time::Instance();

	// pre-load each minigame
	// each should return RETURN_SUCCESS = 0, so this sum should == 0
	if (mText->InitialPreload() +
		mTime->InitialPreload() +
		Born::Instance()->InitialPreload() +
		Dead::Instance()->InitialPreload() +
		Fuck::Instance()->InitialPreload() +
		Fuzz::Instance()->InitialPreload() +
		Kids::Instance()->InitialPreload() +
		Play::Instance()->InitialPreload() +
		Pray::Instance()->InitialPreload() +
		Shop::Instance()->InitialPreload() +
		Work::Instance()->InitialPreload() != 0) {
		return RETURN_ERROR;
	}

	// place minigame title above-left of the main game frame, Samhayne
	mMinigameNameRect.x = DM_MINIGAME_LABEL_X;
	mMinigameNameRect.y = DM_MINIGAME_LABEL_Y;

	return RETURN_SUCCESS;
}


void DungeonMaster::GenerateTutorialSurfaces() {
	// render the actual text
	mTutSignGood = TTF_RenderText_Shaded(SMALL_FONT, "TUTORIAL", SDL_CLR_BLACK, SDL_CLR_WHITE);
	mTutSignBad = TTF_RenderText_Shaded(SMALL_FONT, "TUTORIAL", SDL_CLR_WHITE, SDL_CLR_BLACK);
	if (!mTutSignGood || !mTutSignBad) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render TUTORIAL labels (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
	}

	// figure out where these signs will be placed
	GetMiniFramePixel(125, 0, &mTutSignRect);
	mTutSignRect.x += 6 * PIXELW - mTutSignGood->w;
	mTutSignRect.y = DM_MINIGAME_LABEL_Y;
}


void DungeonMaster::GenerateScoreSurfaces() {
	// discard the old surfaces
	GarbageGobbler::Instance()->Discard(mPlayerScoreGoodSfc);
	GarbageGobbler::Instance()->Discard(mPlayerScoreBadSfc);
	mPlayerScoreGoodSfc = mPlayerScoreBadSfc = NULL;

	// create the new string
	char scoreStr[256];
	sprintf(scoreStr, "Score: %d", (int)mPlayerScore);

	// render the scores
	mPlayerScoreGoodSfc = TTF_RenderText_Shaded(SMALL_FONT, scoreStr, SDL_CLR_BLACK, SDL_CLR_WHITE);
	mPlayerScoreBadSfc = TTF_RenderText_Shaded(SMALL_FONT, scoreStr, SDL_CLR_WHITE, SDL_CLR_BLACK);
	if (!mPlayerScoreGoodSfc || !mPlayerScoreBadSfc) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render player scores (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
	}

	// place the player score (above-right of minigame frame), adjusting for string length
	mPlayerScoreRect.x = DM_SCORE_LABEL_START_X - mPlayerScoreGoodSfc->w;
	mPlayerScoreRect.y = DM_SCORE_LABEL_Y;
}


void DungeonMaster::SelectHUDInfo() {
	// render the game name, the score, and the TUTORIAL sign as necessary
	switch (mCurrentMinigame->GetMinigameType()) {
	case MINIGAME_GOOD:
		mMinigameNameSfc = TTF_RenderText_Shaded(SMALL_FONT, mCurrentMinigame->GetTitle(), SDL_CLR_BLACK, SDL_CLR_WHITE);
		mTutSignSfc = mTutSignGood;
		mRenderedScoreSfc = mPlayerScoreGoodSfc;
		break;

	case MINIGAME_BAD:
		mMinigameNameSfc = TTF_RenderText_Shaded(SMALL_FONT, mCurrentMinigame->GetTitle(), SDL_CLR_WHITE, SDL_CLR_BLACK);
		mTutSignSfc = mTutSignBad;
		mRenderedScoreSfc = mPlayerScoreBadSfc;
		break;

	case MINIGAME_NEUTRAL:	// definitely do NOT show HUD for TEXT, TIME, or FUZZ
		mRenderedScoreSfc = mTutSignSfc = mMinigameNameSfc = NULL;
		return;
	}

	// check if we successfully rendered the minigame label
	if (!mMinigameNameSfc && mCurrentMinigame->GetMinigameType() != MINIGAME_NEUTRAL) {
		char msg[256];
		sprintf(msg, "ERROR: Could not render minigame name (%s)!\n", TTF_GetError());
		mLog->LogMsg(msg);
	}
}


bool DungeonMaster::IsRealLife() {
	return (mCurrentGameState == STATE_BIRTH ||
			mCurrentGameState == STATE_LIFE ||
			mCurrentGameState == STATE_DEATH);
}


void DungeonMaster::SwitchToMinigame(Minigame* mg) {
	mCurrentMinigame->EndMinigame();
	mCurrentMinigame = mg;
	mCurrentMinigame->NewMinigame(this->IsRealLife());
}


void DungeonMaster::CheckMinigameTimeRemaining() {
	// if minigame time has run out, go back to clock
	if (mCurrentMinigame->IsTimed()) {
		if (mCurrentMinigame->HasTimedOut()) {
			// fuzz out, which will get us back to the clock
			this->SwitchToMinigame(Fuzz::Instance());
		}
	}
}


void DungeonMaster::CheckLifeTimeRemaining() {
	Uint32 elapsedGameTime = SDL_GetTicks() - mLifeStartTime;

	// if too much time has passed, GAME OVER
	if (mCurrentGameState != STATE_DEATH && elapsedGameTime >= DM_LIFE_LENGTH) {
		mCurrentGameState = STATE_DEATH;
		this->SwitchToMinigame(Fuzz::Instance());

	} else {	// otherwise, just render the appropriate game time bar
		mLifeTimeLeftRect.w = (int)(192 * (1 - ((float)elapsedGameTime / (float)DM_LIFE_LENGTH))) * PIXELW;
	}
}


void DungeonMaster::StartLife() {
	mLifeStartTime = SDL_GetTicks();
	mPlayerScore = 0;
	this->GenerateScoreSurfaces();	// re-render player scores
	this->NextLifeMinigame();	// will go to BORN
}


void DungeonMaster::NextLifeMinigame() {
	switch (mCurrentGameState) {
	case STATE_BIRTH:
		mCurrentGameState = STATE_LIFE;
		this->SwitchToMinigame(Born::Instance());
		break;
	case STATE_LIFE:
		switch (rand() % 6) {	// choose a random minigame to go to
		case 0:
			this->SwitchToMinigame(Work::Instance());
			break;
		case 1:
			this->SwitchToMinigame(Play::Instance());
			break;
		case 2:
			this->SwitchToMinigame(Shop::Instance());
			break;
		case 3:
			this->SwitchToMinigame(Fuck::Instance());
			break;
		case 4:
			this->SwitchToMinigame(Kids::Instance());
			break;
		case 5:
			this->SwitchToMinigame(Pray::Instance());
			break;
		}
		break;
	case STATE_DEATH:	// if we're going to death, just die
		this->SwitchToMinigame(Dead::Instance());
		break;
	default:
		char msg[256];
		sprintf(msg, "ERROR: DM attempting to go to new minigame in non-LIFE state!\n");
		mLog->LogMsg(msg);
		break;
	}
}


void DungeonMaster::PrepareTitleText() {
	mCurrentMinigame = mText;
	mText->NewMinigame(this->IsRealLife());
	mText->GiveNormalString("THE AMERICAN DREAM");
	mText->GiveSmallString("Are We Not Drawn");
	mText->GiveSmallString("Onward To New Era?");
	mText->GiveSmallString(" ");

	// formerly...
	//mText->GiveSmallString("a game for GAMM4");
	//mText->GiveSmallString("by");
	//mText->GiveSmallString("newton64");

	// formerly...
	//mText->GiveSmallString("a game for Eastern Bloc\'s");
	//mText->GiveSmallString("\'Housewarming Party\'");
	//mText->GiveSmallString("september 7 to 11, 2011");
	//mText->GiveSmallString("by");
	//mText->GiveSmallString("nick rudzicz");

	mText->GiveSmallString("a game for the world");
	mText->GiveSmallString("by");
	mText->GiveSmallString("nick nick nick nick");
}


void DungeonMaster::PrepareLifeText() {
	mCurrentMinigame = mText;
	mText->NewMinigame(this->IsRealLife());
	mText->GiveNormalString("LIFE");
	mText->GiveNormalString(" ");
	mText->GiveSmallString("engage le jeu,");
	mText->GiveSmallString("que je le gagne.");
}


void DungeonMaster::PrepareGameOverText() {
	mCurrentMinigame = mText;
	mText->NewMinigame(this->IsRealLife());
	mText->GiveNormalString("IN GIRUM IMUS NOCTE");
	mText->GiveNormalString("ET CONSUMMIMUR IGNI");
	mText->GiveNormalString(" ");
	mText->GiveSmallString("we wander in the night");
	mText->GiveSmallString("and are consumed by fire");
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

DungeonMaster::DungeonMaster() {
	mLog = Log::Instance();
}


DungeonMaster::~DungeonMaster() {
}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

DungeonMaster* DungeonMaster::Instance() {
	static DungeonMaster mInstance;
	return &mInstance;
}


int DungeonMaster::InitScreen(Uint32 w, Uint32 h, Uint32 b, Uint32 f) {
	if ((mMainScreen = SDL_SetVideoMode(w, h, b, f)) == NULL) {
		Log* log = Log::Instance();
		char msg[256];
		sprintf(msg, "ERROR: DM could not set video mode (%s)!\n", SDL_GetError());
		log->LogMsg(msg);
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


SDL_Surface* DungeonMaster::GetScreen() {
	return mMainScreen;
}


int DungeonMaster::InitGame() {
	// load all game fonts
	if (LoadAllFonts() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load system fonts!\n");
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// initialise all minigames
	if (this->InitMinigames() != RETURN_SUCCESS) {
		char msg[256];
		sprintf(msg, "ERROR: Could not initialise minigames!\n");
		mLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// render tutorial signs
	this->GenerateTutorialSurfaces();

	// render the player score
	mPlayerScore = 0;
	this->GenerateScoreSurfaces();

	// defaults
	mLifeTimeLeftRect.x = 0;
	mLifeTimeLeftRect.y = SCREEN_H - DM_TIME_BAR_H - PIXELH;
	mLifeTimeLeftRect.h = DM_TIME_BAR_H;

	// display the title screen when we first start up
	mCurrentGameState = STATE_TITLE;
	this->PrepareTitleText();

	return RETURN_SUCCESS;
}


float DungeonMaster::GetPlayerScore() {
	return mPlayerScore;
}


void DungeonMaster::ModifyPlayerScore(float pts) {
	// modify the score
	mPlayerScore += pts;
	if (mPlayerScore < 0) {
		mPlayerScore = 0;
	}

	// redraw the score on the game background
	this->GenerateScoreSurfaces();
	this->SelectHUDInfo();
}


void DungeonMaster::EndMinigame(Minigame* m) {
	if (m == mText) {	// finished showing some kind of text
		switch (mCurrentGameState) {
		case STATE_TITLE:	// finished showing titles, move to TIME
			mCurrentGameState = STATE_CLOCK;
			this->SwitchToMinigame(mTime);
			break;
		case STATE_TUTORIAL:	// finished showing instructions, do minigame tutorial
			this->SwitchToMinigame(mTime->GetMinigame());
			break;
		case STATE_BIRTH:	// shown life text, begin LIFE itself
			this->StartLife();
			break;
		case STATE_GAMEOVER:	// shown GAME OVER, move back to credits
			mCurrentMinigame->EndMinigame();
			mCurrentGameState = STATE_TITLE;
			this->PrepareTitleText();
			break;
		default:
			mLog->LogMsg("WARNING: TEXT ended in strange game state!\n");
			break;
		}

	} else if (m == mTime) {	// looking at the clock; either do minigame tutorial, or begin LIFE
		if (mTime->BeginLife()) {
			// beginning life itself!
			mCurrentGameState = STATE_BIRTH;
			mLog->LogMsg("Go go gadget\n");
			this->PrepareLifeText();

		} else if (mTime->Quit()) {	// QUIT was selected
			return;	// do nothing, wait for DM loop to terminate in main.cpp

		} else { // start a minigame tutorial
			mCurrentGameState = STATE_TUTORIAL;
			this->SwitchToMinigame(mText);	// show minigame instructions

			// give TEXT the title and instructions for the minigame tutorial
			mText->GiveNormalString(mTime->GetMinigame()->GetTitle());
			mText->GiveNormalString(" ");
			vector<const char*> v = mTime->GetMinigame()->GetInstructions();
			for (unsigned int i = 0; i < v.size(); ++i) {
				mText->GiveSmallString(v[i]);
			}
		}

	} else if (m == Fuzz::Instance()) {	// finished fuzzing something out
		switch (mCurrentGameState) {
		case STATE_TUTORIAL:	// fuzz after a tutorial, go back to clock
			mCurrentGameState = STATE_CLOCK;
			this->SwitchToMinigame(mTime);
			break;
		case STATE_BIRTH:		// FALLTHROUGH! Both BIRTH, LIFE, and DEATH move to next minigame
		case STATE_LIFE:
		case STATE_DEATH:
			this->NextLifeMinigame();
			break;
		default:
			mLog->LogMsg("ERROR: Fuzz ended in strange game state!\n");
			SDL_Quit();
			break;
		}

	} else {	// otherwise, this is from a minigame
		if (this->IsRealLife() && m == Dead::Instance()) {
			// if we just "died," then we just finished LIFE
			mCurrentGameState = STATE_GAMEOVER;
			this->PrepareGameOverText();

		} else {
			// otherwise, we just fuzz out after this minigame
			this->SwitchToMinigame(Fuzz::Instance());

			// fuzz shouldn't show unless we finished a tutorial or a LIFE minigame
			if (mCurrentGameState != STATE_TUTORIAL && !this->IsRealLife()) {
				mLog->LogMsg("ERROR: Minigame ended in strange game state!\n");
				SDL_Quit();
			}
		}
	}

	// figure out which scorecard to render
	this->SelectHUDInfo();
}


bool DungeonMaster::Quit() {
	return mTime->Quit();
}


void DungeonMaster::Process(Uint32 ticks) {
	mCurrentMinigame->Process(ticks);
}


void DungeonMaster::Update(Uint32 ticks) {
	// update the current minigame
	mCurrentMinigame->Update(ticks);

	// if we're playing LIFE, we need to check if it has time remaining
	if (this->IsRealLife() && mCurrentMinigame != mText) {
		this->CheckLifeTimeRemaining();
	}

	// if LIFE time hasn't expired, continue and check on minigame time remaining
	if (mCurrentGameState != STATE_DEATH) {
		this->CheckMinigameTimeRemaining();
	}
}


void DungeonMaster::Render(Uint32 ticks) {
	// tell the current minigame to render stuff on the main screen
	mCurrentMinigame->Render(ticks);

	// this is a bad way to do this, but i'm tired
	bool renderMinigameName;
	bool renderTutorial, renderScore;
	bool renderGoodTimeBar, renderBadTimeBar;

	// rendering decisions based on the current game state
	switch (mCurrentGameState) {
	case STATE_TUTORIAL:
		renderTutorial = true;
		renderScore = false;
		renderGoodTimeBar = renderBadTimeBar = false;
		break;
	case STATE_BIRTH:
	case STATE_LIFE:
	case STATE_DEATH:
		renderTutorial = false;
		renderScore = true;
		renderGoodTimeBar = renderBadTimeBar = (mCurrentGameState != STATE_DEATH);
		break;
	default:	// CLOCK, TITLE, GAMEOVER
		renderTutorial = renderScore = renderGoodTimeBar = renderBadTimeBar = false;
		break;
	}

	// rendering decisions based on the minigame type
	switch (mCurrentMinigame->GetMinigameType()) {
	case MINIGAME_GOOD:
		renderMinigameName = true;
		renderBadTimeBar = false;
		break;

	case MINIGAME_BAD:
		renderMinigameName = true;
		renderGoodTimeBar = false;
		break;

	case MINIGAME_NEUTRAL:
		renderMinigameName = false;
		renderTutorial = renderScore = false;
		renderGoodTimeBar = renderBadTimeBar = false;
		break;
	}

	// render the name of the minigame, Chastain
	if (renderMinigameName) {
		SDL_BlitSurface(mMinigameNameSfc, NULL, mMainScreen, &mMinigameNameRect);
	}

	// render TUTORIAL sign
	if (renderTutorial) {
		SDL_BlitSurface(mTutSignSfc, NULL, mMainScreen, &mTutSignRect);
	}

	// render player score
	if (renderScore) {
		SDL_BlitSurface(mRenderedScoreSfc, NULL, mMainScreen, &mPlayerScoreRect);
	}

	// render the global time on a good screen
	if (renderGoodTimeBar) {
		SDL_FillRect(mMainScreen, &mLifeTimeLeftRect, SDL_MapRGB(mMainScreen->format, SDL_CLR_BLACK));
	}

	// render the global time on a bad screen
	if (renderBadTimeBar) {
		SDL_FillRect(mMainScreen, &mLifeTimeLeftRect, SDL_MapRGB(mMainScreen->format, SDL_CLR_WHITE));
	}

	// if the minigame is timed, draw the minigame time bar
	if (mCurrentMinigame->IsTimed()) {
		// calculate the height of the timer bar based on the time remaining
		int numpix = (int)((1.0f - mCurrentMinigame->FractionTimeRemaining()) * 138.0f);
		SDL_Rect rect;
		rect.x = 166 * PIXELW;
		rect.y = (149 - numpix) * PIXELH;
		rect.w = 4 * PIXELW;
		rect.h = numpix * PIXELH;
		switch (mCurrentMinigame->GetMinigameType()) {
		case MINIGAME_GOOD:
			SDL_FillRect(mMainScreen, &rect, SDL_MapRGB(mMainScreen->format, SDL_CLR_BLACK));
			break;
		case MINIGAME_BAD:
			SDL_FillRect(mMainScreen, &rect, SDL_MapRGB(mMainScreen->format, SDL_CLR_WHITE));
			break;
		default:
			break;
		}
	}

	// update the screen
	SDL_Flip(mMainScreen);
}


int DungeonMaster::Cleanup() {
	// close down each minigame
	// each should return RETURN_SUCCESS = 0, so this sum should = 0
	if (mText->Cleanup() +
		mTime->Cleanup() +
		Born::Instance()->Cleanup() +
		Dead::Instance()->Cleanup() +
		Fuck::Instance()->Cleanup() +
		Fuzz::Instance()->Cleanup() +
		Kids::Instance()->Cleanup() +
		Play::Instance()->Cleanup() +
		Pray::Instance()->Cleanup() +
		Shop::Instance()->Cleanup() +
		Work::Instance()->Cleanup() != 0) {
		return RETURN_ERROR;
	}

	// close all the fonts
	CloseAllFonts();

	return RETURN_SUCCESS;
}
