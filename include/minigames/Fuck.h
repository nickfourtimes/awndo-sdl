#ifndef FUCK_H_
#define FUCK_H_

#include <queue>

#include <SDL_mixer.h>

#include "minigames/Minigame.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

enum SexPelletType { NORMAL_PELLET, AROUSE_PELLET };
typedef struct {
	bool active;	// are we actively tracking this pellet?
	bool in;		// is this an "in" pellet?
	SDL_Rect rect;
	SexPelletType type;
} SexPellet;


///////////////////////////////////////////////////////////////////////////////
// MAIN MINIGAME CLASS
///////////////////////////////////////////////////////////////////////////////

class Fuck : public Minigame {
private:
	// player and partner's paddles
	SDL_Rect mPlayer, mPartner;

	// is anyone having an orgasm?
	bool mPlayerOrgasm, mPartnerOrgasm;
	int mPlayerNumArouse, mPartnerNumArouse;
	SDL_Surface* mOrgasmSfc;	// surface to display if there's an orgasm

	// sex is a ticking clock
	Uint32 mLastTick;
	Uint32 mLastLower, mTicksPerLower;		// how often we lower pellets
	Uint32 mLastRelease, mTicksPerRelease;	// how often we release pellets

	// sex pellets
	SDL_Surface* mNormalPelletSfc, * mArousePelletSfc;	// drawing each pellet
	SexPellet* mPlayerPellets, * mPartnerPellets;		// pellets falling on player and partner sides
	queue<int> mFreePlayerPellets, mFreePartnerPellets;	// which pellets in our lists are free?
	Uint32 mPlayerTurnsNoArouse, mPartnerTurnsNoArouse;	// how long since arousal?
	Uint32 mReleaseTicks, mLoweringTicks;				// for timing purposes
	bool mPelletPlayerSide;		// on which side should the next pellet fall?

	// push it
	bool mThrusting;

	// sound effects
	Mix_Chunk* mPelletSound, * mArouseSound;
	Mix_Chunk* mOrgasmSound, * mDoubleOrgasmSound;

	/** Time events to occur on a tick of the clock */
	void SexTick();

	/** Create the draw-able pellet surfaces */
	int GeneratePellets();

	/** Create the surface to display for an orgasm */
	int GenerateOrgasmSurface();

	/** Load all sound effects */
	int LoadSoundEffects();

	/** Release a new sex pellet */
	void ReleasePellet();

	/** Move all pellets down the screen */
	void LowerPellets();

	/** Check a pellet to see if it needs erasing or points */
	void CheckPellet(int, bool);

	/** Player is more aroused */
	void ArousePlayer();

	/** Partner is more aroused */
	void ArousePartner();

	/** Shorten the remaining time if someone is orgasming */
	void OrgasmShortenTime();

	/** Draw the two participants */
	void RenderMates();

	/** Draw all sex pellets */
	void RenderPellets();

	/** Implement standard virtual functions */
	int _InitialPreload();
	void _NewMinigame();
	void _EndMinigame();
	void _Process(Uint32);
	void _Update(Uint32);
	void _Render(Uint32);
	int _Cleanup();

protected:
	Fuck();	// be smart. use protection.

public:
	static Minigame* Instance();
	virtual ~Fuck();
};

#endif /*FUCK_H_*/
