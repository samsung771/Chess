#include "SDL.h"
#include <windows.h>
#include <string>
#include <vector>


#define WIDTH  8
#define HEIGHT 8

#define COLOURMASK 0b10000000
#define MOVEMASK   0b01000000
#define PIECEMASK  0b00111111

#define PAWN	   0b00000001
#define KNIGHT	   0b00000010
#define BISHOP	   0b00000100
#define ROOK	   0b00001000
#define QUEEN	   0b00010000
#define KING	   0b00100000

#define windowWIDTH  800
#define windowHEIGHT 600



class ChessGame {
private:
	//vector movements for each piece type
	int rookMoves[4][2] = {
		{1,0},
		{-1,0},
		{0,-1},
		{0,1}
	}; int bishopMoves[4][2] = {
		{1,1},
		{-1,1},
		{1,-1},
		{-1,-1}
	}; int knightMoves[8][2] = {
		{2,1},
		{2,-1},
		{1,2},
		{-1,2},
		{-2,1},
		{-2,-1},
		{1,-2},
		{-1,-2}
	}; int allMoves[8][2] = {
		{1,1},
		{-1,1},
		{1,-1},
		{-1,-1},
		{1,0},
		{-1,0},
		{0,-1},
		{0,1}
	};
	
	bool isRunning = true;

	//rendering variables
	SDL_Texture* texture[12] = { NULL };
	const SDL_Colour background{ 83, 94, 93, 255 };
	const SDL_Colour darkSquare{ 49, 51, 51, 255 };
	const SDL_Colour lightSquare{ 225, 235, 234, 255 };
	const SDL_Colour colCheck{ 225, 0, 0, 255 };

	//rendering size variables
	bool resize = true;
	int squareSize = windowHEIGHT / HEIGHT;
	int startingPosx, startingPosy = 0;
	int w, h;
	const float pieceScale[6] = { 0.7,0.8,0.8,0.8,0.8,0.8 };
	const float pieceScalerW[6] = { 0.95,1,1,1,1,1 };

	//mouse variables
	int mousePosX, mousePosY;
	bool mouseClick = 0;

	//original pos of piece
	int pieceHeldX, pieceHeldY;
	//piece representation
	uint8_t pieceHeld = 0;

	//game variables
	bool bCheck = 0;
	bool wCheck = 0;
	uint8_t enPassent[16] = { 0 };

	//SDL variables
	SDL_Renderer* renderer;
	SDL_Window* window;

	//macro to get the piece from its binary representation
	#define getPiece(x,y)\
	int piece = log2(board[y][x] & PIECEMASK);\
	bool colour = (board[y][x] & COLOURMASK) >> 7;\
	bool hasMoved = !(board[y][x] & MOVEMASK) >> 6;

	//draws squares
	void drawSquare(SDL_Colour colour, int x, int y, int w, int h) {
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);

