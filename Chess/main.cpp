#include "SDL.h"
#include <windows.h>
#include <string>

#define WIDTH  8
#define HEIGHT 8

#define COLOURMASK 0b10000000
#define MOVEMASK   0b01000000
#define PIECEMASK  0b00111111

#define windowWIDTH  800
#define windowHEIGHT 600

bool isRunning = true;
bool resize = true; 
int squareSize = windowHEIGHT/8;
int startingPosx, startingPosy = 0;
int w, h;
int mousePosX, mousePosY;
bool mouseClick = 0;
bool isWhiteTurn = 1;
int pieceHeldX, pieceHeldY;
uint8_t pieceHeld = 0;
SDL_Texture* texture[12] = { NULL };
uint8_t board[8][8] = { {0} };

const float pieceScale[6] = { 0.7,0.8,0.8,0.8,0.8,0.8 };
const float pieceScalerW[6] = { 0.95,1,1,1,1,1 };
const SDL_Colour background{ 83, 94, 93,255 };
const SDL_Colour darkSquare{ 49, 51, 51,255 };
const SDL_Colour lightSquare{ 225, 235, 234,255 };

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

		if (w > h) squareSize = h / HEIGHT;
		else squareSize = w / WIDTH;

		startingPosx = w / 2 - squareSize * 4;
		startingPosy = h / 2 - squareSize * 4;

		resize = false;
	}

	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if ((x + y) % 2 == 1) drawSquare(darkSquare, 
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
	   "knight.bmp",
	   "bishop.bmp",
	   "rook.bmp",
	   "queen.bmp",
	   "king.bmp",
	   "pawnb.bmp",
	   "knightb.bmp",
	   "bishopb.bmp",
	   "rookb.bmp",
	   "queenb.bmp",
	   "kingb.bmp",
	};

	for (int i = 0; i < 12; i++) {
		temp = SDL_LoadBMP(files[i]);

		texture[i] = SDL_CreateTextureFromSurface(renderer, temp);
	}

	SDL_FreeSurface(temp);
}

bool isValid(int x, int y) {
	return true;
}

bool isInCheck(int x, int y) {
	return false;
}

void update() {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if ((board[y][x] & 0b00111111) != 0) {
				int piece = log2(board[y][x] & 0b00111111);
				int colour = (board[y][x] & 0b10000000) >> 7;

				SDL_Rect pos;
				pos.x = startingPosx + x * squareSize + squareSize * (1 - pieceScale[piece] * pieceScalerW[0]) / 2;
				pos.y = startingPosy + y * squareSize + squareSize * (1 - pieceScale[piece] * pieceScalerW[0]) / 2;
				pos.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
				pos.h = squareSize * pieceScale[piece];

				SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &pos);
			}
		}
	}

	if (mouseClick) {
		int squareX = floor((mousePosX - startingPosx) / squareSize);
		int squareY = floor((mousePosY - startingPosy) / squareSize);

		uint8_t player = isWhiteTurn ? 0b10000000 : 0;

		if (squareX >= 0 && squareX < 8 && squareY >= 0 && squareY < 8) {
			int colour = (board[squareY][squareX] & COLOURMASK) >> 7;

			//place piece
			if (board[squareY][squareX] == 0 && pieceHeld != 0) {
				board[squareY][squareX] = pieceHeld;
				pieceHeld = 0;

				isWhiteTurn = !isWhiteTurn;
			}

			else if (colour == !isWhiteTurn) {
				//pickup piece
				if (board[squareY][squareX] != 0 && pieceHeld == 0) {
					pieceHeld = board[squareY][squareX];
					board[squareY][squareX] = 0;
					pieceHeldX = squareX;
					pieceHeldY = squareY;
				}
				else if (board[squareY][squareX] != 0 && pieceHeld != 0) {
					board[pieceHeldY][pieceHeldX] = pieceHeld;
					pieceHeld = 0;
				}
			}
			//capture piece
			else if (board[squareY][squareX] != 0 && pieceHeld != 0) {
				board[squareY][squareX] = pieceHeld;
				pieceHeld = 0;

				isWhiteTurn = !isWhiteTurn;
			}
		}
		mouseClick = false;
	}


	if (pieceHeld != 0) {
		int piece = log2(pieceHeld & 0b00111111);
		int colour = (pieceHeld & COLOURMASK) >> 7;

		SDL_Rect rect;
		rect.x = mousePosX - squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2 - 9;
		rect.y = mousePosY - squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2 - 9;
		rect.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
		rect.h = squareSize * pieceScale[piece];

		SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &rect);
	}
}

void renderScreen() {
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);

	SDL_RenderClear(renderer);

	drawBackground();
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

		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&mousePosX, &mousePosY);
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouseClick = true;
			break;
	};
}

void loadBoardFromFen(std::string fen) {
	int pointerX = 0;
	int pointerY = 0;
	for (char i : fen) {
		if (i != toupper(i)) {
			board[pointerY][pointerX] |= 0b10000000;
		}

		switch (tolower(i)){
		case 'r':
			board[pointerY][pointerX] |= 0b00001000;
			pointerX++;
			break;
		case 'k':
			board[pointerY][pointerX] |= 0b00100000;
			pointerX++;
			break;
		case 'p':
			board[pointerY][pointerX] |= 0b00000001;
			pointerX++;
			break;
		case 'n':
			board[pointerY][pointerX] |= 0b00000010;
			pointerX++;
			break;
		case 'b':
			board[pointerY][pointerX] |= 0b00000100;
			pointerX++;
			break;
		case 'q':
			board[pointerY][pointerX] |= 0b00010000; 
			pointerX++;
			break;
		default:
			if (i == '/') {
				pointerY++;
				pointerX = 0;
			}
			else if (i - 48 > 0 && i - 48 < 9) {
				pointerX += i - 48;
			}
			break;
		}

		if (pointerX > 8 || pointerY > 8) break;		
	}
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

	loadBoardFromFen("rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR");

	while (isRunning) {
		handleEvents();
		renderScreen();
		update();

		SDL_RenderPresent(renderer);
	}

	return 0;
}