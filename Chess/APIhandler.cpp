#include "APIhandler.h"

int APIhandler::makeMove(int startingX, int startingY, int endingX, int endingY) {
	char command[50] = "python makeMove.py ";
	char ay[2] = { char((8 - startingY) + 48),0 };
	char ax[2] = { lettersLookup[startingX],0 };
	char by[2] = { char((8 - endingY) + 48),0 };
	char bx[2] = { lettersLookup[endingX],0 };
	strcat_s(command, 50, ax);
	strcat_s(command, 50, ay);
	strcat_s(command, 50, bx);
	strcat_s(command, 50, by);
	return system(command);
}

bool APIhandler::waitForChallenge() {
	system("python acceptChallenge.py");
	std::ifstream colourF;
	colourF.open("colour.txt");
	char colour[1];
	colourF.read(colour, 1);
	return colour[0] == '1';
}

bool APIhandler::seekGame() {
	system("python seekGame.py");
	std::ifstream colourF;
	colourF.open("colour.txt");
	char colour[1];
	colourF.read(colour, 1);
	return colour[0] == '1';
}

void APIhandler::resign() {
	system("python resign.py");
}

std::vector<int> APIhandler::getMove() {
	system("python getMove.py");

	std::ifstream statusF;
	statusF.open("status.txt");
	char status[1];
	statusF.read(status, 1);
	if (status[0] == '0')
		return { 10 };
	else if (status[0] == 'w')
		return { 11 };
	else if (status[0] == 'b')
		return { 12 };
	else {
		std::ifstream moveF;
		moveF.open("moves.txt");
		char move[4];
		moveF.read(move, 4);
		return { int(move[0]) - 97,
			7 - (int(move[1]) - 49),
			int(move[2]) - 97,
			7 - (int(move[3]) - 49) };
	}	
}