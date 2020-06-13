#ifndef FUZZ_H_
#define FUZZ_H_

#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>

#include "minigames/Minigame.h"

using namespace std;


class Fuzz : public Minigame {
private:
	vector<SDL_Surface*> mFuzzySurfaces;	// random static surfaces
	SDL_Surface* mCurrentSfc;	// the one we're currently using

	vector<int> mSurfaceOrder;	// make a random ordering for the list

	Uint32 mTimeStarted;	// when did we start the static?
	Uint32 mLastScreenSwitch;	// when we last changed random screens

	Mix_Chunk* mNoise;	// a static-y sound that we play continuously
	int mNoiseChannel;	// the channel the sound plays on

	Fuzz();	// singleton

	/** Point our current screen at the next one in the list */
	void PointAtNextFuzzy();

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
	virtual ~Fuzz();
};


#endif /*FUZZ_H_*/
