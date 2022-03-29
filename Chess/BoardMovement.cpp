#include "BoardMovement.h"

void BoardMoveController::genVectorBoard() {
    boardGrid.clear();
    //generate an array of nodes for the A* pathfinding
    for (int y = 0; y < 17; y++) {
        std::vector<node> temp;
        for (int x = 0; x < 15; x++) {
            node newNode;
            newNode.x = x;
            newNode.y = y;

            //if there is a piece on this square set it as an obstacle
            if ((y - 1) % 2 == 0 && x % 2 == 0) {
                if (board[x / 2][(y - 1) / 2] != 0) newNode.obstacle = true;
            }
            
            //if it is not a corner piece dont allow diagonals
            if ((x + (y % 2)) % 2 == 0) newNode.diagonals = false;

            temp.push_back(newNode);
        }
        boardGrid.push_back(temp);
    }
}

BoardMoveController::BoardMoveController() {
    serialControl.init("COM4");

    //generate node matrix for pathfinding
    genVectorBoard();

    deactivateMagnet();
    home();
}

void BoardMoveController::movePiece(int startingSqY, int startingSqX, int endingSqY, int endingSqX) {
    //deactivate magnet and move directly to the square the piece is on
    deactivateMagnet();
    moveToSquare(startingSqX, startingSqY);

    //turn on magnet and generate a path for the piece
    activateMagnet();
    std::vector<std::vector<int>> trail = Astar(boardGrid, 2 * startingSqX, 2 * startingSqY + 1, 2 * endingSqX, 2 * endingSqY + 1);

    int commandsInBuffer = 0;

    std::vector <std::vector<int>> movement;

    //find the direction of the first move vector
    int direction[2] = { 
        trail[trail.size() - 1][0] - (2 * startingSqX),
        trail[trail.size() - 1][1] - (2 * startingSqY + 1)};

    //combine vectors if their directions are the same e.g. (1,1)->(2,2)->(3,3) becomes (1,1)->(3,3)
    for (int i = trail.size() - 1; i >= 1; i--) {
        /*
        if (i == trail.size() - 1) movement.push_back({ trail[i][0], trail[i][1] }); //needs to add the first move

        //add the vector if it changes direction
        else if (trail[i + 1][0] - trail[i][0] != direction[0] || trail[i + 1][1] - trail[i][1] != direction[1]) {
            direction[0] = trail[i + 1][0] - trail[i][0];
            direction[1] = trail[i + 1][1] - trail[i][1];
            movement.push_back({ trail[i][0], trail[i][1] });
        }
        */
        if (trail[i - 1][0] - trail[i][0] != direction[0] || trail[i - 1][1] - trail[i][1] != direction[1]) {
            direction[0] = trail[i - 1][0] - trail[i][0];
            direction[1] = trail[i - 1][1] - trail[i][1];
            movement.push_back({ trail[i][0], trail[i][1] });
        }
    }
    movement.push_back({ trail[0][0], trail[0][1] });

    for (int i = 0; i < movement.size(); i++)
    {
        //generate board positions from the node matrix
        int x = round(movement[i][0] * 92.5 + 0xA4);
        int y = round(movement[i][1] * 95.5 + 0xF6);

        //seperate to bytes
        char xBytes[2];
        char yBytes[2];

        xBytes[0] = (x & 0x00FF);
        yBytes[0] = (y & 0x00FF);
        xBytes[1] = (x & 0xFF00) >> 8;
        yBytes[1] = (y & 0xFF00) >> 8;

        //set command
        char command[5] = { 0x01,xBytes[1],xBytes[0],yBytes[1],yBytes[0] };

        char buffer = 0;

        //if it has sent too many commands the board cant keep up so wait untill a byte has been sent back
        if (commandsInBuffer > 8) {
            bool commandDone = false;

            while (!commandDone) {
                serialControl.SerialRead(&buffer, 1);
                if (buffer == 'ï') commandDone = true;
            }
            commandsInBuffer--;
        }

        //send command
        serialControl.SerialWrite(command, 5);

        commandsInBuffer++;
    }

    deactivateMagnet();
}

