#include "BoardMagnet.h"

BoardMagnetController::BoardMagnetController() {
	serialControl.init("COM4");
}

char BoardMagnetController::update() {
	char buffer[1] = {0};
	serialControl.SerialRead(buffer,1);
	return buffer[0];
}