#include "Players.h"

Player::Player() {

}

bool Player::update() {
	return false;
}


Human::Human(int* mouseX, int* mouseY, bool* mouseCl) : Player() {
	mousePosX  = mouseX;
	mousePosY  = mouseY;
	mouseClick = mouseCl;
}

bool Human::update() {
	int squareSize   = chessGamePtr->renderer.getSquareSize();
	int startingPosx = chessGamePtr->renderer.getStartPosX();
	int startingPosy = chessGamePtr->renderer.getStartPosY();

	if (*mouseClick) {
		//gets square that was clicked
		int squareX = floor((*mousePosX - startingPosx) / squareSize);
		int squareY = floor((*mousePosY - startingPosy) / squareSize);

		if (squareX >= 0 && squareX < WIDTH && squareY >= 0 && squareY < HEIGHT) {
			int col = (chessGamePtr->board[squareY][squareX] & COLOURMASK) >> 7;

			//if clicking on a piece of your colour
			if (chessGamePtr->board[squareY][squareX] != 0 && colour == col) {
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

			else {
				int returnVal = chessGamePtr->moveManager.makeMove(pieceHeldX, pieceHeldY, squareX, squareY, pieceHeld, chessGamePtr->board);
				if (returnVal == 1) {
					pieceHeld = 0;
					return true;
				}
				else if (returnVal == 2) {
					chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);
					return true;
				}
			}
		}

		
		chessGamePtr->renderer.pieceHeld  =  pieceHeld;
		chessGamePtr->renderer.pieceHeldX =  pieceHeldX;
		chessGamePtr->renderer.pieceHeldY =  pieceHeldY;
		chessGamePtr->renderer.mousePosY  =  mousePosY;
		chessGamePtr->renderer.mousePosX  =  mousePosX;

		chessGamePtr->renderer.available  =  chessGamePtr->moveManager.legalMoves(
			pieceHeldX,
			pieceHeldY,
			pieceHeld,
			chessGamePtr->board,
			colour);
		
		*mouseClick = false;
	}

	return false;
}


bool RandAI::update() {
	std::vector<std::vector<int>> available = chessGamePtr->moveManager.getAllLegalMoves(colour, chessGamePtr->board);

	srand(time(NULL));
	int random = rand() % available.size();

	if (available.size() > 0) {
		if (chessGamePtr->moveManager.makeMove(available[random][0],
			available[random][1],
			available[random][2],
			available[random][3],
			chessGamePtr->board[available[random][1]][available[random][0]],
			chessGamePtr->board))

			return true;
	}

	return false;
}