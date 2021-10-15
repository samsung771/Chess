#include "MoveManager.h"

std::vector<std::vector<int>> MoveManager::raycast(int x, int y, int dx, int dy, int len, bool isWhite, uint8_t board[8][8]) {
	std::vector<std::vector<int>> path;
	for (int i = 0; i < len; i++) {
		//add vector
		x += dx;
		y += dy;

		//check if it is still in play space w/ 1 at the end to indicate empty
		if (y >= 0 && y < 8 && x >= 0 && x < 8) {
			//add to ray if space is empty
			if (board[y][x] == 0) {
				path.push_back({ x,y, 0 });
			}
			//add to ray if the piece is an enemy w/ 1 at the end to indicate a capture
			else {
				getPiece(x, y);
				if (isWhite == colour) {
					path.push_back({ x,y, 1 });
				}
				break;
			}
		}
		else break;
	}
	return path;
}

std::vector<std::vector<int>> MoveManager::availableMoves(int x, int y, uint8_t pieceCheck, uint8_t board[8][8]) {
	int piece = (pieceCheck & PIECEMASK);
	bool colour = (pieceCheck & COLOURMASK) >> 7;
	bool hasMoved = !((pieceCheck & MOVEMASK) >> 6);

	std::vector<std::vector<int>> possibleMoves;
	switch (piece) {
	case PAWN:
		//check move captures
		// (!colour * -1) | 1 means it will go in the forward direction for that colour 
		//e.g. +1 rank for black -1 rank for white
		for (std::vector<int> i : raycast(x, y, 1, (!colour * -1) | 1, 1, !colour, board))
			if (i[2] == 1)	possibleMoves.push_back(i);
		for (std::vector<int> i : raycast(x, y, -1, (!colour * -1) | 1, 1, !colour, board))
			if (i[2] == 1)	possibleMoves.push_back(i);

		if (!hasMoved) {
			//check move forward 2
			for (std::vector<int> i : raycast(x, y, 0, (!colour * -1) | 1, 2, !colour, board)) {
				if (i[2] == 0)	possibleMoves.push_back(i);
			}
		}
		else {
			//check move forward 1
			for (std::vector<int> i : raycast(x, y, 0, (!colour * -1) | 1, 1, !colour, board))
				if (i[2] == 0)	possibleMoves.push_back(i);
		}

		int add;

		if (colour != 0) add = 0;
		else add = 8;

		//check en passent squares
		//left
		if (enPassent[x + 1 + add] == y + ((!colour * -1) | 1)) {
			for (std::vector<int> i : raycast(x, y, 1, (!colour * -1) | 1, 1, !colour, board)) {
				if (i[2] == 0) {
					i[2] = 2;
					possibleMoves.push_back(i);
				}
			}
		}
		//right
		if (enPassent[x - 1 + add] == y + ((!colour * -1) | 1)) {
			for (std::vector<int> i : raycast(x, y, -1, (!colour * -1) | 1, 1, !colour, board)) {
				if (i[2] == 0) {
					i[2] = 2;
					possibleMoves.push_back(i);
				}
			}
		}
		break;
	case KNIGHT:
		//checks knight moves
		for (int i = 0; i < 8; i++) {
			for (std::vector<int> i : raycast(x, y, knightMoves[i][0], knightMoves[i][1], 1, !colour, board))
				possibleMoves.push_back(i);
		}
		break;
	case BISHOP:
		//checks bishop moves
		for (int i = 0; i < 4; i++) {
			for (std::vector<int> i : raycast(x, y, bishopMoves[i][0], bishopMoves[i][1], 10, !colour, board))
				possibleMoves.push_back(i);
		}
		break;
	case ROOK:
		//checks rook moves
		for (int i = 0; i < 4; i++) {
			for (std::vector<int> i : raycast(x, y, rookMoves[i][0], rookMoves[i][1], 10, !colour, board))
				possibleMoves.push_back(i);
		}
		break;
	case QUEEN:
		//check queen moves
		for (int i = 0; i < 8; i++) {
			for (std::vector<int> i : raycast(x, y, allMoves[i][0], allMoves[i][1], 10, !colour, board))
				possibleMoves.push_back(i);
		}
		break;
	case KING:
		//check king moves
		for (int i = 0; i < 8; i++) {
			for (std::vector<int> i : raycast(x, y, allMoves[i][0], allMoves[i][1], 1, !colour, board))
				possibleMoves.push_back(i);
		}

		//checks if king is in check
		bool inCheck = 0;

		/*
		if (colour)
			inCheck = wCheck;
		else
			inCheck = bCheck;
		*/

		//check for castling
		//must not have moved and not be in check
		if (!hasMoved && !inCheck) {
			//kingside
			for (int i = 4; i < 8; i++) {
				int Rookpiece = board[y][i] & PIECEMASK;
				bool Rookcolour = (board[y][i] & COLOURMASK) >> 7;
				bool RookhasMoved = !((board[y][i] & MOVEMASK) >> 6);

				if (Rookpiece == ROOK && !RookhasMoved && Rookcolour == colour)
					possibleMoves.push_back({ x + 2,y,0 });
				else if (Rookpiece != 0)
					break;
			}
			//queenside
			for (int i = 4; i >= 0; i--) {
				int Rookpiece = board[y][i] & PIECEMASK;
				bool Rookcolour = (board[y][i] & COLOURMASK) >> 7;
				bool RookhasMoved = !((board[y][i] & MOVEMASK) >> 6);

				if (Rookpiece == ROOK && !RookhasMoved && Rookcolour == colour)
					possibleMoves.push_back({ x - 2,y,0 });
				else if (Rookpiece != 0)
					break;
			}
		}
		break;
	}


	return possibleMoves;
}

