#ifndef PRAY_H_
#define PRAY_H_
#include <SDL_mixer.h>

#include "minigames/Minigame.h"


class Pray : public Minigame {
private:
	// instructions or "prayer finished" surfaces
	SDL_Surface* mBgSfc, *mInstructionSfc, * mFinishedSfc;
	SDL_Surface* mPrayHandsApart, * mPrayHandsTogether;
	bool mIsPraying, mFinishedPraying;

	// points
	Uint32 mDonatedPoints;	// how much we've donated
	Uint32 mLastAddPoints;	// last time we donated

	// sound effects
	Mix_Chunk* mDonateBeep;
	Mix_Chunk* mVictoryBeep;

	Pray();	// singleton

	/** Callback when victory sound finishes */
	static void CatchFinishedPraySounds(int);

	/** Load all sound effects for the minigame */
	int LoadSoundEffects();

	/** Import all hands for the minigame */
	int LoadHandVisuals();

	/** Cover up any previous donation tallies */
	void ClearDonatedPoints();

	/** Render the number of donated points to the screen */
	void RenderDonatedPoints();

	/** Render hands */
	void RenderHands();

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
	virtual ~Pray();
};

#endif /*PRAY_H_*/
