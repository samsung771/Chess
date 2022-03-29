#pragma once
#include <vector>
#include <iostream>
#include <fstream>

class APIhandler {
private:
	const char lettersLookup[8] = { 'a','b','c','d','e','f','g','h' };
public:
	int makeMove(int startingX, int startingY, int endingX, int endingY);

	bool waitForChallenge();

	bool seekGame();

	void resign();

	std::vector<int> getMove();
};