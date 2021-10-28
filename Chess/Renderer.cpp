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

		if (w > h - (squareSize*2 + 40)) squareSize = (h - (squareSize*2 + 40)) / HEIGHT;
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

void Renderer::renderButton(SDL_Colour col, int piece, int colour, int x, int y, int w, int h ) {
	drawSquare(col, x, y, w, h);

	//render piece
	SDL_Rect pos;
	pos.x = x + squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2;
	pos.y = y + squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2;
	pos.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
	pos.h = squareSize * pieceScale[piece];

	SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &pos);
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
				pos.x = startingPosx + x * squareSize + squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2;
				pos.y = startingPosy + y * squareSize + squareSize * (1 - pieceScale[piece] * pieceScalerW[piece]) / 2;
				pos.w = squareSize * pieceScale[piece] * pieceScalerW[piece];
				pos.h = squareSize * pieceScale[piece];
				

				SDL_RenderCopy(renderer, texture[piece + colour * 6], NULL, &pos);
			}
		}
	}

	
	if (bPromotionsVisible) {
		renderButton(lightSquare, 4, 1, 
			startingPosx + squareSize * 7, 
			startingPosy + squareSize * 8 + squareSize * 0.2,
			squareSize, squareSize);
		renderButton(darkSquare, 3, 1,
			startingPosx + squareSize * 6,
			startingPosy + squareSize * 8 + squareSize * 0.2,
			squareSize, squareSize);
		renderButton(lightSquare, 2, 1,
			startingPosx + squareSize * 5,
			startingPosy + squareSize * 8 + squareSize * 0.2,
			squareSize, squareSize);
		renderButton(darkSquare, 1, 1,
			startingPosx + squareSize * 4,
			startingPosy + squareSize * 8 + squareSize * 0.2,
			squareSize, squareSize);
	}
	
	if (wPromotionsVisible) {
		renderButton(darkSquare, 4, 0,
			startingPosx + squareSize * 7,
			startingPosy - squareSize - squareSize * 0.2,
			squareSize, squareSize);
		renderButton(lightSquare, 3, 0,
			startingPosx + squareSize * 6,
			startingPosy - squareSize - squareSize * 0.2,
			squareSize, squareSize);
		renderButton(darkSquare, 2, 0,
			startingPosx + squareSize * 5,
			startingPosy - squareSize - squareSize * 0.2,
			squareSize, squareSize);
		renderButton(lightSquare, 1, 0,
			startingPosx + squareSize * 4,
			startingPosy - squareSize - squareSize*0.2,
			squareSize, squareSize);
	}

	renderTimer(0);
	renderTimer(1);

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

void Renderer::drawMatrix(bool matrix[9][6], int xStart, int yStart, int w, int h, SDL_Colour col) {
	int pixelSizeX = floor(w / 6);
	int pixelSizeY = floor(h / 9);

	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 6; x++) {
			if (matrix[y][x]) {
				drawSquare(col,
					xStart + x * pixelSizeX,
					yStart + y * pixelSizeY,
					pixelSizeX, pixelSizeY);
			}
		}
	}
}

void Renderer::drawNumber(int number, int xStart, int yStart, int w, int h, SDL_Colour col) {
	//make sure num is 1 digit
	int num = number % 10;
	switch (num) {
	case 0:
		drawMatrix(zero, xStart, yStart, w, h, col);
		break;
	case 1:
		drawMatrix(one, xStart, yStart, w, h, col);
		break;
	case 2:
		drawMatrix(two, xStart, yStart, w, h, col);
		break;
	case 3:
		drawMatrix(three, xStart, yStart, w, h, col);
		break;
	case 4:
		drawMatrix(four, xStart, yStart, w, h, col);
		break;
	case 5:
		drawMatrix(five, xStart, yStart, w, h, col);
		break;
	case 6:
		drawMatrix(six, xStart, yStart, w, h, col);
		break;
	case 7:
		drawMatrix(seven, xStart, yStart, w, h, col);
		break;
	case 8:
		drawMatrix(eight, xStart, yStart, w, h, col);
		break;
	case 9:
		drawMatrix(nine, xStart, yStart, w, h, col);
		break;
	default:
		break;
	}
}

void Renderer::renderTimer(bool colour) {
	int timer = 0;
	int y = 0;
	if (colour) {
		timer = bTimer;
		y = startingPosy - squareSize - squareSize * 0.1;
	}
	else {
		timer = wTimer;
		y = startingPosy + squareSize * 8 + squareSize * 0.3;
	}

	int numy = y + squareSize * 0.175;

	drawSquare(darkSquare, startingPosx, y, squareSize * 2.2, squareSize * 0.8);
	
	int display[4] = { 0,0,0,0 };

	int mins = floor(timer / 60);
	int secs = timer % 60;

	if (mins >= 10) {
		display[0] = floor(mins / 10);
		display[1] = mins % 10;
	}
	else {
		display[1] = mins;
	}

	if (secs >= 10) {
		display[2] = floor(secs / 10);
		display[3] = secs % 10;
	}
	else {
		display[3] = secs;
	}

	int space = 0;

	for (int i = 0; i < 4; i++) {
		if (i % 2 == 0)
			space = squareSize * 0.25;
		else
			space = squareSize * 0.1;
		drawNumber(display[i], startingPosx + i * (squareSize * 0.5) + space, numy, squareSize * 0.3, squareSize * 0.5, lightSquare);
	}

	drawMatrix(colon, startingPosx + squareSize*0.95, numy, squareSize * 0.3, squareSize * 0.5, lightSquare);//startingPosx + 2.5 * (squareSize * 0.4 + squareSize * 0.1) + space, numy, squareSize * 0.3, squareSize * 0.5, lightSquare);
}