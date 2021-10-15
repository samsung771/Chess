#pragma once

#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>

#define windowWIDTH  800
#define windowHEIGHT 600

#define WIDTH  8
#define HEIGHT 8

#define COLOURMASK 0b10000000
#define MOVEMASK   0b01000000
#define PIECEMASK  0b00111111

//macro to get the piece from its binary representation
#define getPiece(x,y)\
int piece = log2(board[y][x] & PIECEMASK);\
bool colour = (board[y][x] & COLOURMASK) >> 7;\
bool hasMoved = !(board[y][x] & MOVEMASK) >> 6;

class MoveManager;

class Renderer {
	//rendering variables
	SDL_Texture* texture[12] = { NULL };
	const SDL_Colour background{ 83, 94, 93, 255 };
	const SDL_Colour darkSquare{ 49, 51, 51, 255 };
	const SDL_Colour lightSquare{ 225, 235, 234, 255 };
	const SDL_Colour colCheck{ 225, 0, 0, 255 };

	//rendering size variables
	int w, h;
	const float pieceScale[6] = { 0.7,0.8,0.8,0.8,0.8,0.8 };
	const float pieceScalerW[6] = { 0.95,1,1,1,1,1 };
	//SDL variables
	SDL_Renderer* renderer;
	SDL_Window* window;


	//draws squares
	void drawSquare(SDL_Colour colour, int x, int y, int w, int h);

	//draws background to fit screen
	void drawBackground();

public:
	int squareSize = windowHEIGHT / HEIGHT;
	int startingPosx, startingPosy = 0;
	bool resize = true;
	uint8_t pieceHeld;
	int pieceHeldX, pieceHeldY = 0;
	int* mousePosX;
	int* mousePosY;

	std::vector<std::vector<int>> available;

	//load textures and initialise some variables
	void init();

	//clears frame and renders a new one
	void renderScreen(uint8_t board[8][8]);

	int getSquareSize();

	int getStartPosX();

	int getStartPosY();
};