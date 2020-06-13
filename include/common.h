#ifndef COMMON_H_
#define COMMON_H_

#include <SDL.h>
#include <SDL_ttf.h>


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

extern const int RETURN_ERROR;
extern const int RETURN_SUCCESS;

extern const float PI;

// are all tutorials initially playable or not?
extern bool LIVELY;

// logging
extern const char* LOG_FILENAME;

// graphics constants
extern const int ATARI_PIXEL_W;
extern const int ATARI_PIXEL_H;
extern const int PIXELW;
extern const int PIXELH;
extern const int MINIFRAME_SZ;

// screen constants
extern const Uint32 FULLSCREEN;
extern const int SCREEN_W;
extern const int SCREEN_H;
extern const int SCREEN_BPP;
extern const int DESIRED_FPS;

// SDL constants
extern const Uint32 RMASK;
extern const Uint32 GMASK;
extern const Uint32 BMASK;
extern const Uint32 AMASK;
extern const SDL_Color SDL_CLR_BLACK;
extern const SDL_Color SDL_CLR_WHITE;
extern const SDL_Color SDL_CLR_RED;
extern const SDL_Color SDL_CLR_GREEN;
extern const SDL_Color SDL_CLR_BLUE;

// fonts
extern const char* TTF_FILE;
extern const int FONT_SMALL_SIZE;
extern const int FONT_NORMAL_SIZE;
extern TTF_Font* SMALL_FONT;
extern TTF_Font* NORMAL_FONT;

// joystick mappings
// These have to be defines, as const int can't be used in switches
#define JOYSTICK_A 0
#define JOYSTICK_START 7
#define JOYSTICK_BACK 6
/*#define JOYSTICK_B 1
#define JOYSTICK_X 2
#define JOYSTICK_Y 3*/

// shorthand
enum Direction { UP, DN, LT, RT };


///////////////////////////////////////////////////////////////////////////////
// PROTOTYPES
///////////////////////////////////////////////////////////////////////////////

/** Load all fonts at initialisation */
int LoadAllFonts();


/** Clean up all the loaded fonts */
void CloseAllFonts();


/** Get the path of a file WITHIN "assets/" folder */
void GetAssetName(char* dst, const char* path);


/** Check if two rectangles overlap */
bool RectanglesOverlap(SDL_Rect& r1, SDL_Rect& r2);


/**
 * SDL provides SDL_MapRGB which maps separate Uint8 r, g, and b values to a
 * Uint32 value. We want to do the same with a regular SDL_Color struct.
 */
Uint32 SDL_MapRGB(SDL_PixelFormat* fmt, SDL_Color clr);


/**
 * Each minigame will occur within a fixed-size frame. This function creates
 * and colours in that frame.
 * @param bg The background colour
 * @param fg The foreground colour
 * @return the newly-created SDL_Surface if successful, NULL if it fails
 */
SDL_Surface* DrawMinigameFrame(SDL_Color bkgdClr, SDL_Color frameClr);


/**
 * Each minigame takes place within a frame. Minigames draw within this
 * "126x126" frame, so this function returns an SDL_Rect representing a pixel
 * *within* the frame. That is, querying for (0,0) will give the top-left
 * pixel *inside the frame*.
 */
int GetMiniFramePixel(Uint32 x, Uint32 y, SDL_Rect* pos);

#endif /*COMMON_H_*/
