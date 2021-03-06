#include "ChessGame.h"

void ChessGame::update() {
	if (isPlayer1sTurn) {
		if (player1->update()) {
			isPlayer1sTurn = !isPlayer1sTurn;

			player1->check = moveManager.checkCheck(board, 0);
			player2->check = moveManager.checkCheck(board, 1);

			renderer.bCheck = player2->check;
			renderer.wCheck = player1->check;

			moveManager.bCheck = player2->check;
			moveManager.wCheck = player1->check;

			if (moveManager.getAllLegalMoves(0, board).size() == 0) {
				//checkmate
				if (player2->check) {
					renderer.state = 1;
				}
				//satlemate
				else {
					renderer.state = 3;
				}
			}
			else if (moveManager.getAllLegalMoves(1, board).size() == 0) {
				//checkmate
				if (player1->check) {
					renderer.state = 2;
				}
				//stalemate
				else {
					renderer.state = 3;
				}
			}
		}
	}
	else {
		if (player2->update()) {
			isPlayer1sTurn = !isPlayer1sTurn;

			player1->check = moveManager.checkCheck(board, 0);
			player2->check = moveManager.checkCheck(board, 1);

			renderer.bCheck = player2->check;
			renderer.wCheck = player1->check;

			moveManager.bCheck = player2->check;
			moveManager.wCheck = player1->check;
			
			if (moveManager.getAllLegalMoves(0, board).size() == 0) {
				//checkmate
				if (player2->check) {
					renderer.state = 1;
				}
				//satlemate
				else {
					renderer.state = 3;
				}
			}
			else if (moveManager.getAllLegalMoves(1, board).size() == 0) {
				//checkmate
				if (player1->check) {
					renderer.state = 2;
				}
				//stalemate
				else {
					renderer.state = 3;
				}
			}
		}
	}
}

void ChessGame::handleEvents() {
	SDL_Event event;

	//get events
	SDL_PollEvent(&event);
	
	*mouseClick = false;

	switch (event.type)
	{
		//pressing the exit button
		case SDL_QUIT:
			isRunning = false;
			APIhandler.resign();
			break;

		//window resize event
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				renderer.resize = true;
				break;
			}
			break;

		//quit if escape key is pressed
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					isRunning = false;
					break;
			}
			break;

		//get mouse position if it is moved
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(mousePosX, mousePosY);
			break;

		//get mouse click
		case SDL_MOUSEBUTTONDOWN:
			if (!mouseDown) {
				mouseDown = true;
				*mouseClick = true;
			}
			break;


		case SDL_MOUSEBUTTONUP:
			mouseDown = false;
			break;
	};
}

void ChessGame::loadBoardFromFen(std::string fen) {
	int pointerX = 0;
	int pointerY = 0;
	int parameterPointer = 0;

	//for each thing in the fen
	for (char i : fen) {
		bool isWhite = true;

		//checks the colour
		if (i != toupper(i)) {
			isWhite = false;
			board[pointerY][pointerX] |= COLOURMASK;
		}

		//while it is looking through the board
		if (pointerY < 8) {
			//switch for each piece
			switch (tolower(i)) {
			case 'r':
				board[pointerY][pointerX] |= ROOK;
				pointerX++;
				break;
			case 'k':
				board[pointerY][pointerX] |= KING;
				pointerX++;
				break;
			case 'p':
				if (pointerY == 1 && !isWhite) board[pointerY][pointerX] |= MOVEMASK;
				else if (pointerY == 6) board[pointerY][pointerX] |= MOVEMASK;
				board[pointerY][pointerX] |= PAWN;
				pointerX++;
				break;
			case 'n':
				board[pointerY][pointerX] |= KNIGHT;
				pointerX++;
				break;
			case 'b':
				board[pointerY][pointerX] |= BISHOP;
				pointerX++;
				break;
			case 'q':
				board[pointerY][pointerX] |= QUEEN;
				pointerX++;
				break;
				//new line
			case '/':
				pointerY++;
				pointerX = 0;
				break;
				//skip open squares
			default:
				if (i - 48 > 0 && i - 48 < 9) {
					pointerX += i - 48;
				}
				break;
			};
		}
		//parameters
		else {
			//colour of the piece
			switch (i) {
				if (parameterPointer == 0) {
			case 'w':
				isPlayer1sTurn = true;
				parameterPointer++;
				break;
			case 'b':
				isPlayer1sTurn = false;
				parameterPointer++;
				break;
				}
			}

			//checks for castling
			if (parameterPointer >= 1 && parameterPointer < 5) {
				if (i == 'K') {
					if (parameterPointer == 2)
						parameterPointer++;
					board[7][4] |= MOVEMASK;
					board[7][7] |= MOVEMASK;
				}
				else if (i == 'k') {
					if (parameterPointer == 3)
						parameterPointer++;
					board[0][4] |= MOVEMASK;
					board[0][7] |= MOVEMASK;
				}
				else if (i == 'Q') {
					if (parameterPointer == 2)
						parameterPointer++;
					board[7][4] |= MOVEMASK;
					board[7][0] |= MOVEMASK;
				}
				else if (i == 'q') {
					if (parameterPointer == 3)
						parameterPointer++;
					board[0][4] |= MOVEMASK;
					board[0][0] |= MOVEMASK;
				}
				else {
					parameterPointer++;
				}
			}

		}
		if (pointerX > 7 && pointerY >= 7) pointerY++;
	}
}

ChessGame::ChessGame(Player* playerW, Player* playerB, int* mouseX, int* mouseY, bool* mouseCl) {
	//init renderer
	renderer.init();

	//load from fen of standard starting positions
	loadBoardFromFen(DEFAULT);

	player1 = playerW;
	player2 = playerB;

	mousePosX = mouseX;
	mousePosY = mouseY;
	mouseClick = mouseCl;

	player1->chessGamePtr = this;
	player2->chessGamePtr = this;
}

int ChessGame::Play() {
	renderer.renderScreen(board);

	if (APIhandler.waitForChallenge()) {
		player1->colour = 1;
		player2->colour = 0;
		isPlayer1sTurn = false;
	}
	//main game loop
	while (isRunning) {
		handleEvents();

		current = std::chrono::steady_clock::now();

		SDL_Delay(1);

		if (std::chrono::duration_cast<std::chrono::milliseconds> (current - lastCheckFrame).count() > 1000 / FRAMECAP) {
			renderer.renderScreen(board);
			lastCheckFrame = current;
		}

		if (renderer.state == 0) {
			if (std::chrono::duration_cast<std::chrono::milliseconds> (current - lastCheckSecond).count() > 1000) {
				if (isPlayer1sTurn) {
					renderer.wTimer--;
				}
				else {
					renderer.bTimer--;
				}
				lastCheckSecond = current;
			}
		}

		if (renderer.wTimer <= 0)
			renderer.state = 2;
		else if (renderer.bTimer <= 0)
			renderer.state = 1;

		if (renderer.state == 0)
			update();

		else if (std::chrono::duration_cast<std::chrono::milliseconds> (current - lastCheckSecond).count() > 10000)
			break;
	}

	return 0;
}