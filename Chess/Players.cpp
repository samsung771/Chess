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
				//if clicking an enemy or blank square
				else {
					int returnVal = chessGamePtr->moveManager.makeMove(pieceHeldX, pieceHeldY, squareX, squareY, pieceHeld, chessGamePtr->board);
					//if its normal move or capture
					if (returnVal && (returnVal != 2 || returnVal != 6)) {
						pieceHeld = 0;
						chessGamePtr->renderer.pieceHeld = pieceHeld;
						return true;
					}
					//if its a promotion
					else if (returnVal) {
						//render promotion buttons
						if (colour) chessGamePtr->renderer.bPromotionsVisible = 1;
						else chessGamePtr->renderer.wPromotionsVisible = 1;
						pieceHeld = 0;
						chessGamePtr->renderer.pieceHeld = 0;
						return false;
					}
				}
			}

			chessGamePtr->renderer.pieceHeld  = pieceHeld;
			chessGamePtr->renderer.pieceHeldX = pieceHeldX;
			chessGamePtr->renderer.pieceHeldY = pieceHeldY;
			chessGamePtr->renderer.mousePosY  = mousePosY;
			chessGamePtr->renderer.mousePosX  = mousePosX;

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
	}

	return false;
}


bool RandAI::update() {
	std::vector<std::vector<int>> available = chessGamePtr->moveManager.getAllLegalMoves(colour, chessGamePtr->board);

	srand(time(NULL));
	int random = rand() % available.size();

	//if there is a legal move
	if (available.size() > 0) {
		//try to make the first move
		int returnVal = chessGamePtr->moveManager.makeMove(available[random][0],
			available[random][1],
			available[random][2],
			available[random][3],
			chessGamePtr->board[available[random][1]][available[random][0]],
			chessGamePtr->board);

		//if its a normal move or capture return true
		if (returnVal && (returnVal != 2 || returnVal != 6))  return true;

		//if its a promotion
		else if (returnVal) {
			int random = (rand() % 4) + 1;
			uint8_t piece = 0;
			//promote to a random piece
			piece = pow(2, random);
			chessGamePtr->moveManager.promotePiece(piece, colour, chessGamePtr->board);
			return true;
		}
	}
	//else return false
	return false;
}


bool MagnetPlayer::update() {
	//get next message
	char move = magnetControl.update();

	//if you can't pick up the piece on that square then return false
	if (chessGamePtr->board[((move-1) & 0b01111111) >> 3][((move-1) & 0b01111111) % 8] & COLOURMASK == !colour)
		return false;

	//if there is a command
	if (move) {
		//positions are sent +1 so that position 0,0 isnt ignored
		move -= 1;
		//if picking up a piece
		if ((move+1) & 0b10000000) {
			move &= 0b01111111;
			pieceHeld = chessGamePtr->board[move >> 3][move % 8];
			pieceHeldX = move % 8;
			pieceHeldY = move >> 3;
		}
		//if putting down a piece
		else if (pieceHeld) {
			//make the move
			int returnVal = chessGamePtr->moveManager.makeMove(
				pieceHeldX, 
				pieceHeldY, 
				move % 8,
				move >> 3, 
				pieceHeld, 
				chessGamePtr->board
			);

			if (returnVal && (returnVal != 2 || returnVal != 6)) {
				pieceHeld = 0;
				chessGamePtr->renderer.pieceHeld = pieceHeld;
				return true;
			}

			if (returnVal) {
				chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);
			}
		}
	}

	//update vars
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
	//update movement controller board
	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 8; x++) 
			movementController.board[y][x] = chessGamePtr->board[y][x];
	
	//generate node matrix for pathfinding
	movementController.genVectorBoard();

	//get all moves and get a random number
	std::vector<std::vector<int>> available = chessGamePtr->moveManager.getAllLegalMoves(colour, chessGamePtr->board);

	srand(time(NULL));
	int random = rand() % available.size();

	//if there is a legal move
	if (available.size() > 0) {
		//make the random move
		int returnVal = chessGamePtr->moveManager.makeMove(available[random][0],
			available[random][1],
			available[random][2],
			available[random][3],
			chessGamePtr->board[available[random][1]][available[random][0]],
			chessGamePtr->board);

		//if its a normal move, just move piece
		if (returnVal == 1) {
			movementController.movePiece(available[random][0],
				available[random][1],
				available[random][2],
				available[random][3]);
			return true;
		}
		//if its a capture the original piece needs to be moved
		else if (returnVal == 3) {
			movementController.capturePiece(available[random][0],
				available[random][1],
				available[random][2],
				available[random][3]);

			return true;
		}
		//en passent move
		else if (returnVal == 5) {
			movementController.epCapturePiece(available[random][0],
				available[random][1],
				available[random][2],
				available[random][3]);

			return true;
		}
		//castling
		else if (returnVal == 4) {
			movementController.castle(available[random][0],
				available[random][1],
				available[random][2],
				available[random][3]);

			return true;
		}
		//if its a promotion
		else if (returnVal == 2) {
			int random = (rand() % 4) + 1;
			uint8_t piece = 0;
			piece = pow(2, random);
			chessGamePtr->moveManager.promotePiece(piece, colour, chessGamePtr->board);
			
			//promotion function
			//i cant do this as it requires too many extra pieces

			return true;
		}
		//if its a promotion and capture
		else if (returnVal == 6) {
			int random = (rand() % 4) + 1;
			uint8_t piece = 0;
			piece = pow(2, random);
			chessGamePtr->moveManager.promotePiece(piece, colour, chessGamePtr->board);
			//promotion function
			//i cant do this as it requires too many extra pieces

			movementController.capturePiece(available[random][0],
				available[random][1],
				available[random][2],
				available[random][3]);
			return true;
		}
	}

	return false;
}


