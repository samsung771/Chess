#pragma once
#pragma once
#include <vector>
#include "Serial.h"


class BoardMagnetController {
private:
    SerialController serialControl;
public:
    char board[8][8] = { 0 };

    char update();

    BoardMagnetController();
};