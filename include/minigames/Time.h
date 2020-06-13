#ifndef TIME_H_
#define TIME_H_

#include <SDL.h>
#include <SDL_mixer.h>

#include "minigames/Minigame.h"


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

const Uint32 TIME_NUM_CLOCKTICKS = 10;


///////////////////////////////////////////////////////////////////////////////
// MAIN CLOCK CLASS
///////////////////////////////////////////////////////////////////////////////

class Time : public Minigame {
private:
	bool mActiveMinigames[TIME_NUM_CLOCKTICKS]; // keep track of which minigames are available

	Minigame* mMinigame;	// the minigame we select on the clock

	SDL_Surface* mClockHands[TIME_NUM_CLOCKTICKS];	// images for each of the clock hands
	int mCurrentClockInd;		// index of the clock hand to render

	Uint32 mLastTick;	// when did clock last tick?

	bool mBeginLife;	// do we begin LIFE cycle?
	bool mQuit;		// quit selected?

	// sound effects
	Mix_Chunk* mTickSound;

	Time();	// singleton

	/** Load all clock hands from memory */
	int LoadAllClockHands();

	/** Tick the clock forward one */
	void TickClock();

	/** Render the tick marks around the clock's face */
	void RenderClockTicks();

	/** Render the labels for all the clock ticks */
	void RenderClockLabels();

	/** Render the clock hand */
	void RenderClockHand();

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
	virtual ~Time();

	/** Get the currently-selected minigame */
	Minigame* GetMinigame();

	/** Shall we begin the LIFE cycle? */
	bool BeginLife();

	/** Was "QUIT" selected? */
	bool Quit();
};

#endif /*TIME_H_*/
