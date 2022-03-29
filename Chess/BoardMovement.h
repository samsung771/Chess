#pragma once
#include <vector>
#include "Serial.h"
#include "Pathfinding.h"


class BoardMoveController {
private:
    SerialController serialControl;
    bool captureSquares[15] = { 0 };
    int pieceCap = 0;

    void removePiece(int sqY, int sqX);
public:

    std::vector<std::vector<node>> boardGrid;

    char board[8][8] = { 0 };

    void genVectorBoard();

    void home();

    void activateMagnet();

    void deactivateMagnet();

    void moveToSquare(int x, int y);

    void movePiece(int startingSqX, int startingSqY, int endingSqX, int endingSqY);

    void capturePiece(int startingSqY, int startingSqX, int endingSqY, int endingSqX);

    void castle(int startingSqY, int startingSqX, int endingSqY, int endingSqX);
        
    void epCapturePiece(int startingSqY, int startingSqX, int endingSqY, int endingSqX);

    BoardMoveController();
};