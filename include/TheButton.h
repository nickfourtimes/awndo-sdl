#ifndef THEBUTTON_H_
#define THEBUTTON_H_

#include <SDL.h>


enum EventType { PRESS, RELEASE };


class TheButton {
private:
	bool mButtonUp;
	bool mPressedThisFrame, mReleasedThisFrame;

	TheButton();	// singleton

	/** Process a button event */
	void Event(EventType);

protected:
public:
	static TheButton* Instance();
	~TheButton();

	/** Was the button pressed in this frame? */
	bool PressedThisFrame();

	/** May sometimes need to consume "button pressed" messages */
	void ConsumeButtonPressed();

	/** Was the button released in this frame? */
	bool ReleasedThisFrame();

	/** May sometimes need to consume "button released" messages */
	void ConsumeButtonReleased();

	/** Query whether the button is in the "up" state */
	bool IsUp();

	/** Query whether the button is in the "down" state */
	bool IsDown();

	/** Standard game loop functions */
	void Process(Uint32, SDL_Event&);
	void Update(Uint32);

	/** Notify the button that the frame is ending */
	void EndOfFrame();
};

#endif /* THEBUTTON_H_ */
