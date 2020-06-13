#ifndef TEXT_H_
#define TEXT_H_

#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "minigames/Minigame.h"

using namespace std;


class Text : public Minigame {
private:
	// a saved surface with all currently-rendered text
	SDL_Surface* mTextSurface;

	// instead of constantly recreating mTextSurface, we just have a list of parts to render
	vector<SDL_Rect> mNormalRects, mSmallRects;

	// the text images we will blit to screen
	vector<SDL_Surface*> mNormalSurfaces, mSmallSurfaces;

	// the parts of each string that have *not* been rendered to the screen
	vector<string> mNormalStrings, mSmallStrings;

	// combined height of all strings, in pixels
	int mCombinedTextHeight;

	// store the time we last displayed a new character
	Uint32 mLastCharTime;

	// the text has finished its "crawl"
	bool mTextFinished;

	// flash the cursor on and off when we're finished writing text
	bool mShowCursor;
	Uint32 mCursorClock;

	Text();	// singleton

	/** Show the next available character */
	void DisplayNewCharacter();

	/** See if we've finished showing text */
	void CheckTextFinished();

	/** Calculate which parts of the strings are visible */
	void CalculateVisibilityRects();

	/** Calculate what is visible of a particular font */
	void CalculateFontRects(TTF_Font*, vector<SDL_Surface*>&, vector<string>&, vector<SDL_Rect>&, int&);

	/** Render all text to the screen */
	void RenderAllText();

	/** Render a given line to the screen */
	void RenderLine(SDL_Surface*, SDL_Rect);

	/** Implement standard virtual functions */
	int _InitialPreload();
	void _NewMinigame();
	void _EndMinigame();
	void _Process(Uint32);
	void _Update(Uint32);
	void _Render(Uint32);
	int _Cleanup();

protected:
public:
	static Minigame* Instance();
	virtual ~Text();

	/** Add a normal-sized string to the text list */
	void GiveNormalString(const char*);

	/** Add a small-sized string to the text list */
	void GiveSmallString(const char*);

	/** Discard all strings */
	void ClearTextSurfaces();
};

#endif /*TEXT_H_*/
