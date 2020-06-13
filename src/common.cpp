#include "common.h"
#include "Log.h"


///////////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////////

const int RETURN_ERROR = -1;
const int RETURN_SUCCESS = 0;

const float PI = 3.14159265f;

bool LIVELY;

// path to assets folder
const char* ASSETS_PATH = "assets/";

// logging
const char* LOG_FILENAME = "awndo.log";

// graphics constants
const int ATARI_PIXEL_W = 192;
const int ATARI_PIXEL_H = 160;
const int PIXELW = 4;
const int PIXELH = 3;
const int MINIFRAME_SZ = 126;

// screen constants
const Uint32 FULLSCREEN = 0x00000000;	// WINDOWED
//const Uint32 FULLSCREEN = SDL_FULLSCREEN;	// FULLSCREEN
const int SCREEN_W = ATARI_PIXEL_W * PIXELW;
const int SCREEN_H = ATARI_PIXEL_H * PIXELH;
const int SCREEN_BPP = 32;
const int DESIRED_FPS = 60;

// SDL constants
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const Uint32 RMASK = 0xff000000;
const Uint32 GMASK = 0x00ff0000;
const Uint32 BMASK = 0x0000ff00;
const Uint32 AMASK = 0x000000ff;
#else
const Uint32 RMASK = 0x000000ff;
const Uint32 GMASK = 0x0000ff00;
const Uint32 BMASK = 0x00ff0000;
const Uint32 AMASK = 0xff000000;
#endif
const SDL_Color SDL_CLR_BLACK = { 0, 0, 0, 255 };
const SDL_Color SDL_CLR_WHITE = { 255, 255, 255, 255 };
const SDL_Color SDL_CLR_RED = { 255, 0, 0, 255 };
const SDL_Color SDL_CLR_GREEN = { 0, 255, 0, 255 };
const SDL_Color SDL_CLR_BLUE = { 0, 0, 255, 255 };

// font constants
const char* TTF_FILE = "fonts/Commodore 64/Commodore-64-v621c.TTF";
const int FONT_SMALL_SIZE = 24;
const int FONT_NORMAL_SIZE = 48;
TTF_Font* SMALL_FONT;
TTF_Font* NORMAL_FONT;


///////////////////////////////////////////////////////////////////////////////
// FUNCTION IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////

int LoadAllFonts() {
	char str[256];
	GetAssetName(str, TTF_FILE);

	SMALL_FONT = TTF_OpenFont(str, FONT_SMALL_SIZE);
	NORMAL_FONT = TTF_OpenFont(str, FONT_NORMAL_SIZE);

	if (!SMALL_FONT || !NORMAL_FONT) {
		char msg[256];
		sprintf(msg, "ERROR: Could not load game fonts (%s)!\n", TTF_GetError());
		Log::Instance()->LogMsg(msg);
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


void CloseAllFonts() {
	TTF_CloseFont(SMALL_FONT);
	TTF_CloseFont(NORMAL_FONT);
}


void GetAssetName(char* dst, const char* nameInAssets) {
	sprintf(dst, "%s%s", ASSETS_PATH, nameInAssets);
}


bool RectanglesOverlap(SDL_Rect& r1, SDL_Rect& r2) {
	if (r1.x >= r2.x + r2.w ||
		r2.x >= r1.x + r1.w ||
		r1.y >= r2.y + r2.h ||
		r2.y >= r1.y + r1.h) {
		return false;
	} else {
		return true;
	}
}


Uint32 SDL_MapRGB(SDL_PixelFormat* fmt, SDL_Color clr) {
	return SDL_MapRGB(fmt, clr.r, clr.g, clr.b);
}


SDL_Surface* DrawMinigameFrame(SDL_Color bkgdClr, SDL_Color frameClr) {
	SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, SCREEN_BPP, RMASK, GMASK, BMASK, 0);
	if (!surface) {
		return NULL;
	}

	// draw our background
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, bkgdClr));

	// draw our foreground
	SDL_Rect rect;
	rect.x = 108;	// top bar
	rect.y = 33;
	rect.w = 552;
	rect.h = 18;
	SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, frameClr));
	rect.w = 24;	// left bar
	rect.h = 414;
	SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, frameClr));
	rect.x = 636;	// right bar
	rect.y = 33;
	rect.w = 24;
	rect.h = 414;
	SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, frameClr));
	rect.x = 108;	// bottom bar
	rect.y = 429;
	rect.w = 552;
	rect.h = 18;
	SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, frameClr));

	return surface;
}


int GetMiniFramePixel(Uint32 x, Uint32 y, SDL_Rect* pos) {
	if (x < 0 || x >= 126 || y < 0 || y >= 126) {
		pos->w = pos->h = pos->x = pos->y = 0;
		return RETURN_ERROR;
	}

	pos->w = PIXELW;
	pos->h = PIXELH;
	pos->x = PIXELW * (27 + 6 + x);
	pos->y = PIXELH * (11 + 6 + y);

	return RETURN_SUCCESS;
}
