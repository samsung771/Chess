#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>

#include "Players.h"
#include "ChessGame.h"
#include <iostream>

int main(int argc, char* argv[]) {
	int mousePosX, mousePosY;
	bool mouseClick;

	Human player1(&mousePosX, &mousePosY, &mouseClick);
	//Human player2(&mousePosX, &mousePosY, &mouseClick);
	RandAI player2;
	//MagnetPlayer player2;

	player1.colour = 0;
	player2.colour = 1;

	ChessGame chess(&player1, &player2, &mousePosX, &mousePosY, &mouseClick);

	chess.Play();

	return 0;
}