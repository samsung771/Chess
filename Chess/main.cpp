#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>
//#include <thread>

#include "Players.h"
#include "ChessGame.h"
#include <iostream>

int main(int argc, char* argv[]) {
	int mousePosX, mousePosY;
	bool mouseClick;

	APIHuman player1(&mousePosX, &mousePosY, &mouseClick);
	//Human player2(&mousePosX, &mousePosY, &mouseClick);
	//RandAI player2;
	APIPlayer player2;
	
	/*
	MagnetPlayerAPI player1;
	BoardPlayerAPI player2;
	*/

	player1.colour = 0;
	player2.colour = 1;

	ChessGame chess(&player1, &player2, &mousePosX, &mousePosY, &mouseClick);

	chess.Play();

	return 0;
}