bool BoardPlayerAPI::update() {
	//update movement controller board
	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 8; x++)
			movementController.board[y][x] = chessGamePtr->board[y][x];

	//generate node matrix for pathfinding
	movementController.genVectorBoard();

	system("python getMove.py");
	std::ifstream moveF;
	moveF.open("moves.txt");
	char move[4];
	moveF.read(move, 4);
		
	//make the random move
	int returnVal = chessGamePtr->moveManager.makeMove(
		int(move[0]) - 97,
		7 - (int(move[1]) - 49),
		int(move[2]) - 97,
		7 - (int(move[3]) - 49),
		chessGamePtr->board[7 - (int(move[1]) - 49)][int(move[0]) - 97],
		chessGamePtr->board);

	//if its a normal move, just move piece
	if (returnVal == 1) {
		movementController.movePiece(int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49));
		return true;
	}
	//if its a capture the original piece needs to be moved
	else if (returnVal == 3) {
		movementController.capturePiece(int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49));

		return true;
	}
	//en passent move
	else if (returnVal == 5) {
		movementController.epCapturePiece(int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49));

		return true;
	}
	//castling
	else if (returnVal == 4) {
		movementController.castle(int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49));

		return true;
	}
	//if its a promotion
	else if (returnVal == 2) {
		chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);

		//promotion function
		//i cant do this as it requires too many extra pieces

		movementController.movePiece(int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49));
		return true;
	}
	//if its a promotion and capture
	else if (returnVal == 6) {
		chessGamePtr->moveManager.promotePiece(QUEEN, colour, chessGamePtr->board);

		//promotion function
		//i cant do this as it requires too many extra pieces

		movementController.capturePiece(int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49));
		return true;
	}
	
	return false;
}


MagnetPlayerAPI::MagnetPlayerAPI() {
	system("python acceptChallenge.py");
}

bool MagnetPlayerAPI::update() {
	//get next message
	char move = magnetControl.update();

	//if you can't pick up the piece on that square then return false
	if (move & 0b10000000) {
		if ((chessGamePtr->board[((move - 1) & 0b01111111) >> 3][((move - 1) & 0b01111111) % 8] & COLOURMASK) >> 7 == !colour)
			return false;
	}

	//if there is a command 0s are ignored
	if (move) {
		//positions are sent +1 so that position 0,0 isnt ignored
		move -= 1;
		//if picking up a piece
		if ((move + 1) & 0b10000000) {
			move &= 0b01111111;
			pieceHeld = chessGamePtr->board[move >> 3][move % 8];
			pieceHeldX = move % 8;
			pieceHeldY = move >> 3;
		}
		//if putting down a piece
		else if (pieceHeld) {
			//make the move
			int returnVal = chessGamePtr->moveManager.makeMove(
				pieceHeldX,
				pieceHeldY,
				move % 8,
				move >> 3,
				pieceHeld,
				chessGamePtr->board
			);
			//if its a valid move
			if (returnVal) {
				//generate command variables
				char command[50] = "python makeMove.py ";
				char ay[2] = { char((8 - pieceHeldY) + 48), 0 };
				char ax[2] = { lettersLookup[pieceHeldX],   0 };
				char by[2] = { char((8 - (move >> 3)) + 48),0 };
				char bx[2] = { lettersLookup[move % 8],     0 };

				//combine to one command
				strcat_s(command, 50, ax);
				strcat_s(command, 50, ay);
				strcat_s(command, 50, bx);
				strcat_s(command, 50, by);

				//send command to shell
				int pyReturn = system(command);
				std::cout << pyReturn << '\n';

				pieceHeld = 0;
				chessGamePtr->renderer.pieceHeld = pieceHeld;

				if (pyReturn)
					return false;
				return true;
			}
		}
	}

	//update vars
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


APIHuman::APIHuman(int* mouseX, int* mouseY, bool* mouseCl) : Player() {
	mousePosX = mouseX;
	mousePosY = mouseY;
	mouseClick = mouseCl;

	system("python acceptChallenge.py");
}

bool APIHuman::update() {
	int squareSize = chessGamePtr->renderer.getSquareSize();
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
				//if clicking an enemy or blank square
				else {
					int returnVal = chessGamePtr->moveManager.makeMove(pieceHeldX, pieceHeldY, squareX, squareY, pieceHeld, chessGamePtr->board);
					if (returnVal) {
						char command[50] = "python makeMove.py ";
						char ay[2] = { char((8 - pieceHeldY) + 48),0 };
						char ax[2] = { lettersLookup[pieceHeldX],0 };
						char by[2] = { char((8 - squareY) + 48),0 };
						char bx[2] = { lettersLookup[squareX],0 };
						strcat_s(command, 50, ax);
						strcat_s(command, 50, ay);
						strcat_s(command, 50, bx);
						strcat_s(command, 50, by);
						system(command);
					}
					//if its normal move or capture
					if (returnVal && (returnVal != 2 || returnVal != 6)) {
						pieceHeld = 0;
						chessGamePtr->renderer.pieceHeld = pieceHeld;
						return true;
					}
					//if its a promotion
					else if (returnVal) {
						//render promotion buttons
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
	}

	return false;
}


bool APIPlayer::update() {
	system("python getMove.py");
	std::ifstream moveF;
	moveF.open("moves.txt");
	char move[4];
	moveF.read(move, 4);
	chessGamePtr->moveManager.makeMove(
		int(move[0])-97,
		7-(int(move[1])-49),
		int(move[2])-97,
		7-(int(move[3])-49),
		chessGamePtr->board[7-(int(move[1]) - 49)][int(move[0]) - 97],
		chessGamePtr->board);
	return true;
}