std::vector<std::vector<int>> MoveManager::getAllMoves(bool colourToCheck, uint8_t board[8][8]) {
	std::vector<std::vector<int>> moves;

	//goes through each piece in the board
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			//if there is a piece there
			if ((board[y][x] & PIECEMASK) != 0) {
				//if it is the colour that is getting checked
				bool colour = (board[y][x] & COLOURMASK) >> 7;
				if (colour == colourToCheck) {
					//add available moves for that piece
					for (std::vector<int> i : availableMoves(x, y, board[y][x], board)) {
						std::vector<int> temp = { x,y,i[0],i[1] };
						moves.push_back(temp);
					}
				}
			}
		}
	}
	return moves;
}

std::vector<std::vector<int>> MoveManager::legalMoves(int x, int y, uint8_t pieceCheck, uint8_t board[8][8]) {
	std::vector<std::vector<int>> legalMoves;

	//get all available moves
	for (std::vector<int> i : availableMoves(x, y, pieceCheck, board)) {
		//copies board
		uint8_t newBoard[8][8];
		memcpy(newBoard, board, sizeof(board));
		//makes the possible move on the new copied board
		newBoard[i[1]][i[0]] = pieceCheck;

		//check if move will put the king in check
		if (!(checkCheck(newBoard)[!isWhiteTurn])) {
			//if not add it as a legal move
			legalMoves.push_back(i);
		}
	}

	return legalMoves;
}

std::vector<std::vector<int>> MoveManager::getAllLegalMoves(bool colourToCheck, uint8_t board[8][8]) {
	std::vector<std::vector<int>> moves;

	//goes through each piece in the board
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			//if there is a piece there
			if ((board[y][x] & PIECEMASK) != 0) {
				//if it is the colour that is getting checked
				bool colour = (board[y][x] & COLOURMASK) >> 7;
				if (colour == colourToCheck) {
					//add available moves for that piece
					for (std::vector<int> i : legalMoves(x, y, board[y][x], board)) {
						std::vector<int> temp = { x,y,i[0],i[1] };
						moves.push_back(temp);
					}
				}
			}
		}
	}
	return moves;
}

std::vector<bool> MoveManager::checkCheck(uint8_t board[8][8]) {
	int wchecks = 0;
	int bchecks = 0;
	//looks through all possible moves for white
	for (std::vector<int> i : getAllMoves(1, board)) {
		//if a piece can capture the king then add a check
		if ((board[i[3]][i[2]] & PIECEMASK) == KING)
			bchecks++;
	}
	//do the same for black
	for (std::vector<int> i : getAllMoves(0, board)) {
		if ((board[i[3]][i[2]] & PIECEMASK) == KING)
			wchecks++;

	}

	//make the ints bools
	if (bchecks > 0) bchecks = 1;
	if (wchecks > 0) wchecks = 1;

	//return checks
	return { (bool)bchecks, (bool)wchecks };
}

