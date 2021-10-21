#include "Renderer.h"

void Renderer::drawSquare(SDL_Colour colour, int x, int y, int w, int h) {
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);

	SDL_Rect square{ x,y,w,h };
	SDL_RenderFillRect(renderer, &square);
}

void Renderer::drawBackground() {
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

void Renderer::init() {
	//init SDL and open window
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Chess",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWIDTH,
		windowHEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);

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

void Renderer::renderScreen(uint8_t board[8][8]) {
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

		//render pieceHeld at the mouse
		int piece = log2(pieceHeld & PIECEMASK);
		int colour = (pieceHeld & COLOURMASK) >> 7;

		//render piece
		SDL_Rect rect;
		rect.x = *mousePosX - squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2 - 9;
		rect.y = *mousePosY - squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2 - 9;
		rect.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
		rect.h = squareSize * pieceScale[piece];

		SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &rect);
	}
	

	//show render
	SDL_RenderPresent(renderer);
}

int Renderer::getSquareSize() {
	return squareSize;
}

int Renderer::getStartPosX() {
	return startingPosx;
}

int Renderer::getStartPosY() {
	return startingPosy;
}