#include "BoardMovement.h"

BoardMoveController::BoardMoveController() {
    serialControl.init("COM3");

    for (int y = 0; y < 17; y++) {
        std::vector<node> temp;
        for (int x = 0; x < 15; x++) {
            node newNode;
            newNode.x = x;
            newNode.y = y;

            if ((y - 1) % 2 == 0 && x % 2 == 0) {
                if (board[(y - 1) / 2][x / 2] != 0) newNode.obstacle = true;
            }

            if ((x + (y % 2)) % 2 == 0) newNode.diagonals = false;

            temp.push_back(newNode);
        }
        boardGrid.push_back(temp);
    }

    deactivateMagnet();
    home();
    movePiece(0, 0, 7, 7);
}

void BoardMoveController::movePiece(int startingSqX, int startingSqY, int endingSqX, int endingSqY) {
    deactivateMagnet();
    moveToSquare(startingSqX, startingSqY);
    activateMagnet();
    std::vector<std::vector<int>> trail = Astar(boardGrid, 2 * startingSqX, 2 * startingSqY + 1, 2 * endingSqX, 2 * endingSqY + 1);

    int commandsInBuffer = 0;

    std::vector <std::vector<int>> movement;

    int direction[2] = { 0,0 };

    //simplifies trail into just the turns needed
    for (int i = trail.size() - 1; i >= 0; i--) {

        if (i == 0) movement.push_back({ trail[i][0], trail[i][1] });

        else if (trail[i - 1][0] - trail[i][0] != direction[0] || trail[i - 1][1] - trail[i][1] != direction[1]) {
            direction[0] = trail[i - 1][0] - trail[i][0];
            direction[1] = trail[i - 1][1] - trail[i][1];
            movement.push_back({ trail[i][0], trail[i][1] });
        }
    }

    for (int i = 0; i < movement.size(); i++)
    {
        int x = round(movement[i][0] * 92.5 + 0xA4);
        int y = round(movement[i][1] * 95.5 + 0xF6);

        char xBytes[2];
        char yBytes[2];

        xBytes[0] = (x & 0x00FF);
        yBytes[0] = (y & 0x00FF);
        xBytes[1] = (x & 0xFF00) >> 8;
        yBytes[1] = (y & 0xFF00) >> 8;

        char command[5] = { 0x01,xBytes[1],xBytes[0],yBytes[1],yBytes[0] };

        char buffer = 0;


        if (commandsInBuffer > 8) {

            bool commandDone = false;

            while (!commandDone) {
                Sleep(300);
                serialControl.SerialRead(&buffer, 1);
                if (buffer == 'ï') commandDone = true;
            }
            commandsInBuffer--;
        }

        serialControl.SerialWrite(command, 5);

        commandsInBuffer++;
    }

    deactivateMagnet();
}

void BoardMoveController::moveToSquare(int x, int y) {
    x = x * 185 + 0xA4;
    y = y * 191 + 0x155;

    char xBytes[2];
    char yBytes[2];

    xBytes[0] = (x & 0x00FF);
    yBytes[0] = (y & 0x00FF);
    xBytes[1] = (x & 0xFF00) >> 8;
    yBytes[1] = (y & 0xFF00) >> 8;

    char command[5] = { 0x01,xBytes[1],xBytes[0],yBytes[1],yBytes[0] };

    serialControl.SerialWrite(command, 5);
}

void BoardMoveController::deactivateMagnet() {
    char command[5] = { 0x05, 0x00, 0x00, 0x00, 0x00 };

    serialControl.SerialWrite(command, 5);
}

void BoardMoveController::activateMagnet() {
    char command[5] = { 0x05, 0x00, 0x00, 0x00, 0x01 };

    serialControl.SerialWrite(command, 5);
}

void BoardMoveController::home() {
    char command[5] = { 0x03, 0x00, 0x00, 0x00, 0x00 };

    serialControl.SerialWrite(command, 5);
}