#ifndef MINIGAME_H_
#define MINIGAME_H_

#include <vector>

#include <SDL.h>

#include "Log.h"
#include "TheButton.h"
#include "Timer.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

enum MinigameType { MINIGAME_NEUTRAL, MINIGAME_GOOD, MINIGAME_BAD };

class DungeonMaster; // avoid circular includes

const Uint32 MINIGAME_DEFAULT_LENGTH = 30 * 1000;	// in milliseconds


///////////////////////////////////////////////////////////////////////////////
// MAIN MINIGAME CLASS
///////////////////////////////////////////////////////////////////////////////

class Minigame {
private:
protected:
	// pointers back to major game components (game log, dungeon master, and Button)
	Log* mLog;
	DungeonMaster* mDM;
	TheButton* mTheButton;

	const char* mMinigameName;			// what is this minigame called?
	vector<const char*> mInstructions;	// a set of instructions for the minigame

	MinigameType mGameType;		// is this a good, bad, or neutral minigame?

	SDL_Surface* mScreen;	// a pointer to the main game surface
	SDL_Surface* mBackground;	// save a background surface for rendering in every frame

	// is this the real life? is this just fantasy/tutorial?
	bool mIsRealLife;

	// figure out if and when the minigame should time-out
	bool mIsTimed;
	bool mTimedOut;
	Uint32 mMinigameRunTime;	// how long does the minigame run?
	Timer mMinigameTimer;		// the actual stopwatch

	bool mReceivedFirstEvent;	// make sure we don't receive a spurious button press

	Minigame();	// singleton

	/** Called by a completed Minigame to notify the DM */
	void NotifyDMMinigameComplete();

	/* Protected functions to do things in inherited Minigames */
	virtual int _InitialPreload() = 0;
	virtual void _NewMinigame() = 0;
	virtual void _EndMinigame() = 0;
	virtual void _Process(Uint32) = 0;
	virtual void _Update(Uint32) = 0;
	virtual void _Render(Uint32) = 0;
	virtual int _Cleanup() = 0;

public:
	virtual ~Minigame() = 0;

	/** Get the title of this minigame */
	const char* GetTitle();

	/** Get the minigame's instructions */
	vector<const char*> GetInstructions();

	/** Get the type of minigame */
	MinigameType GetMinigameType();

	/** Initial load of all our resources, to make sure we have everything. */
	int InitialPreload();

	/**
	 * Start a (possibly repeating) Minigame from scratch each time.
	 * TRUE if we are playing through LIFE mode; FALSE if just tutorials.
	 */
	void NewMinigame(bool);

	/** Called by the DM when a minigame is finished to free up temporary resources */
	void EndMinigame();

	/** Check if the minigame can time-out */
	bool IsTimed();

	/** Fraction [0..1] of time remaining in this minigame; -1 if in tutorial */
	float FractionTimeRemaining();

	/** Check if the minigame has timed out */
	bool HasTimedOut();

	/** The standard game loop calls */
	void Process(Uint32);
	void Update(Uint32);
	void Render(Uint32);

	/** Clean up anything that won't go in the destructor (these being singletons) */
	int Cleanup();
};

#endif /*MINIGAME_H_*/
