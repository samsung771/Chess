#include "BoardMagnet.h"

BoardMagnetController::BoardMagnetController() {
	serialControl.init("COM3");
}

char BoardMagnetController::update() {
	//read the buffer
	char buffer[1] = {0};
	serialControl.SerialRead(buffer,1);
	return buffer[0];
}