		SDL_Rect square{ x,y,w,h };
		SDL_RenderFillRect(renderer, &square);
	}

	//draws background to fit screen
	void drawBackground() {
		//update variables if resize event is called
		if (resize) {
			SDL_GetWindowSize(window, &w, &h);

			if (w > h) squareSize = h / HEIGHT;
			else squareSize = w / WIDTH;

			startingPosx = w / 2 - squareSize * 4;
			startingPosy = h / 2 - squareSize * 4;

			resize = false;
		}

		//draws board
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				if ((x + y) % 2 == 1) drawSquare(darkSquare,
					x * squareSize + startingPosx,
					y * squareSize + startingPosy,
					squareSize, squareSize);
				else drawSquare(lightSquare,
					x * squareSize + startingPosx,
					y * squareSize + startingPosy,
					squareSize, squareSize);
			}
		}
	}

	//load textures and initialise some variables
	void init() {
		SDL_Surface* temp = NULL;

		//files for bmp
		char files[12][12] = {
		   "pawn.bmp",
		   "knight.bmp",
		   "bishop.bmp",
		   "rook.bmp",
		   "queen.bmp",
		   "king.bmp",
		   "pawnb.bmp",
		   "knightb.bmp",
		   "bishopb.bmp",
		   "rookb.bmp",
		   "queenb.bmp",
		   "kingb.bmp"
		};

		//load textures
		for (int i = 0; i < 12; i++) {
			temp = SDL_LoadBMP(files[i]);

			texture[i] = SDL_CreateTextureFromSurface(renderer, temp);
		}

		//delete temporary files
		SDL_FreeSurface(temp);
	}

	//casts a line with a certain vector until it hits a piece
	std::vector<std::vector<int>> raycast(int x, int y, int dx, int dy, int len, bool isWhite, uint8_t board[8][8]) {
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

	//gets available moves for a piece
	std::vector<std::vector<int>> availableMoves(int x, int y, uint8_t pieceCheck, uint8_t board[8][8]) {
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

			if (colour)
				inCheck = wCheck;
			else
				inCheck = bCheck;


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

	//update board
	void update() {
		if (mouseClick) {
			//gets square that was clicked
			int squareX = floor((mousePosX - startingPosx) / squareSize);
			int squareY = floor((mousePosY - startingPosy) / squareSize);
			int validMove = 0;

			//checks square is within play space
			if (squareX >= 0 && squareX < WIDTH && squareY >= 0 && squareY < HEIGHT) {
				int colour = (board[squareY][squareX] & COLOURMASK) >> 7;
				//checks available moves
				std::vector<std::vector<int>> available = legalMoves(pieceHeldX, pieceHeldY, pieceHeld);

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

								//swap turn
								isWhiteTurn = !isWhiteTurn;

								//increase move
								if (!isWhiteTurn && move == 1)
									move++;

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

						//check if kings are in check
						std::vector<bool> temp = checkCheck(board);
						bCheck = temp[0];
						wCheck = temp[1];
					}
				}

				//if clicking on a piece of your colour
				else if (colour == !isWhiteTurn) {
					//pickup piece if you aren't holding one
					if (board[squareY][squareX] != 0 && pieceHeld == 0) {
						pieceHeld = board[squareY][squareX];
						board[squareY][squareX] = 0;
						pieceHeldX = squareX;
						pieceHeldY = squareY;
					}
					//replace held piece if you are holding a piece
					else if (board[squareY][squareX] != 0 && pieceHeld != 0) {
						board[pieceHeldY][pieceHeldX] = pieceHeld;
						pieceHeld = 0;
					}
				}

				//capture piece
				else if (board[squareY][squareX] != 0 && pieceHeld != 0) {
					//checks for available moves
					for (std::vector<int> i : available) {
						if (i[0] == squareX && i[1] == squareY) {
							//place piece
							board[squareY][squareX] = pieceHeld;
							pieceHeld = 0;

							//swap turns
							isWhiteTurn = !isWhiteTurn;

							//increase move if it is not the first white turn
							if (!isWhiteTurn && move == 1) {
								move++;
							}

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

					//checks if any king is in check
					std::vector<bool> temp = checkCheck(board);
					bCheck = temp[0];
					wCheck = temp[1];
				}
			}
		}

		//reset mouseClick
		mouseClick = false;
	}

	//clears frame and renders a new one
	void renderScreen() {
		//wipe screen to background colour
		SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
		SDL_RenderClear(renderer);

		//draw the board
		drawBackground();

		//render pieces
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				if ((board[y][x] & PIECEMASK) != 0) {
					getPiece(x, y);

					//make the king's square red if its in check
					if (piece == 5) {
						if ((!colour && bCheck) || (colour && wCheck))
							drawSquare(colCheck,
								x * squareSize + startingPosx,
								y * squareSize + startingPosy,
								squareSize, squareSize);

					}
					
					//render piece
					SDL_Rect pos;
					pos.x = startingPosx + x * squareSize + squareSize * (1 - pieceScale[piece] * pieceScalerW[0]) / 2;
					pos.y = startingPosy + y * squareSize + squareSize * (1 - pieceScale[piece] * pieceScalerW[0]) / 2;
					pos.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
					pos.h = squareSize * pieceScale[piece];

					SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &pos);
				}
			}
		}

		//render possible positions for the held piece
		if (pieceHeld != 0) {
			std::vector<std::vector<int>> available = legalMoves(pieceHeldX, pieceHeldY, pieceHeld);

			for (std::vector<int> i : available) {
				//draw green rectangle
				SDL_Rect rect;
				rect.x = startingPosx + i[0] * squareSize + squareSize / 2 - squareSize * 0.075;
				rect.y = startingPosy + i[1] * squareSize + squareSize / 2 - squareSize * 0.075;
				rect.w = squareSize * 0.15;
				rect.h = squareSize * 0.15;
				SDL_SetRenderDrawColor(renderer, 26, 145, 80, 255);
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		//render pieceHeld at the mouse
		if (pieceHeld != 0) {
			int piece = log2(pieceHeld & PIECEMASK);
			int colour = (pieceHeld & COLOURMASK) >> 7;

			//render piece
			SDL_Rect rect;
			rect.x = mousePosX - squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2 - 9;
			rect.y = mousePosY - squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2 - 9;
			rect.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
			rect.h = squareSize * pieceScale[piece];

			SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &rect);
		}

		//show render
		SDL_RenderPresent(renderer);
	}

	//handle SDL window events e.g. key presses
	void handleEvents() {
		SDL_Event event;

		//get events
		SDL_PollEvent(&event);

		switch (event.type)
		{
		//pressing the exit button
		case SDL_QUIT:
			isRunning = false;
			break;

		//window resize event
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				resize = true;
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
			SDL_GetMouseState(&mousePosX, &mousePosY);
			break;
		
		//get mouse click
		case SDL_MOUSEBUTTONDOWN:
			mouseClick = true;
			break;
		};
	}

public:
	//public game variables
	uint8_t board[8][8] = { {0} };
	bool isWhiteTurn = 1;
	int move = 0;

	//returns all legal moves for piece
	std::vector<std::vector<int>> legalMoves(int x, int y, uint8_t pieceCheck) {
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

	//gets all possible moves for a colour
	std::vector<std::vector<int>> getAllMoves(bool colourToCheck, uint8_t board[8][8]) {
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

	//returns if each king is in check
	std::vector<bool> checkCheck(uint8_t board[8][8]) {
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

	//load a board state from fen notation
	void loadBoardFromFen(std::string fen) {
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
			if (pointerY < 8)
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
				}
			//parameters
			else {
				//colour of the piece
				switch (i) {
					if (parameterPointer == 0) {
				case 'w':
					isWhiteTurn = true;
					parameterPointer++;
					break;
				case 'b':
					isWhiteTurn = false;
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

	//constructor
	ChessGame() {
		//init SDL and open window
		SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("Chess",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowWIDTH,
			windowHEIGHT,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

		renderer = SDL_CreateRenderer(window, -1, 0);

		//load textures
		init();

		//load from fen of standard starting positions
		loadBoardFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	}

	//plays main game loop
	int Play() {
		//main game loop
		while (isRunning) {
			handleEvents();
			renderScreen();
			update();
		}

		return 0;
	}
};

int main(int argc, char* argv[]) {
	ChessGame chess;

	chess.Play();

	return 0;
}