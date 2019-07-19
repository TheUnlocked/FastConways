#pragma once

#include <SDL.h>

static int WIDTH = 640;		// The width of the screen (default: 640)
static int HEIGHT = 480;	// The height of the screen (default: 480)

// Do not touch
static const int PIXEL_SIZE = 4 * sizeof(unsigned char);
static unsigned char* pixels;
static int tps;
static std::mutex* gridMut = new std::mutex;
static int* frames = new int(0);
static bool* quit = new bool(false);

// Functions
int main(int argc, char** argv);
void renderTask(int* grid, int* frames);
void renderConways(int* grid, SDL_Renderer* renderer, SDL_Texture* texture);
void runLogic(int*& dataGrid, int*& tempGrid);
inline int testRule(int currentVal, int neighbors);
inline bool inRange(int low, int high, int num);
int positive_modulo(int num, int denom);
void renderFPS(int framerate);

// Defines the font
// This is almost certainly faster than rendering using more traditional font formats.
// Lower memory footprint as well.
// ints are used for performance.
static const int FONT_SCALE = 2;	// Sets the scale of the font render (default: 2)
static const int FONT_HEIGHT = 9;
static const int FONT_PTRS[]{0, 54, 90, 144, 198, 252, 306, 360, 414, 468, 522, 576, 630, 693};
static const int FONT[]{
	// ZERO
	0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 1, 0, 0, 1, 1,
	0, 1, 0, 1, 0, 1,
	0, 1, 1, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0,

	// ONE
	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 1, 1, 1,
	0, 0, 0, 0,

	// TWO
	0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0,

	// THREE
	0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 1, 0,
	0, 0, 0, 1, 1, 0,
	0, 0, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0,

	// FOUR
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0,
	0, 0, 0, 1, 1, 0,
	0, 0, 1, 0, 1, 0,
	0, 1, 0, 0, 1, 0,
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0,

	// FIVE
	0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1,
	0, 1, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0,

	// SIX
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1,
	0, 0, 1, 0, 0, 0,
	0, 1, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0,

	// SEVEN
	0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 1, 0,
	0, 0, 0, 1, 0, 0,
	0, 0, 1, 0, 0, 0,
	0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,

	// EIGHT
	0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0,

	// NINE
	0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 1, 0,
	0, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0,

	// F
	0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 1, 0, 0,
	0, 0, 0, 1, 0, 0,
	0, 0, 0, 1, 0, 0,
	0, 0, 0, 1, 0, 0,
	0, 0, 0, 1, 0, 0,
	0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 0,

	// P
	0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 1,
	0, 1, 0, 0, 0, 1,
	0, 1, 1, 1, 1, 0,
	0, 1, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,

	// S
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0, 0,
	0, 1, 0, 0, 0, 1, 0,
	0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0,
	0, 1, 0, 0, 0, 1, 0,
	0, 0, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0,
};