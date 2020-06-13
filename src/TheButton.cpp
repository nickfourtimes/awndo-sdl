#include <stdlib.h>

#include "common.h"
#include "TheButton.h"


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// {CON|DE}STRUCTORS
///////////////////////////////////////////////////////////////////////////////

TheButton::TheButton() {
	mButtonUp = true;
	mPressedThisFrame = mReleasedThisFrame = false;
}


TheButton::~TheButton() {

}


///////////////////////////////////////////////////////////////////////////////
// METHODS
///////////////////////////////////////////////////////////////////////////////

TheButton* TheButton::Instance() {
	static TheButton mInstance;
	return &mInstance;
}


void TheButton::Event(EventType type) {
	switch (type) {
	case PRESS:
		if (mButtonUp) {
			mPressedThisFrame = true;
			mButtonUp = false;
		}
		break;
	case RELEASE:
		if (!mButtonUp) {
			mReleasedThisFrame = true;
			mButtonUp = true;
		}
		break;
	}
}


bool TheButton::PressedThisFrame() {
	return mPressedThisFrame;
}


void TheButton::ConsumeButtonPressed() {
	mPressedThisFrame = false;
}


bool TheButton::ReleasedThisFrame() {
	return mReleasedThisFrame;
}


void TheButton::ConsumeButtonReleased() {
	mReleasedThisFrame = false;
}


bool TheButton::IsUp() {
	return mButtonUp;
}


bool TheButton::IsDown() {
	return !mButtonUp;
}


void TheButton::Process(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_SPACE:
		case SDLK_x:
		case SDLK_z:
			this->Event(PRESS);
			break;
		default:
			break;
		}
		break;

	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_SPACE:
		case SDLK_x:
		case SDLK_z:
			this->Event(RELEASE);
			break;
		default:
			break;
		}
		break;

	case SDL_JOYBUTTONDOWN:
		switch (event.jbutton.button) {
		case JOYSTICK_A:
			this->Event(PRESS);
			break;
		}
		break;

	case SDL_JOYBUTTONUP:
		switch (event.jbutton.button) {
		case JOYSTICK_A:
			this->Event(RELEASE);
			break;
		}
		break;
	}
}


void TheButton::EndOfFrame() {
	mPressedThisFrame = mReleasedThisFrame = false;
}
