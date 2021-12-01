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
					if (returnVal == 1 | returnVal == 3) {
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
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 7 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + squareSize * 0.2 + chessGamePtr->renderer.squareSize)
				{
					//promotes to queen
					chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);
					//resets variable
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}
				//same for rook
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + squareSize * 0.2 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(ROOK, colour, chessGamePtr->board);
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}
				//same for bishop
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + 10 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(BISHOP, colour, chessGamePtr->board);
					chessGamePtr->renderer.bPromotionsVisible = 0;
					return true;
				}
				//same for knight
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 &&
					*mousePosY > chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy + chessGamePtr->renderer.squareSize * 8 + squareSize * 0.2 + chessGamePtr->renderer.squareSize)
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
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 7 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 + chessGamePtr->renderer.squareSize)
				{
					//promote pawn to queen
					chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);
					//reset promoting
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
				//same for rook
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 6 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(ROOK, colour, chessGamePtr->board);
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
				//same for bishop
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 5 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(BISHOP, colour, chessGamePtr->board);
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
				//same for knight
				else if (*mousePosX > chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 &&
					*mousePosY > chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 &&
					*mousePosX < chessGamePtr->renderer.startingPosx + chessGamePtr->renderer.squareSize * 4 + chessGamePtr->renderer.squareSize &&
					*mousePosY < chessGamePtr->renderer.startingPosy - chessGamePtr->renderer.squareSize - squareSize * 0.2 + chessGamePtr->renderer.squareSize)
				{
					chessGamePtr->moveManager.promotePiece(KNIGHT, colour, chessGamePtr->board);
					chessGamePtr->renderer.wPromotionsVisible = 0;
					return true;
				}
			}
		}
		
		//*mouseClick = false;
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

		if (returnVal == 1 | returnVal == 3) return true;

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


bool MagnetPlayer::update() {
	char move = magnetControl.update();

	if (move) {
		move -= 1;
		if (pieceHeld == 0) {
			pieceHeld = chessGamePtr->board[move >> 3][move % 8];
			pieceHeldX = move % 8;
			pieceHeldY = move >> 3;
		}
		
		else {
			int returnVal = chessGamePtr->moveManager.makeMove(
				pieceHeldX, 
				pieceHeldY, 
				move % 8,
				move >> 3, 
				pieceHeld, 
				chessGamePtr->board
			);

			if (returnVal == 1 | returnVal == 3) {
				pieceHeld = 0;
				chessGamePtr->renderer.pieceHeld = pieceHeld;
				return true;
			}

		}
	}

	chessGamePtr->renderer.pieceHeld = pieceHeld;
	chessGamePtr->renderer.pieceHeldX = pieceHeldX;
	chessGamePtr->renderer.pieceHeldY = pieceHeldY;

	chessGamePtr->renderer.available = chessGamePtr->moveManager.legalMoves(
		pieceHeldX,
		pieceHeldY,
		pieceHeld,
		chessGamePtr->board,
		colour);

	return false;
}


bool BoardPlayerRand::update() {
	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 8; x++) {
			movementController.board[y][x] = chessGamePtr->board[y][x];
		}
	movementController.genVectorBoard();

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

		if (returnVal == 1) {
			movementController.movePiece(available[random][0],
				available[random][1],
				available[random][2],
				available[random][3]);
			movementController.moveToSquare(4, 4);
			return true;
		}

		else if (returnVal == 3) {

			//capture function

			return true;
		}

		else if (returnVal == 2) {
			int random = (rand() % 4) + 1;
			uint8_t piece = 0;
			piece = pow(2, random);
			chessGamePtr->moveManager.promotePiece(piece, colour, chessGamePtr->board);
			
			
			//promotion function


			return true;
		}
	}

	return false;
}

/*
int minimaxCalls = 0;

int MiniMaxAI::minimax(uint8_t board[8][8], int depth, bool maxPlayer, bool player, int alpha, int beta) {
	minimaxCalls++;

	if (chessGamePtr->moveManager.getAllLegalMoves(0, board).size() == 0) {
		//checkmate
		if (chessGamePtr->moveManager.checkCheck(board,0)) {
			return -9999999999;
		}
		//stlemate
		else {
			return 0;
		}
	}
	else if (chessGamePtr->moveManager.getAllLegalMoves(1, board).size() == 0) {
		//checkmate
		if (chessGamePtr->moveManager.checkCheck(board, 1)) {
			return 9999999999;
		}
		//stalemate
		else {
			return 0;
		}
	}
	
	if (depth <= 0)
		return staticEval(board);

	if (maxPlayer) {
		int bestScore = -99999;

		for (std::vector<int> i : chessGamePtr->moveManager.getAllLegalMoves(player, board)) {
			//copies board
			uint8_t newBoard[8][8];

			//copies board
			for (int x = 0; x < WIDTH; x++) {
				for (int y = 0; y < HEIGHT; y++) {
					newBoard[y][x] = board[y][x];
				}
			}

			chessGamePtr->moveManager.makeMove(i[0], i[1], i[2], i[3], newBoard[i[1]][i[0]], newBoard);
			
			int score;

			score = minimax(board, depth - 1, false, !player, alpha, beta);

			bestScore = score > bestScore ? score : bestScore;

			alpha = score > alpha ? score : alpha;

			if (beta <= alpha) {
				break;
			}
		}
		return bestScore;
	}
	else {
		int bestScore = 99999;

		for (std::vector<int> i : chessGamePtr->moveManager.getAllLegalMoves(player, board)) {
			//copies board
			uint8_t newBoard[8][8];

			//copies board
			for (int x = 0; x < WIDTH; x++) {
				for (int y = 0; y < HEIGHT; y++) {
					newBoard[y][x] = board[y][x];
				}
			}

			newBoard[i[3]][i[2]] = newBoard[i[1]][i[0]];
			newBoard[i[1]][i[0]] = 0;
			
			int score;

			score = minimax(board, depth - 1, true, !player, alpha, beta);

			bestScore = score > bestScore ? score : bestScore;

			beta = score < beta ? score : beta;

			if (beta <= alpha) {
				break;
			}
		}
		return bestScore;
	}
}

int MiniMaxAI::staticEval(uint8_t board[8][8]) {
	int rnd = (rand() % 100) + 1;

	return rnd;
}

bool MiniMaxAI::update() {
	srand(time(NULL));

	int bestScore = -99999;
	std::vector<int> bestMove;

	minimaxCalls = 0;


	for (std::vector<int> i : chessGamePtr->moveManager.getAllLegalMoves(colour, chessGamePtr->board)) {
		//copies board
		uint8_t newBoard[8][8];

		//copies board
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				newBoard[y][x] = chessGamePtr->board[y][x];
			}
		}

		newBoard[i[3]][i[2]] = newBoard[i[1]][i[0]];
		newBoard[i[1]][i[0]] = 0;

		int score = minimax(newBoard, 5, false, colour, -99999, 99999);

		if (score > bestScore) {
			bestScore = score;
			bestMove = i;
		}
	}

	std::cout << minimaxCalls << "\n";

	return chessGamePtr->moveManager.makeMove(bestMove[0], bestMove[1], bestMove[2], bestMove[3], chessGamePtr->board[bestMove[1]][bestMove[0]], chessGamePtr->board);
}
*/