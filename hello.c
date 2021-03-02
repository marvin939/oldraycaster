#include <stdio.h>
#include <SDL.h>

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);

	if (win == NULL) {
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		SDL_DestroyWindow(win);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// SDL2.0 now uses textures to draw things but SDL_LoadBMP returns a surface
	// this lets us choose when to upload or remove textures from the GPU.
	SDL_Surface *bmp = SDL_LoadBMP("hello.bmp");
	if (bmp == NULL) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// To use a hardware accelerated texture for rendering we can create one
	// from the surface we loaded.
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
	// We no longer need the surface
	SDL_FreeSurface(bmp);
	if (tex == NULL) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// A sleep rendering loop, wait for 3 seconds and render and present the
	// screen each time
	for (int i = 0; i < 3; ++i) {
		// First clear the renderer
		SDL_RenderClear(ren);
		// Draw the texture
		SDL_RenderCopy(ren, tex, NULL, NULL);
		// Update the screen
		SDL_RenderPresent(ren);
		// Take a quick break after all that hard work
		SDL_Delay(1000);
	}

	// Clean up our objects and quit
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