bool MoveManager::makeMove(int pieceHeldX, int pieceHeldY, int squareX, int squareY, uint8_t pieceHeld, uint8_t board[8][8]) {
	int validMove = 0;

	//checks square is within play space
	if (squareX >= 0 && squareX < WIDTH && squareY >= 0 && squareY < HEIGHT) {
		int colour = (board[squareY][squareX] & COLOURMASK) >> 7;
		//checks available moves
		std::vector<std::vector<int>> available = legalMoves(pieceHeldX, pieceHeldY, pieceHeld, board);

		//place piece in empty square
		if (board[squareY][squareX] == 0 && pieceHeld != 0) {
			//if it is placing it in the same place
			if (squareX == pieceHeldX && squareY == pieceHeldY) {
				board[squareY][squareX] = pieceHeld;
				pieceHeld = 0;
			}
			else {
				//check if the move is allowed
				for (std::vector<int> i : available) {
					if (i[0] == squareX && i[1] == squareY) {
						board[squareY][squareX] = pieceHeld;
						board[pieceHeldY][pieceHeldX] = 0;
						pieceHeld = 0;

						//move rook when king castles
						if ((board[squareY][squareX] & PIECEMASK) == KING && abs(squareX - pieceHeldX) == 2) {
							if (squareX - pieceHeldX < 0) {
								board[squareY][squareX + 1] = board[squareY][0];
								board[squareY][0] = 0;
							}
							else {
								board[squareY][squareX - 1] = board[squareY][7];
								board[squareY][7] = 0;
							}
						}

						//adds enPassent squares
						if ((board[squareY][squareX] & PIECEMASK) == PAWN) {
							if (squareY - pieceHeldY == 2)
								enPassent[squareX + 8] = squareY - 1;
							else if (squareY - pieceHeldY == -2)
								enPassent[squareX] = squareY + 1;
							else if (!((board[squareY][squareX] & COLOURMASK) >> 7)) {
								enPassent[squareX + 8] = 0;
								enPassent[pieceHeldX] = 0;
							}
							else {
								enPassent[squareX] = 0;
								enPassent[pieceHeldX] = 0;
							}
						}

						//en passent capture
						if (i[2] == 2) {
							if (!((board[squareY][squareX] & COLOURMASK) >> 7))
								board[squareY + 1][squareX] = 0;
							else
								board[squareY - 1][squareX] = 0;
						}

						//stores that piece has moved
						if ((board[squareY][squareX] & MOVEMASK) >> 6 == 1)
							board[squareY][squareX] -= MOVEMASK;

						validMove++;

						break;
					}
				}
				//if move is not valid replace piece in its original position
				if (validMove == 0) {
					board[pieceHeldY][pieceHeldX] = pieceHeld;
					pieceHeld = 0;
					validMove = 0;
				}
			}
		}

		//capture piece
		else if (board[squareY][squareX] != 0 && pieceHeld != 0) {
			//checks for available moves
			for (std::vector<int> i : available) {
				if (i[0] == squareX && i[1] == squareY) {
					//place piece
					board[squareY][squareX] = pieceHeld;
					board[pieceHeldY][pieceHeldX] = 0;

					//removes en passent capture if the pawn is captured
					if ((board[squareY][squareX] & PIECEMASK) == PAWN) {
						if (!((pieceHeld & COLOURMASK) >> 7)) {
							enPassent[squareX + 8] = 0;
							enPassent[pieceHeldX + 8] = 0;
						}
						else {
							enPassent[squareX] = 0;
							enPassent[pieceHeldX] = 0;
						}
					}
					validMove++;

				}
			}
			//replace held piece if a valid move is not made
			if (validMove == 0) {
				board[pieceHeldY][pieceHeldX] = pieceHeld;
				pieceHeld = 0;
			}
		}
	}

	if (validMove > 0)
		return true;
	else
		return false;
}

