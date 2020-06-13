#ifndef PLAY_H_
#define PLAY_H_

#include <vector>

#include <SDL_mixer.h>

#include "common.h"
#include "minigames/Minigame.h"

using namespace std;


class Play : public Minigame {
private:
	// score built up during play, can lose this if we crash
	float mCumulativeScore;

	// the player
	SDL_Surface* mPlayerSfc[4];	// the player's sprites (up, down, left, right)
	Direction mPlayerDirection;	// which way are we facing?
	SDL_Rect mPlayerFramePos;	// position in mini-frame ([0..125])
	Uint32 mLastPlayerMove, mTimeBetweenPlayerMove;	// time the player's movements
	bool mPlayerCrashed;

	// walls
	vector<SDL_Rect> mWalls;

	// pickups
	SDL_Surface* mGoodPickupSfc, * mBadPickupSfc;
	vector<SDL_Rect> mGoodPickups, mBadPickups;

	// sound effects
	Mix_Chunk* mGoodPickupSnd, * mBadPickupSnd;
	Mix_Chunk* mCrashSnd;

	Play();	// singleton

	/** Callback when ending sound finishes */
	static void CatchFinishedPlaySounds(int);

	/** Create all the player sprites */
	int GeneratePlayerSprites();

	/** Create the sprites for the objects we can pick up */
	int GeneratePickupSprites();

	/** Load all sound effects for the minigame */
	int LoadSoundEffects();

	/** Create a pseudo-random set of walls for the level */
	void PlaceLevelWalls();

	/** Randomly place all of the X's and O's */
	void PlaceAllPickups();

	/** Move the player in a given direction */
	void MovePlayer();

	/** Check if the player intersects a wall */
	bool PlayerIntersectsWall();

	/** Check if the player intersects a pickup */
	void CheckPlayerPickups();

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
	virtual ~Play();
};

#endif /*PLAY_H_*/
