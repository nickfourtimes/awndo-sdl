#ifndef BORN_H_
#define BORN_H_

#include <SDL_mixer.h>

#include "minigames/Minigame.h"


class Born : public Minigame {
private:
	Uint32 mDilation;	// how dilated are we (IN PIXELS)
	Uint32 mLastContraction;	// last time we contracted
	Uint32 mNextContraction;	// time we will next contract
	bool mBorn;		// when we're done

	// sound effects
	Mix_Chunk* mDilationBeeps[3];
	Mix_Chunk* mVictoryBeep;

	Born();	// singleton

	/** Callback for when any playing sound finishes */
	static void CatchFinishedBornSounds(int);

	/** Load all minigame sound effects */
	int LoadSoundEffects();

	/** When the thingie has to contract */
	void Contract();

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
	virtual ~Born();
};

#endif /*BORN_H_*/
