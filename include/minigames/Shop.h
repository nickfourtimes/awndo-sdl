#ifndef SHOP_H_
#define SHOP_H_

#include <SDL_mixer.h>

#include "minigames/Minigame.h"


///////////////////////////////////////////////////////////////////////////////
// EXTRAS
///////////////////////////////////////////////////////////////////////////////

enum Mode { MODE_A, MODE_B, MODE_C };
typedef struct {
	bool active;
	SDL_Rect pos;
	Mode mode;
} ShopItem;


///////////////////////////////////////////////////////////////////////////////
// MAIN MINIGAME CLASS
///////////////////////////////////////////////////////////////////////////////

class Shop : public Minigame {
private:
	SDL_Surface* mItemSfc[3];

	Uint32 mLastClockTick;	// last time the clock ticked

	SDL_Rect mPlayer;	// the physical player
	int mShrinkTicks;	// last time player shrunk down
	bool mIsShopping;	// is the player shopping?

	Mode mCurrentMode;	// the current hotness
	int mModeChangeTicks;	// when did we last change mode?

	// items
	ShopItem* mItemList;
	int mItemMoveTicks;

	// sound effects
	Mix_Chunk* mModeMoveSnd, * mModeSnd, * mUnmodeSnd;

	Shop();	// singleton

	/** Ticking clock */
	void ClockTick();

	/** Create the surfaces for the items */
	int GenerateItemSprites();

	/** Load all sound effects */
	int LoadSoundEffects();

	/** Randomly choose a new mode */
	void RandomlyChooseMode();

	/** Generate a new item at the end of the list */
	void IntroduceNewItem();

	/** Move all the items along */
	void MoveAllItems();

	/** Check if the player gets any items */
	void CheckPlayerItems();

	/** Make the player accumulate if necessary */
	void PlayerAccumulates();

	/** Make the player shrink if necessary */
	void PlayerShrinks();

	/** Draw whatever's a-la-mode */
	void RenderCurrentMode();

	/** Draw all the items */
	void RenderAllItems();

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
	virtual ~Shop();
};

#endif /*SHOP_H_*/
