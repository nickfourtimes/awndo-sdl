#include "Timer.h"


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

Timer::Timer() {
	mRunning = false;
	mStartTicks = 0;
}


Timer::~Timer() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

void Timer::Start() {
	if (!mRunning) {
		mRunning = true;
		mStartTicks = SDL_GetTicks();
	}
}


void Timer::Reset() {
	if (mRunning) {
		mStartTicks = SDL_GetTicks();
	}

	mAccumulatedTicks = 0;
}


Uint32 Timer::GetTicks() {
	if (mAccumulatedTicks == 0) {
		if (mRunning) {
			return SDL_GetTicks() - mStartTicks;
		} else {
			return 0;
		}
	} else {
		return mAccumulatedTicks;
	}
}


void Timer::SetTicks(Uint32 t) {
	if (mRunning) {
		mStartTicks = SDL_GetTicks() - t;
	}
}


void Timer::Stop() {
	if (mRunning) {
		mRunning = false;
		mAccumulatedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}
