#pragma once
#pragma once
#include <vector>
#include "Serial.h"
#include "Pathfinding.h"


class BoardMagnetController {
private:
    SerialController serialControl;
public:
    char board[8][8] = { 0 };

    char update();

    BoardMagnetController();
};