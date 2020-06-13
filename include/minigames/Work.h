#ifndef WORK_H_
#define WORK_H_

#include <SDL_mixer.h>

#include "common.h"
#include "minigames/Minigame.h"


class Work : public Minigame {
private:
	bool** mWorkPegs;
	int mNumPegsRemaining;
	int mPlayerRow, mPlayerCol;

	// sfx
	Mix_Chunk* mPegSound;

	Work();	// singleton

	/** Render all the pegs still active */
	void RenderAllPegs();

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
	virtual ~Work();
};

#endif /*WORK_H_*/
