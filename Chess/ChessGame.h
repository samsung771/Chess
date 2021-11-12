#pragma once
#include "Renderer.h"
#include "MoveManager.h"
#include "Players.h"

#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>


#define WIDTH  8
#define HEIGHT 8

#define COLOURMASK 0b10000000
#define MOVEMASK   0b01000000
#define PIECEMASK  0b00111111

#define PAWN	   0b00000001
#define KNIGHT	   0b00000010
#define BISHOP	   0b00000100
#define ROOK	   0b00001000
#define QUEEN	   0b00010000
#define KING	   0b00100000

#define FRAMECAP   30

//macro to get the piece from its binary representation
#define getPiece(x,y)\
int piece = log2(board[y][x] & PIECEMASK);\
bool colour = (board[y][x] & COLOURMASK) >> 7;\
bool hasMoved = !(board[y][x] & MOVEMASK) >> 6;

class Player;
class Human;
class RandAI;

class ChessGame {
private:
	bool isRunning = true;

	//update board
	void update();


	//handle SDL window events e.g. key presses
	void handleEvents();

	std::chrono::steady_clock::time_point lastCheckFrame = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastCheckSecond = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

public:
	//public game variables
	uint8_t board[8][8] = { {0} };
	bool isWhiteTurn = 1;
	int move = 0;

	Player* player1;
	Player* player2;

	//mouse variables
	int* mousePosX;
	int* mousePosY;
	bool* mouseClick;

	Renderer renderer;
	MoveManager moveManager;

	//load a board state from fen notation
	void loadBoardFromFen(std::string fen);

	//constructor
	ChessGame(Player* playerW, Player* playerB, int* mouseX, int* mouseY, bool* mouseCl);

	//plays main game loop
	int Play();
};