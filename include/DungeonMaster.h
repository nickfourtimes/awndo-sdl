#ifndef DUNGEONMASTER_H_
#define DUNGEONMASTER_H_

#include <SDL.h>

#include "Log.h"
#include "minigames/Minigame.h"
#include "minigames/Text.h"
#include "minigames/Time.h"


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

// the game runs though this simple but effective state-machine
enum GameState {
	STATE_TITLE, STATE_CLOCK, STATE_TUTORIAL,
	STATE_BIRTH, STATE_LIFE, STATE_DEATH, STATE_GAMEOVER
};


///////////////////////////////////////////////////////////////////////////////
// DUNGEON MASTER --- This class basically runs the whole game
///////////////////////////////////////////////////////////////////////////////

class DungeonMaster {
private:
	Log* mLog;

	SDL_Surface* mMainScreen;

	// player's total score
	float mPlayerScore;
	SDL_Rect mPlayerScoreRect;
	SDL_Surface* mPlayerScoreGoodSfc, * mPlayerScoreBadSfc, * mRenderedScoreSfc;

	// text to show what minigame we're playing
	SDL_Surface* mMinigameNameSfc;
	SDL_Rect mMinigameNameRect;

	// text to indicate if we're in a tutorial
	SDL_Surface* mTutSignGood, * mTutSignBad, * mTutSignSfc;
	SDL_Rect mTutSignRect;

	// time remaining in the main game
	Uint32 mLifeStartTime;
	SDL_Rect mLifeTimeLeftRect;

	// where we are in the game's state diagram
	GameState mCurrentGameState;

	// pointers to various minigames
	Text* mText;
	Time* mTime;
	Minigame* mCurrentMinigame;

	DungeonMaster();	// singleton

	/** Try to initialise all minigames */
	int InitMinigames();

	/** Render the "TUTORIAL" text surfaces */
	void GenerateTutorialSurfaces();

	/** Render surfaces showing the player's score */
	void GenerateScoreSurfaces();

	/** Choose which elements and versions (good or bad) of the HUD info will be shown */
	void SelectHUDInfo();

	/** Check if we're currently playing through LIFE mode */
	bool IsRealLife();

	/** End a current minigame and move to the next given one */
	void SwitchToMinigame(Minigame*);

	/** Check if the minigame has time remaining */
	void CheckMinigameTimeRemaining();

	/** See how much time is remaining. Return TRUE if there is still time left, FALSE otherwise */
	void CheckLifeTimeRemaining();

	/** Set up and initiate the LIFE minigames */
	void StartLife();

	/** Choose the next LIFE minigame to load */
	void NextLifeMinigame();

	/** Give new text to show on screen */
	void PrepareTitleText();
	void PrepareLifeText();
	void PrepareGameOverText();

protected:
public:
	static DungeonMaster* Instance();
	~DungeonMaster();

	/** Create the game screen */
	int InitScreen(Uint32, Uint32, Uint32, Uint32);
	SDL_Surface* GetScreen();

	/** Initialise game components */
	int InitGame();

	/** Check the player's score */
	float GetPlayerScore();

	/** Change the player's score */
	void ModifyPlayerScore(float pts);

	/** Called by a minigame when it is complete, to notify the DM */
	void EndMinigame(Minigame*);

	/** Does the game signal to quit? */
	bool Quit();

	/** Standard game loop functions */
	void Process(Uint32);
	void Update(Uint32);
	void Render(Uint32);

	/** Clean up everything in the game */
	int Cleanup();
};

#endif /*DUNGEONMASTER_H_*/
