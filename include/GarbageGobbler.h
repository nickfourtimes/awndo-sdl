#ifndef GARBAGEGOBBLER_H_
#define GARBAGEGOBBLER_H_

#include <vector>

#include <SDL.h>

using namespace std;


/**
 * We may have to dispose of a lot of SDL_Surfaces, because I am a big jerk.
 * The Garbage Gobbler is a righteous deallocator of memory. Praise be.
 *
 * "Who has garbage for the Garbage Gobbler? Who has garbage for the Garbage Gobbler?
 * Who has garbage for the Garbage Gobbler? Who has garbage for the Garbage Gobbler?"
 * -- Garbage Gobbler song, chorus.
 */
class GarbageGobbler {
private:
	vector<SDL_Surface*> mSurfacesToDelete;

	GarbageGobbler();	// singleton

protected:
public:
	static GarbageGobbler* Instance();
	~GarbageGobbler();

	/** Give a free-able surface to the gobbler to gobble when there's time */
	void Discard(SDL_Surface*);

	/** Check if the Gobbler is empty */
	bool IsEmpty();

	/** Dispose of any available things */
	void Gobble();
};

#endif /*GARBAGEGOBBLER_H_*/
