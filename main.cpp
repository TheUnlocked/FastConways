#define RENDER 1	// 1 enables rendering
#define NOINLINE 0	// 1 prevents inlining on certain functions for profiling

#include <execution>
#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include <ppl.h>
#include "rle.h"
#include "main.h"

#pragma warning(disable: 26451)

int main(int argc, char** argv) {

	// Empty for random.
	// Reads RLE files (http://www.conwaylife.com/wiki/Run_Length_Encoded)

	std::string dataLoc;
	int dataLocType = -1;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--width") || !strcmp(argv[i], "-w")) {
			WIDTH = std::stoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "--height") || !strcmp(argv[i], "-h")) {
			HEIGHT = std::stoi(argv[++i]);
		}
		else if (!strcmp(argv[i], "--wrap")) {
			WRAP = true;
		}
		else if (!strcmp(argv[i], "--file") || !strcmp(argv[i], "-f")) {
			std::string filename = argv[++i];
			if (filename.length() == 0)
				continue;
			if (dataLocType != -1) {
				std::cout << "--file and --url cannot both be used at the same time.\n";
			}
			dataLoc = filename;
			dataLocType = 0;
		}
		else if (!strcmp(argv[i], "--url")) {
			std::string url = argv[++i];
			if (url.length() == 0)
				continue;
			if (dataLocType != -1) {
				std::cout << "--file and --url cannot both be used at the same time.\n";
			}
			dataLoc = url;
			dataLocType = 1;
		}
	}

	int* conwaysDisplay;
	int* conwaysTemp = new int[WIDTH * HEIGHT];

#if RENDER
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}
#endif

	switch(dataLocType){
	case 0:
		conwaysDisplay = loadGridFromString(dataLoc, WIDTH, HEIGHT);
		break;
	case 1:
		conwaysDisplay = loadGridFromURL(dataLoc, WIDTH, HEIGHT);
		break;
	default:
		conwaysDisplay = new int[WIDTH * HEIGHT];

		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0, 1);
		auto randBinary = std::bind(distribution, generator);

		for (int x = 0; x < WIDTH; x++)
			for (int y = 0; y < HEIGHT; y++)
				conwaysDisplay[x + y * WIDTH] = randBinary();
	}

#if RENDER
	std::thread renderTask(renderTask, conwaysDisplay, frames);
#else
	auto start = std::chrono::system_clock::now();
	auto end = start;
#endif

	SDL_Event event;
	while (!*quit) {
		runLogic(conwaysDisplay, conwaysTemp);
		*frames += 1;
#if RENDER == 0
		if (!(*frames % 1000)) {
			end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed = (end - start);
			auto elapsedSec = elapsed.count();
			std::swap(start, end);

			std::cout << "1000 frames elapsed in " << elapsedSec << "s\n"
				<< "Average " << 1000 / elapsedSec << "fps\n\n";
		}
#endif
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym != SDLK_ESCAPE)
					break;
			case SDL_QUIT:
				std::quick_exit(0);
				break;
			}
		}
	}
	return 0;
}

