#ifndef TIMER_H_
#define TIMER_H_

#include <SDL.h>


/**
 * A simple stopwatch class to help with timing minigames.
 */
class Timer {
private:
	bool mRunning;
	Uint32 mStartTicks, mAccumulatedTicks;

protected:
public:
	Timer();
	~Timer();

	void Start();
	void Reset();
	Uint32 GetTicks();
	void SetTicks(Uint32);
	void Stop();
};

#endif /*TIMER_H_*/
