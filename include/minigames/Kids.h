#ifndef KIDS_H_
#define KIDS_H_

#include <SDL_mixer.h>

#include "minigames/Minigame.h"


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	bool active;
	SDL_Rect pos;
} Missile;


///////////////////////////////////////////////////////////////////////////////
// MAIN MINIGAME CLASS
///////////////////////////////////////////////////////////////////////////////

class Kids : public Minigame {
private:
	// parent and child sprites
	SDL_Surface* mParentSfc, * mChildSfc;
	SDL_Rect mParentPos, mChildPos;

	// missiles
	SDL_Surface* mMissileSfc;
	Missile* mMissileList;
	int mNumReleasedMissiles;

	// ticking clock
	Uint32 mLastClockTick;
	int mChildFallTicks, mParentFallTicks;	// person ticks
	int mMissileTopBottomReleaseTicks, mMissileMiddleReleaseTicks;
	int mMissileMoveTicks;	// how often we move the missiles

	// movement
	Uint32 mChildLastFall, mParentLastFall;
	int mChildDirection;

	// sound effects
	Mix_Chunk* mParentSwoopSnd, * mChildHitSnd, * mParentHitSnd, * mMissileMoveSnd;

	Kids();	// singleton

	/** Keep a ticking clock */
	void TickClock();

	/** Create the parent and child */
	int GeneratePersonSprites();

	/** Create our missile sprite */
	int GenerateMissileSprite();

	/** Load all sound effects */
	int LoadSoundEffects();

	/** Update the child's position */
	void MoveChild(int);

	/** Limit the child's movement */
	void LimitChild();

	/** Move the parent, drag the child if necessary */
	void MoveParent(int);

	/** Move all missiles */
	void MoveMissiles();

	/** Find the index of an inactive missile */
	int FindInactiveMissile();

	/** Release missiles at the top and bottom of the frame */
	void ReleaseTopBottomMissiles();

	/** Release missiles randomly within the frame */
	void ReleaseMiddleMissiles();

	/** Draw the parent and child to the screen */
	void RenderParentAndChild();

	/** Draw all missiles */
	void RenderMissiles();

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
	virtual ~Kids();
};

#endif /*KIDS_H_*/