void BoardMoveController::moveToSquare(int x, int y) {
    //calculate positions from the square numbers
    x = x * 185 + 0xA4;
    y = y * 191 + 0x155;

    //split it into each seperate byte
    char xBytes[2];
    char yBytes[2];

    xBytes[0] = (x & 0x00FF);
    yBytes[0] = (y & 0x00FF);
    xBytes[1] = (x & 0xFF00) >> 8;
    yBytes[1] = (y & 0xFF00) >> 8;

    //send command
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

void BoardMoveController::removePiece(int sqY, int sqX) {
    //deactivate magnet and move directly to the square the piece is on
    deactivateMagnet();
    moveToSquare(sqX, sqY);

    //moves piece off the board
    activateMagnet();

    std::vector<std::vector<int>> trail = Astar(boardGrid, 2 * sqX, 2 * sqY + 1, 7, 0);

    int commandsInBuffer = 0;

    std::vector <std::vector<int>> movement;

    //find the direction of the first move vector
    int direction[2] = {
        trail[trail.size() - 1][0] - (2 * sqX),
        trail[trail.size() - 1][1] - (2 * sqY + 1) };

    //combine vectors if their directions are the same e.g. (1,1)->(2,2)->(3,3) becomes (1,1)->(3,3)
    for (int i = trail.size() - 1; i >= 1; i--) {
        if (trail[i - 1][0] - trail[i][0] != direction[0] || trail[i - 1][1] - trail[i][1] != direction[1]) {
            direction[0] = trail[i - 1][0] - trail[i][0];
            direction[1] = trail[i - 1][1] - trail[i][1];
            movement.push_back({ trail[i][0], trail[i][1] });
        }
    }
    movement.push_back({ trail[0][0], trail[0][1] });

    //sends move to board
    for (int i = 0; i < movement.size(); i++)
    {
        //generate board positions from the node matrix
        int x = round(movement[i][0] * 92.5 + 0xA4);
        int y = round(movement[i][1] * 95.5 + 0xF6);

        //seperate to bytes
        char xBytes[2];
        char yBytes[2];

        xBytes[0] = (x & 0x00FF);
        yBytes[0] = (y & 0x00FF);
        xBytes[1] = (x & 0xFF00) >> 8;
        yBytes[1] = (y & 0xFF00) >> 8;

        //set command
        char command[5] = { 0x01,xBytes[1],xBytes[0],yBytes[1],yBytes[0] };

        char buffer = 0;

        //if it has sent too many commands the board cant keep up so wait until a byte has been sent back
        if (commandsInBuffer > 8) {
            bool commandDone = false;

            while (!commandDone) {
                serialControl.SerialRead(&buffer, 1);
                if (buffer == 'ï') commandDone = true;
            }
            commandsInBuffer--;
        }

        //send command
        serialControl.SerialWrite(command, 5);

        commandsInBuffer++;

        if (i == movement.size() - 1) {
            char command[5] = { 0x01,xBytes[1],xBytes[0],0x00,0x90 };
            serialControl.SerialWrite(command, 5);
        }
    }


    int x = round(pieceCap * 92.5 + 0xA4);
    int y = 0x0035;

    if (pieceCap % 2) y = 0x0090;
    char xBytes[2];
    char yBytes[2];

    xBytes[0] = (x & 0x00FF);
    yBytes[0] = (y & 0x00FF);
    xBytes[1] = (x & 0xFF00) >> 8;
    yBytes[1] = (y & 0xFF00) >> 8;

    //send command
    char command[5] = { 0x01,xBytes[1],xBytes[0],yBytes[1],yBytes[0] };

    pieceCap++;

    serialControl.SerialWrite(command, 5);
}

void BoardMoveController::capturePiece(int startingSqY, int startingSqX, int endingSqY, int endingSqX) {
    removePiece(endingSqY, endingSqX);
    movePiece(startingSqY, startingSqX, endingSqY, endingSqX);
    /*
    deactivateMagnet();
    moveToSquare(startingSqX, startingSqY);

    //turn on magnet and generate a path for the piece
    activateMagnet();
    trail = Astar(boardGrid, 2 * startingSqX, 2 * startingSqY + 1, 2 * endingSqX, 2 * endingSqY + 1);

    commandsInBuffer = 0;

    movement.clear();

    //find the direction of the first move vector
    direction[0] = trail[trail.size() - 1][0] - (2 * startingSqX);
    direction[1] = trail[trail.size() - 1][1] - (2 * startingSqY + 1);

    //combine vectors if their directions are the same e.g. (1,1)->(2,2)->(3,3) becomes (1,1)->(3,3)
    for (int i = trail.size() - 1; i >= 1; i--) {
        if (trail[i - 1][0] - trail[i][0] != direction[0] || trail[i - 1][1] - trail[i][1] != direction[1]) {
            direction[0] = trail[i - 1][0] - trail[i][0];
            direction[1] = trail[i - 1][1] - trail[i][1];
            movement.push_back({ trail[i][0], trail[i][1] });
        }
    }
    movement.push_back({ trail[0][0], trail[0][1] });

    for (int i = 0; i < movement.size(); i++)
    {
        //generate board positions from the node matrix
        int x = round(movement[i][0] * 92.5 + 0xA4);
        int y = round(movement[i][1] * 95.5 + 0xF6);

        //seperate to bytes
        char xBytes[2];
        char yBytes[2];

        xBytes[0] = (x & 0x00FF);
        yBytes[0] = (y & 0x00FF);
        xBytes[1] = (x & 0xFF00) >> 8;
        yBytes[1] = (y & 0xFF00) >> 8;

        //set command
        char command[5] = { 0x01,xBytes[1],xBytes[0],yBytes[1],yBytes[0] };

        char buffer = 0;

        //if it has sent too many commands the board cant keep up so wait until a byte has been sent back
        if (commandsInBuffer > 8) {
            bool commandDone = false;

            while (!commandDone) {
                serialControl.SerialRead(&buffer, 1);
                if (buffer == 'ï') commandDone = true;
            }
            commandsInBuffer--;
        }

        //send command
        serialControl.SerialWrite(command, 5);

        commandsInBuffer++;
    }

    deactivateMagnet();
    */
}

void BoardMoveController::epCapturePiece(int startingSqY, int startingSqX, int endingSqY, int endingSqX) {
    removePiece(startingSqY, endingSqX);
    movePiece(startingSqY, startingSqX, endingSqY, endingSqX);
}

void BoardMoveController::castle(int startingSqY, int startingSqX, int endingSqY, int endingSqX) {
    movePiece(startingSqY, startingSqX, endingSqY, endingSqX);
    if (endingSqX == 2)
        movePiece(startingSqY, 0, endingSqY, 3);
    else
        movePiece(startingSqY, 7, endingSqY, 5);
}