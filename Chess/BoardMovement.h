#pragma once
#include <vector>
#include "Serial.h"
#include "Pathfinding.h"


class BoardMoveController {
private:
    SerialController serialControl;
public:

    std::vector<std::vector<node>> boardGrid;

    char board[8][8] = { 0 };

    void home();

    void activateMagnet();

    void deactivateMagnet();

    void moveToSquare(int x, int y);

    void movePiece(int startingSqX, int startingSqY, int endingSqX, int endingSqY);

    BoardMoveController();
};