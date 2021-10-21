#pragma once

#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>
#include "ChessGame.h"
#include <stdlib.h>
#include <time.h>  


class ChessGame;


class Player {
public:
	bool check = false;
	bool colour;

	ChessGame* chessGamePtr = nullptr;

	Player();

	virtual bool update();

	virtual bool promote();
};


class Human : public Player {
public:
	//mouse variables
	int* mousePosX;
	int* mousePosY;
	bool* mouseClick;

	//original pos of piece
	int pieceHeldX, pieceHeldY = 0;
	//piece representation
	uint8_t pieceHeld = 0;

	Human(int* mouseX, int* mouseY, bool* mouseCl);

	bool update();

	bool promote();
};

class RandAI : public Player {
	bool update();

	bool promote();
};