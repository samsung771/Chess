#include "SDL.h"
#include <windows.h>
#include <string>

#define WIDTH 8
#define HEIGHT 8

#define windowWIDTH 600
#define windowHEIGHT 400

bool isRunning = true;
bool resize = true; 
int squareSize = windowHEIGHT/8;
int startingPosx, startingPosy = 0;
int w, h;
const SDL_Colour background{ 89, 73, 51,255 };
const SDL_Colour darkSquare{ 66, 26, 7,255 };
const SDL_Colour lightSquare{ 245, 214, 171,255 };
SDL_Texture* texture[12] = { NULL };
SDL_Rect rect;

SDL_Renderer* renderer;
SDL_Window* window;

void drawSquare(SDL_Colour colour, int x, int y, int w, int h){
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);

	SDL_Rect square{ x,y,w,h };
	SDL_RenderFillRect(renderer, &square);
}

void drawBackground() {
	if (resize) {
		SDL_GetWindowSize(window, &w, &h);

		if (w > h) squareSize = h / 8;
		else squareSize = w / 8;

		startingPosx = w / 2 - squareSize * 4;
		startingPosy = h / 2 - squareSize * 4;

		resize = false;
	}

	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if ((x + y) % 2 == 0) drawSquare(darkSquare, 
				x * squareSize + startingPosx, 
				y * squareSize + startingPosy, 
				squareSize, squareSize);
			else drawSquare(lightSquare, 
				x * squareSize + startingPosx, 
				y * squareSize + startingPosy, 
				squareSize, squareSize);
		}
	}
}

void init() {
	SDL_Surface* temp = NULL;

	 char files[12][12] = {
		"pawn.bmp",
		"pawnb.bmp",
		"king.bmp",
		"kingb.bmp",
		"queen.bmp",
		"queenb.bmp",
		"knight.bmp",
		"kinghtb.bmp",
		"rook.bmp",
		"rookb.bmp",
		"bishop.bmp",
		"bishopb.bmp",
	};

	for (int i = 0; i < 12; i++) {
		temp = SDL_LoadBMP(files[i]);

		texture[i] = SDL_CreateTextureFromSurface(renderer, temp);
	}
	
	SDL_FreeSurface(temp);

	rect.x = 0;
	rect.y = 200;
	rect.w = squareSize * 0.8;
	rect.h = squareSize * 0.8;
}

void update() {
	rect.w = squareSize * 0.8;
	rect.h = squareSize * 0.8;
}

void render() {
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);

	SDL_RenderClear(renderer);

	drawBackground();

	SDL_RenderCopy(renderer, texture[0], NULL, &rect);

	SDL_RenderPresent(renderer);
}

void handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT:
			isRunning = false;
			break;
		
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					resize = true;
					break;
			}
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					isRunning = false;
					break;
			}
			break;
	};
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Chess", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		windowWIDTH, 
		windowHEIGHT, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);

	init();

	while (isRunning) {
		handleEvents();
		update();
		render();
	}

	return 0;
}