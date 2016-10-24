#include "Chip8.h"
#include <iostream>
#include <SDL.h>

int main(int argc, char *argv[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	//Initialize SDL
	if (argc != 2) {
		printf("chip8 <path to rom>\n");
		return 1;
	}
		
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 2;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * 10, 32 * 10, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return 3;
		}
		else {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == NULL) {
				printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
				return 4;
			}
		}
	}

	Chip8 inst;
	inst.loadRom(argv[1]);

	SDL_Event event;
	SDL_Rect r;
	r.h = 10;
	r.w = 10;
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				for (int i = 0; i < 16; i++) {
					switch (event.key.keysym.sym) {
						case SDLK_x: {inst.keys[0] = 1; break; }
						case SDLK_1: {inst.keys[1] = 1; break; }
						case SDLK_2: {inst.keys[2] = 1; break; }
						case SDLK_3: {inst.keys[3] = 1; break; }
						case SDLK_q: {inst.keys[4] = 1; break; }
						case SDLK_w: {inst.keys[5] = 1; break; }
						case SDLK_e: {inst.keys[6] = 1; break; }
						case SDLK_a: {inst.keys[7] = 1; break; }
						case SDLK_s: {inst.keys[8] = 1; break; }
						case SDLK_d: {inst.keys[9] = 1; break; }
						case SDLK_z: {inst.keys[10] = 1; break; }
						case SDLK_c: {inst.keys[11] = 1; break; }
						case SDLK_4: {inst.keys[12] = 1; break; }
						case SDLK_r: {inst.keys[13] = 1; break; }
						case SDLK_f: {inst.keys[14] = 1; break; }
						case SDLK_v: {inst.keys[15] = 1; break; }
					}
				}
			}
			else if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
					case SDLK_x: {inst.keys[0] = 0; break; }
					case SDLK_1: {inst.keys[1] = 0; break; }
					case SDLK_2: {inst.keys[2] = 0; break; }
					case SDLK_3: {inst.keys[3] = 0; break; }
					case SDLK_q: {inst.keys[4] = 0; break; }
					case SDLK_w: {inst.keys[5] = 0; break; }
					case SDLK_e: {inst.keys[6] = 0; break; }
					case SDLK_a: {inst.keys[7] = 0; break; }
					case SDLK_s: {inst.keys[8] = 0; break; }
					case SDLK_d: {inst.keys[9] = 0; break; }
					case SDLK_z: {inst.keys[10] = 0; break; }
					case SDLK_c: {inst.keys[11] = 0; break; }
					case SDLK_4: {inst.keys[12] = 0; break; }
					case SDLK_r: {inst.keys[13] = 0; break; }
					case SDLK_f: {inst.keys[14] = 0; break; }
					case SDLK_v: {inst.keys[15] = 0; break; }
				}
			}
		}
		inst.emulateCycle();
		if (inst.reDraw) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			for (int i = 0; i < 64 * 32; i++)
				if (inst.display[i] == 1) {
					r.x = i % 64 * 10;
					r.y = i / 64 * 10;
					SDL_RenderFillRect(renderer, &r);
				}
			SDL_RenderPresent(renderer);
			inst.reDraw = false;
		}
	}
	return 0;
}