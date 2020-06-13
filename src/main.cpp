#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#undef main
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "common.h"
#include "DungeonMaster.h"
#include "GarbageGobbler.h"
#include "Log.h"
#include "TheButton.h"
#include "Timer.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// GLOBAL INSTANCE HANDLES
///////////////////////////////////////////////////////////////////////////////

DungeonMaster *dungeonMaster;
GarbageGobbler *garbageGobbler;
Log *myLog;
TheButton *theButton;


///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

/** Get any and all command-line arguments */
void GetProgramArguments(int argc, char** argv) {
	//TODO see about getting command-line arguments as to whether this is lively or not

	//TODO THIS IS JUST A DEBUG HACK
	LIVELY = true;
}


/** Initialise our SDL application, and its various components. */
int InitSDL() {
	// main SDL initialisation
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		myLog->LogMsg("ERROR: Could not initialise SDL!\n");
		return RETURN_ERROR;
	}

	// set up our rendering surface
	if (dungeonMaster->InitScreen(SCREEN_W, SCREEN_H, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | FULLSCREEN) != RETURN_SUCCESS) {
		myLog->LogMsg("ERROR: Could not set up screen!\n");
		return RETURN_ERROR;
	}

	// name our window
	SDL_WM_SetCaption("THE AMERICAN DREAM", NULL);

	// get our joystick up and running
	SDL_JoystickEventState(SDL_ENABLE);
	if (SDL_JoystickOpen(0) == NULL) {
		myLog->LogMsg("WARNING: No joystick detected!\n");
	}

	// Initialise the SDL_image library
	// Not necessarily needed; see
	// http://forums.libsdl.org/viewtopic.php?t=5275&sid=f7f83edf2d126454cad9ce4909ec9e17
	/*if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != 0) {
		char msg[256];
		sprintf(msg, "ERROR: Could not initialise SDL_image library (%s)!\n", IMG_GetError());
		log->LogMsg(msg);
		return RETURN_ERROR;
	}*/

	// initialise the Mixer library
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) != 0) {
		char msg[256];
		sprintf(msg, "ERROR: Could not initialise the SDL_mixer library (%s)!\n", Mix_GetError());
		myLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// initialise the TTF font library
	if (TTF_Init() != 0) {
		char msg[256];
		sprintf(msg, "ERROR: Could not initialise SDL_ttf library (%s)!\n", TTF_GetError());
		myLog->LogMsg(msg);
		return RETURN_ERROR;
	}

	// hide the mouse
	SDL_ShowCursor(SDL_DISABLE);

	return RETURN_SUCCESS;
}


void CloseSDL() {
	//IMG_Quit();
	Mix_CloseAudio();
	TTF_Quit();

	myLog->LogMsg("*** SDL libraries closed ***\n");

	// show the cursor once again
	SDL_ShowCursor(SDL_ENABLE);
}


///////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	srand((unsigned int)time(NULL));

	// get our handles
	dungeonMaster = DungeonMaster::Instance();
	garbageGobbler = GarbageGobbler::Instance();
	myLog = Log::Instance();
	theButton = TheButton::Instance();

	// try to activate our log file
	if (myLog->InitLog(LOG_FILENAME) != RETURN_SUCCESS) {
		//TODO: Messagebox?
		cout << "ERROR: Could not initialise log file!" << endl;
		return RETURN_ERROR;
	}

	// get all command-line arguments
	GetProgramArguments(argc, argv);

	// initialise our SDL context and surfaces
	if (InitSDL() != RETURN_SUCCESS) {
		myLog->LogMsg("ERROR: Problems with SDL initialisation! Quitting...\n");
		return RETURN_ERROR;
	}

	// make sure all our minigames have their required resources
	if (dungeonMaster->InitGame() != RETURN_SUCCESS) {
		myLog->LogMsg("ERROR: DungeonMaster failed to initialise properly!\n");
		return RETURN_ERROR;
	}

	// to be used in our game loop
	SDL_Event event;
	bool quit = false;
	Timer* timer = new Timer();
	timer->Start();
	Uint32 ticks, now;
	Uint32 ticksSinceLastProcess, ticksSinceLastUpdate, ticksSinceLastRender;
	ticksSinceLastProcess = ticksSinceLastUpdate = ticksSinceLastRender = SDL_GetTicks();

	// main game loop
	myLog->LogMsg("*** GAME BEGINS! ***\n");
	while (!quit && !dungeonMaster->Quit()) {
		timer->Reset();

		// poll all the events this frame
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					//TODO: RESET probably goes here
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_JOYBUTTONDOWN:
				switch (event.jbutton.button) {
				case JOYSTICK_BACK:
					quit = true;
					break;
				default:
					break;
				}
				break;
			case SDL_JOYBUTTONUP:
				break;
			}
			theButton->Process(0, event); //TODO: ticks?
		} // end while(poll events)

		// don't process/update/render if we've already chosen to quit
		if (quit) {
			continue;
		}

		// let the current minigame process downbuttons
		dungeonMaster->Process(0);	//TODO: ticks?	

		now = SDL_GetTicks();
		dungeonMaster->Update(now - ticksSinceLastUpdate);	//TODO: ticks?
		ticksSinceLastUpdate = now;

		// check if it's time to quit before doing anything
		if (dungeonMaster->Quit()) {
			continue;
		}

		dungeonMaster->Render(0);	//TODO: ticks?

		theButton->EndOfFrame();

		// cap our framerate
		ticks = timer->GetTicks();
		if ((int)ticks < 1000 / DESIRED_FPS) {
			if (1000 / DESIRED_FPS - ticks > 4) {	// if we have cycles to kill, gobble some memories
				Uint32 gobblestart = SDL_GetTicks();
				garbageGobbler->Gobble();
				Uint32 gobbletime = SDL_GetTicks() - gobblestart;
				SDL_Delay(1000 / DESIRED_FPS - ticks - gobbletime);
			} else {
				SDL_Delay(1000 / DESIRED_FPS - ticks);
			}
		}
	}	// end of game loop
	myLog->LogMsg("*** GAME ENDS! ***\n");

	// clean up and close down
	delete timer;
	if (dungeonMaster->Cleanup() != RETURN_SUCCESS) {
		myLog->LogMsg("WARNING: Problems cleaning up minigames; shutting down anyway!\n");
	}
	while (!garbageGobbler->IsEmpty()) {
		// properly discard anything in the Gobbler
		garbageGobbler->Gobble();
	}
	CloseSDL();
	myLog->CloseLog();

	return RETURN_SUCCESS;
}
