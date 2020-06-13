#ifndef DEAD_H_
#define DEAD_H_

#include <vector>

#include <SDL.h>

#include "minigames/Minigame.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	SDL_Rect rect;
	bool active, falling;
	float dx, dy;
	int xdir;
	bool hover;
	int numframes;
} Snowflake;


///////////////////////////////////////////////////////////////////////////////
// MAIN MINIGAME CLASS
///////////////////////////////////////////////////////////////////////////////

class Dead : public Minigame {
private:
	float mSnowflakePtVal;	// snowflake value is based on player's score

	Snowflake* mMainSnowflakes;	// the obvious array of snowflakes
	Snowflake* mFrameSnowflakes;	// TWIST ENDING SNOWFLAKES
	vector<int> mRandomFrameIndices;	// randomly drop frame flakes

	Uint32 mLastDropSnowflake;	// when did we last drop a flake?
	Uint32 mLastFlakeDisappeared = -1;	// when did the scene "finish"?

	// counters to keep track of what's what
	Uint32 mNumFlakesActive;	// # snowflakes still active on screen
	Uint32 mNumFlakesNotDropped;	 // # snowflakes that are not falling
	Uint32 mNumFlakesHeld;	// how many flakes are we holding on to?

	Dead();	// singleton

	/** Find the first non-moving snowflake to drop */
	void FindAvailableSnowflake();

	/** Make a snowflake drop */
	void DropSnowflake(Snowflake*);

	/** Move and (optionally) accelerate all snowflakes */
	void UpdateAllSnowflakes();

	/** Update an individual snowflake */
	void UpdateSnowflake(Snowflake*);

	/** Check if a snowflake has gone out of bounds */
	void CheckFlakeBounds(Snowflake*);

	/** Draw all the active snowflakes */
	void RenderAllSnowflakes();

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
	virtual ~Dead();
};

#endif /*DEAD_H_*/
