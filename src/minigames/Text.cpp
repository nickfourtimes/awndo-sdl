#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "Log.h"
#include "TheButton.h"
#include "minigames/Text.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

// time between successive characters being displayed
const Uint32 TEXT_TYPEWRITER_TICKS = 1000 / DESIRED_FPS * 1;

// cursor constants
const Uint32 TEXT_CURSOR_DELAY = 500;	// time that the cursor is on/off
const Uint32 TEXT_CURSOR_SIZE = 32;		// in pixels
const Uint32 TEXT_CURSOR_OFFSET = 50;	// from bottom-right corner

// is this text skippable?
const bool TEXT_SKIPPABLE = false;


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

void Text::DisplayNewCharacter() {
	// remove the next char from our vector, thus uncovering one char in the final render
	bool removedChar = false;
	vector<string>::iterator iter;
	for (iter = mNormalStrings.begin(); iter != mNormalStrings.end() && !removedChar; ++iter) {
		if ((*iter).length() > 0) {
			(*iter).erase(0, 1);
			removedChar = true;
		}
	}

	// do the same with the small strings
	for (iter = mSmallStrings.begin(); iter != mSmallStrings.end() && !removedChar; ++iter) {
		if ((*iter).length() > 0) {
			(*iter).erase(0, 1);
			removedChar = true;
		}
	}

	// displayed a new character, check if there are any remaining
	this->CheckTextFinished();

	// see which parts of the text are visible
	this->CalculateVisibilityRects();

	// we've now updated a character; wait a bit.
	mLastCharTime = SDL_GetTicks();
}


void Text::CheckTextFinished() {
	vector<string>::iterator iter;
	for (iter = mNormalStrings.begin(); iter != mNormalStrings.end(); ++iter) {
		if ((*iter).length() > 0) {
			return;	// text is not finished displaying
		}
	}

	for (iter = mSmallStrings.begin(); iter != mSmallStrings.end(); ++iter) {
		if ((*iter).length() > 0) {
			return;	// text is not finished displaying
		}
	}

	// we get here if no string has any characters remaining
	mTextFinished = true;
	mCursorClock = SDL_GetTicks();
}


void Text::CalculateVisibilityRects() {
	// for each string to display (normal first, then small)...
	int offsetFromTop = 0;
	this->CalculateFontRects(NORMAL_FONT, mNormalSurfaces, mNormalStrings, mNormalRects, offsetFromTop);
	this->CalculateFontRects(SMALL_FONT, mSmallSurfaces, mSmallStrings, mSmallRects, offsetFromTop);
}


void Text::CalculateFontRects(TTF_Font* font, vector<SDL_Surface*>& surfaces, vector<string>& strings, vector<SDL_Rect>& rects, int& voff) {
	for (unsigned int i = 0; i < surfaces.size(); ++i) {
		if (surfaces[i]->w != rects[i].w) {
			// figure out the length of the non-visible part of the string
			int nonvisibleWidth;
			TTF_SizeText(font, strings[i].c_str(), &nonvisibleWidth, NULL);

			// subtract this length from the length of the whole string
			int visibleTextWidth = surfaces[i]->w - nonvisibleWidth;

			// display the visible portion of the string
			rects[i].h = surfaces[i]->h;
			rects[i].w = visibleTextWidth;
			rects[i].x = SCREEN_W / 2 - visibleTextWidth / 2;	// centred horizontally
			rects[i].y = SCREEN_H / 2 - mCombinedTextHeight / 2 + voff;	// centred vertically
		}

		voff += surfaces[i]->h;
	}
}


void Text::RenderAllText() {
	// blit all normal text surfaces
	for (unsigned int i = 0; i < mNormalSurfaces.size(); ++i) {
		this->RenderLine(mNormalSurfaces[i], mNormalRects[i]);
	}

	// blit all small text surfaces
	for (unsigned int i = 0; i < mSmallSurfaces.size(); ++i) {
		this->RenderLine(mSmallSurfaces[i], mSmallRects[i]);
	}
}


void Text::RenderLine(SDL_Surface* lineSurface, SDL_Rect rect) {
	SDL_Rect srcrect, destrect;
	srcrect.x = srcrect.y = 0;
	srcrect.w = rect.w;
	srcrect.h = rect.h;
	destrect.x = rect.x;
	destrect.y = rect.y;
	SDL_BlitSurface(lineSurface, &srcrect, mScreen, &destrect);
}


int Text::_InitialPreload() {
	// defaults
	mGameType = MINIGAME_NEUTRAL;
	mIsTimed = false;

	return RETURN_SUCCESS;
}


