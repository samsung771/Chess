#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>

#include "Players.h"
#include "ChessGame.h"


#define COLOURMASK 0b10000000
#define MOVEMASK   0b01000000
#define PIECEMASK  0b00111111

#define PAWN	   0b00000001
#define KNIGHT	   0b00000010
#define BISHOP	   0b00000100
#define ROOK	   0b00001000
#define QUEEN	   0b00010000
#define KING	   0b00100000


int main(int argc, char* argv[]) {
	int mousePosX, mousePosY;
	bool mouseClick;

	Human player1(&mousePosX, &mousePosY, &mouseClick);
	Human player2(&mousePosX, &mousePosY, &mouseClick);

	player1.colour = 1;
	player2.colour = 0;

	ChessGame chess(&player1, &player2, &mousePosX, &mousePosY, &mouseClick);

	chess.Play();

	return 0;
}