#pragma once
#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>

#define COLOURMASK 0b10000000
#define MOVEMASK   0b01000000
#define PIECEMASK  0b00111111

#define PAWN	   0b00000001
#define KNIGHT	   0b00000010
#define BISHOP	   0b00000100
#define ROOK	   0b00001000
#define QUEEN	   0b00010000
#define KING	   0b00100000

#define WIDTH  8
#define HEIGHT 8


//macro to get the piece from its binary representation
#define getPiece(x,y)\
int piece = log2(board[y][x] & PIECEMASK);\
bool colour = (board[y][x] & COLOURMASK) >> 7;\
bool hasMoved = !(board[y][x] & MOVEMASK) >> 6;


class MoveManager {
	//vector movements for each piece type
	int rookMoves[4][2] = {
		{1,0},
		{-1,0},
		{0,-1},
		{0,1}
	}; int bishopMoves[4][2] = {
		{1,1},
		{-1,1},
		{1,-1},
		{-1,-1}
	}; int knightMoves[8][2] = {
		{2,1},
		{2,-1},
		{1,2},
		{-1,2},
		{-2,1},
		{-2,-1},
		{1,-2},
		{-1,-2}
	}; int allMoves[8][2] = {
		{1,1},
		{-1,1},
		{1,-1},
		{-1,-1},
		{1,0},
		{-1,0},
		{0,-1},
		{0,1}
	};
	//game variables
	uint8_t enPassent[16] = { 0 };


	//casts a line with a certain vector until it hits a piece
	std::vector<std::vector<int>> raycast(int x, int y, int dx, int dy, int len, bool isWhite, uint8_t board[8][8]);

	//gets available moves for a piece
	std::vector<std::vector<int>> availableMoves(int x, int y, uint8_t pieceCheck, uint8_t board[8][8]);

	//gets all possible moves for a colour
	std::vector<std::vector<int>> getAllMoves(bool colourToCheck, uint8_t board[8][8]);



public:
	bool* isWhiteTurn;
	int* move;
	bool wCheck;
	bool bCheck;

	//returns all legal moves for piece
	std::vector<std::vector<int>> legalMoves(int x, int y, uint8_t pieceCheck, uint8_t board[8][8], bool colour);

	//gets all possible moves for a colour
	std::vector<std::vector<int>> getAllLegalMoves(bool colourToCheck, uint8_t board[8][8]);

	//returns if each king is in check
	bool checkCheck(uint8_t board[8][8], bool colour);

	//checks if a move is valid and makes the move
	int makeMove(int pieceHeldX, int pieceHeldY, int squareX, int squareY, uint8_t &pieceHeld, uint8_t board[8][8]);

	//promotes a pawn
	bool promotePiece(uint8_t promoteTo, bool pieceCol, uint8_t board[][8]);
};