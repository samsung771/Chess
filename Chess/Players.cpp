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
		if (!chessGamePtr->renderer.bPromotionsVisible && !chessGamePtr->renderer.wPromotionsVisible) {
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
						chessGamePtr->renderer.pieceHeld = pieceHeld;
						return true;
					}
					else if (returnVal == 2) {
						if (colour) chessGamePtr->renderer.bPromotionsVisible = 1;
						else chessGamePtr->renderer.wPromotionsVisible = 1;
						pieceHeld = 0;
						chessGamePtr->renderer.pieceHeld = 0;
						return false;
					}
				}
			}


			chessGamePtr->renderer.pieceHeld = pieceHeld;
			chessGamePtr->renderer.pieceHeldX = pieceHeldX;
			chessGamePtr->renderer.pieceHeldY = pieceHeldY;
			chessGamePtr->renderer.mousePosY = mousePosY;
			chessGamePtr->renderer.mousePosX = mousePosX;

			chessGamePtr->renderer.available = chessGamePtr->moveManager.legalMoves(
				pieceHeldX,
				pieceHeldY,
				pieceHeld,
				chessGamePtr->board,
				colour);
		}
		//if a pawn needs promoting
		else {
			//if black is promoting
			if (chessGamePtr->renderer.bPromotionsVisible) {
				//checks if clicked in queen button
				if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 7 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 7 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 + chessGamePtr->renderer.squareSize)
				{
					//promotes to queen
					chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);
					//resets variable
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}
				//same for rook
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(ROOK, colour, chessGamePtr->board);
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}
				//same for bishop
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(BISHOP, colour, chessGamePtr->board);
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}
				//same for knight
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(KNIGHT, colour, chessGamePtr->board);
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}

			}
			//if white is promoting
			else if (chessGamePtr->renderer.wPromotionsVisible) {
				//if clicked on queen button
				if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 7 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 7 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 + chessGamePtr->renderer.squareSize)
				{
					//promote pawn to queen
					chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);
					//reset promoting
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
				//same for rook
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(ROOK, colour, chessGamePtr->board);
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
				//same for bishop
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(BISHOP, colour, chessGamePtr->board);
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
				//same for knight
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(KNIGHT, colour, chessGamePtr->board);
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}

			}
		}
		
		*mouseClick = false;
	}

	return false;
}


bool RandAI::update() {
	std::vector<std::vector<int>> available = chessGamePtr->moveManager.getAllLegalMoves(colour, chessGamePtr->board);

	srand(time(NULL));
	int random = rand() % available.size();

	if (available.size() > 0) {
		int returnVal = chessGamePtr->moveManager.makeMove(available[random][0],
			available[random][1],
			available[random][2],
			available[random][3],
			chessGamePtr->board[available[random][1]][available[random][0]],
			chessGamePtr->board);

		if (returnVal == 1) return true;

		else if (returnVal == 2) {
			int random = (rand() % 4) + 1;
			uint8_t piece = 0;
			piece = pow(2, random);
			chessGamePtr->moveManager.promotePiece(piece, colour, chessGamePtr->board);
			return true;
		}
	}

	return false;
}