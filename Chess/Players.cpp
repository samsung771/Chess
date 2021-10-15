#include "Players.h"

Player::Player() {

}

void Player::update() {

}


Human::Human(int* mouseX, int* mouseY, bool* mouseCl) : Player() {
	mousePosX = mouseX;
	mousePosY = mouseY;
	mouseClick = mouseCl;
}

void Human::update() {
	int squareSize = chessGamePtr->renderer.getSquareSize();
	int startingPosx = chessGamePtr->renderer.getStartPosX();
	int startingPosy = chessGamePtr->renderer.getStartPosY();
	if (*mouseClick) {
		//gets square that was clicked
		int squareX = floor((*mousePosX - startingPosx) / squareSize);
		int squareY = floor((*mousePosY - startingPosy) / squareSize);

		if (squareX >= 0 && squareX < WIDTH && squareY >= 0 && squareY < HEIGHT) {
			int col = (chessGamePtr->board[squareY][squareX] & COLOURMASK) >> 7;

			//check if kings are in check
			std::vector<bool> temp = chessGamePtr->moveManager.checkCheck(chessGamePtr->board);
			check = temp[(int)colour];

			if (chessGamePtr->board[squareY][squareX] != 0) {
				//if clicking on a piece of your colour
				if (colour == col) {
					//pickup piece if you aren't holding one
					if (pieceHeld == 0) {
						pieceHeld = chessGamePtr->board[squareY][squareX];
						chessGamePtr->board[squareY][squareX] = 0;
						pieceHeldX = squareX;
						pieceHeldY = squareY;
					}
					//replace held piece if you are holding a piece
					else if (pieceHeld != 0) {
						chessGamePtr->board[pieceHeldY][pieceHeldX] = pieceHeld;
						pieceHeld = 0;
					}
				}
			}

			else {
				if (chessGamePtr->moveManager.makeMove(pieceHeldX, pieceHeldY, squareX, squareY, pieceHeld, chessGamePtr->board))
					pieceHeld = 0;
			}
		}

		
		chessGamePtr->renderer.pieceHeld = pieceHeld;
		chessGamePtr->renderer.pieceHeldX = pieceHeldX;
		chessGamePtr->renderer.pieceHeldY = pieceHeldY;
		chessGamePtr->renderer.mousePosY = mousePosY;
		chessGamePtr->renderer.mousePosX = mousePosX;
		chessGamePtr->renderer.available = chessGamePtr->moveManager.legalMoves(pieceHeldX,pieceHeldY,pieceHeld,chessGamePtr->board);
		
		*mouseClick = false;
	}
}
