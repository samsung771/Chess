#pragma once

#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>
#include "ChessGame.h"
#include <stdlib.h>
#include <time.h>  
#include <iostream>
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

class BoardPlayerRand : public Player {
private:
	BoardMoveController movementController;
public:
	bool update();
};

/*
class MiniMaxAI : public Player {
private:
	struct position {

	};

	int minimax(uint8_t board[8][8], int depth, bool maxPlayer, bool player, int alpha, int beta);

	int staticEval(uint8_t board[8][8]);

public:
	bool update();
};
*/