#if RENDER
void renderTask(int* grid, int* ticks) {
	SDL_Window* window = SDL_CreateWindow("Fast Conway's", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	pixels = new unsigned char[WIDTH * HEIGHT * PIXEL_SIZE];

	if (!window || !renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and/or renderer: %s", SDL_GetError());
		exit(2);
	}

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);

	std::cout << "Render Mode: " << info.name << "\n\n";

	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();
	int prevTicks = *ticks;

	double tpsCycles = 0;
	double tpsTotal = 0;
	static const double frameTime = 1000 / 60.;

	while (!*quit) {
		a = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> work_time = a - b;

		if (work_time.count() < frameTime)
		{
			std::chrono::duration<double, std::milli> delta_ms(frameTime - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		b = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> sleep_time = b - a;

		tpsTotal += ((double)*ticks - prevTicks) * 1000 / (work_time + sleep_time).count();
		if (++tpsCycles == 30) {
			tps = (int)(tpsTotal / tpsCycles);
			tpsTotal = 0;
			tpsCycles = 0;
		}

		//if (*frames - prevFrames > 0)
		//	std::cout << "Frames: " << *frames - prevFrames << "\nTime: " << (work_time + sleep_time).count() << "\n";

		renderConways(grid, renderer, texture);
		prevTicks = *ticks;

		SDL_PumpEvents();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

#if NOINLINE
__declspec(noinline)
#endif
void renderConways(int* grid, SDL_Renderer* renderer, SDL_Texture* texture)
{
	std::memset(pixels, 0, WIDTH * HEIGHT * PIXEL_SIZE);

	gridMut->lock();
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		if (grid[i]) {
			std::memset(pixels + (long long) i * PIXEL_SIZE, 255, PIXEL_SIZE);
		}
	gridMut->unlock();

	renderFPS(tps);

	SDL_UpdateTexture(texture, NULL, pixels, WIDTH * PIXEL_SIZE);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
#endif

#if NOINLINE
__declspec(noinline)
#endif
void runLogic(int*& dataGrid, int*& tempGrid) {
	std::fill(tempGrid, tempGrid + WIDTH * HEIGHT, 0);

	for (int i = 0; i < WIDTH * HEIGHT; i++)
		if (dataGrid[i]) {
			int x = i % WIDTH,
				y = i / WIDTH;
			for (int i = -1; i < 2; i++)
				for (int j = -1; j < 2; j++) {
					int xPos = x + i,
						yPos = y + j;
					if (WRAP) {
						if (!((i == 0) * (j == 0))) {
							tempGrid[positive_modulo(xPos, WIDTH) + positive_modulo(yPos, HEIGHT) * WIDTH]++;
						}
					}
					else
					{
						if (!((i == 0) * (j == 0)) && inRange(0, WIDTH - 1, xPos) * inRange(0, HEIGHT - 1, yPos)) {
							tempGrid[xPos + yPos * WIDTH]++;
						}
					}
				}
		}

	gridMut->lock();
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		dataGrid[i] = testRule(dataGrid[i], tempGrid[i]);
	gridMut->unlock();
}

inline int testRule(int currentVal, int neighbors) {
	return currentVal * (neighbors == 2) + (neighbors == 3);
}

inline bool inRange(int low, int high, int num) {
	return (num - high) * (num - low) <= 0;
}

inline int positive_modulo(int num, int denom) {
	return (num % denom + denom) % denom;
}

#if RENDER
#if NOINLINE
__declspec(noinline)
#endif
void renderFPS(int framerate) {
	auto str = std::to_string(framerate) + "TPS";

	int xOffset = 0;

	for (char c : str) {
		int charPtr = 0;
		switch (c) {
		case '0':
			charPtr = 0;
			break;
		case '1':
			charPtr = 1;
			break;
		case '2':
			charPtr = 2;
			break;
		case '3':
			charPtr = 3;
			break;
		case '4':
			charPtr = 4;
			break;
		case '5':
			charPtr = 5;
			break;
		case '6':
			charPtr = 6;
			break;
		case '7':
			charPtr = 7;
			break;
		case '8':
			charPtr = 8;
			break;
		case '9':
			charPtr = 9;
			break;
		case 'T':
			charPtr = 10;
			break;
		case 'P':
			charPtr = 11;
			break;
		case 'S':
			charPtr = 12;
			break;
		}
		int charWidth = (FONT_PTRS[charPtr + 1] - FONT_PTRS[charPtr]) / FONT_HEIGHT;
		for (int i = 0; i < FONT_PTRS[charPtr + 1] - FONT_PTRS[charPtr]; i++) {
			int x = (xOffset + i % charWidth) * FONT_SCALE,
				y = (i / charWidth) * FONT_SCALE;
			for (int j = 0; j < FONT_SCALE; j++)
				std::memset(
					pixels + (x + ((long long) y + j) * WIDTH) * PIXEL_SIZE,
					FONT[i + FONT_PTRS[charPtr]] * 255,
					PIXEL_SIZE * FONT_SCALE
				);
		}
		xOffset += charWidth;
	}
}
#endif