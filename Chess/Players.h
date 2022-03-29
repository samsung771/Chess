#pragma once

#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>
#include "ChessGame.h"
#include <stdlib.h>
#include <time.h>  
#include <string>
#include "BoardMagnet.h"
#include "BoardMovement.h"

class ChessGame;


class Player {
public:
	bool check = false;
	bool colour;

	ChessGame* chessGamePtr = nullptr;

	Player();

	virtual bool update();
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
};

class RandAI : public Player {
public:
	bool update();
};

class MagnetPlayer : public Player {
private:
	BoardMagnetController magnetControl;
public:
	//original pos of piece
	int pieceHeldX, pieceHeldY = 0;
	//piece representation
	uint8_t pieceHeld = 0;

	bool update();
};

class MagnetPlayerAPI : public Player {
private:
	BoardMagnetController magnetControl;
public:
	//original pos of piece
	int pieceHeldX, pieceHeldY = 0;
	//piece representation
	uint8_t pieceHeld = 0;

	MagnetPlayerAPI();

	bool update();
};

class BoardPlayerRand : public Player {
private:
	BoardMoveController movementController;
public:
	bool update();
};

class BoardPlayerAPI : public Player {
private:
	BoardMoveController movementController;
public:
	bool update();
};

class APIHuman : public Player {
public:
	//mouse variables
	int* mousePosX;
	int* mousePosY;
	bool* mouseClick;

	//original pos of piece
	int pieceHeldX, pieceHeldY = 0;
	//piece representation
	uint8_t pieceHeld = 0;

	APIHuman(int* mouseX, int* mouseY, bool* mouseCl);

	bool update();
};

class APIPlayer : public Player {
	bool update();
};