void Text::_NewMinigame() {
	mTextSurface = NULL;
	mCombinedTextHeight = 0;
	mLastCharTime = SDL_GetTicks();
	mTextFinished = false;
	mShowCursor = false;
}


void Text::_EndMinigame() {
	// clear all surfaces
	for (unsigned int i = 0; i < mNormalSurfaces.size(); ++i) {
		GarbageGobbler::Instance()->Discard(mNormalSurfaces[i]);
	}
	mNormalRects.clear();
	mNormalStrings.clear();
	mNormalSurfaces.clear();

	for (unsigned int i = 0; i < mSmallSurfaces.size(); ++i) {
		GarbageGobbler::Instance()->Discard(mSmallSurfaces[i]);
	}
	mSmallRects.clear();
	mSmallStrings.clear();
	mSmallSurfaces.clear();
}


void Text::_Process(Uint32 ticks) {
	if (mTextFinished || TEXT_SKIPPABLE) {
		if (TheButton::Instance()->PressedThisFrame()) {
			this->NotifyDMMinigameComplete();
		}
	}
}


void Text::_Update(Uint32 ticks) {
	if (!mTextFinished) {
		// check if it's time to display a new character
		if (SDL_GetTicks() - mLastCharTime > TEXT_TYPEWRITER_TICKS) {
			this->DisplayNewCharacter();
		}
	} else {  // finished showing text, so wait for button press
		if (SDL_GetTicks() - mCursorClock > TEXT_CURSOR_DELAY) {
			mShowCursor = !mShowCursor;
			mCursorClock = SDL_GetTicks();
		}
	}
}


void Text::_Render(Uint32 ticks) {
	// clear the screen
	SDL_FillRect(mScreen, NULL, SDL_MapRGB(mScreen->format, SDL_CLR_BLACK));

	// blit all our text to the screen
	this->RenderAllText();

	// show cursor if necessary
	if (mShowCursor) {
		SDL_Rect rect;
		rect.x = SCREEN_W - TEXT_CURSOR_OFFSET - TEXT_CURSOR_SIZE;
		rect.y = SCREEN_H - TEXT_CURSOR_OFFSET - TEXT_CURSOR_SIZE;
		rect.w = rect.h = TEXT_CURSOR_SIZE;
		SDL_FillRect(mScreen, &rect, SDL_MapRGB(mScreen->format, SDL_CLR_WHITE));
	}
}


int Text::_Cleanup() {
	return RETURN_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Text::Text() : Minigame()
{

}


Text::~Text() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

Minigame* Text::Instance() {
	static Text mInstance;
	return &mInstance;
}


void Text::GiveNormalString(const char* msg) {
	// store the actual text surface
	mNormalSurfaces.push_back(TTF_RenderText_Solid(NORMAL_FONT, msg, SDL_CLR_WHITE));

	// store the rect that will define how much of the surface to show
	SDL_Rect rect;
	rect.x = rect.y = rect.w = 0;
	rect.h = mNormalSurfaces.back()->h;
	mNormalRects.push_back(rect);

	// store the string, so we can pop one letter at a time
	string str(msg);
	mNormalStrings.push_back(str);

	int h;
	TTF_SizeText(NORMAL_FONT, msg, NULL, &h);
	mCombinedTextHeight += h;
}


void Text::GiveSmallString(const char* msg) {
	// store the actual text surface
	mSmallSurfaces.push_back(TTF_RenderText_Solid(SMALL_FONT, msg, SDL_CLR_WHITE));

	// store the rect that will define how much of the surface to show
	SDL_Rect rect;
	rect.x = rect.y = rect.w = 0;
	rect.h = mSmallSurfaces.back()->h;
	mSmallRects.push_back(rect);

	// store the string, so we can pop one letter at a time
	string str(msg);
	mSmallStrings.push_back(str);

	int h;
	TTF_SizeText(SMALL_FONT, msg, NULL, &h);
	mCombinedTextHeight += h;
}


void Text::ClearTextSurfaces() {
	SDL_Surface* sfc;

	// Clear all the normal-sized surfaces
	while ((sfc = mNormalSurfaces.back()) != NULL) {
		mNormalSurfaces.pop_back();	// remove the back element from the set
		GarbageGobbler::Instance()->Discard(sfc);
	}

	// Clear all the small-sized surfaces
	while ((sfc = mSmallSurfaces.back()) != NULL) {
		mSmallSurfaces.pop_back();	 // remove back element from set
		GarbageGobbler::Instance()->Discard(sfc);
	}
}
