#pragma once
#include <windows.h>
#include <iostream>


class SerialController {
public:

    HANDLE handle;
    COMSTAT status;
    DWORD errors;

    bool active = true;

    //sets up serial, requires the port and a pointer to the handle
    bool init(const LPCSTR port, int function = GENERIC_READ | GENERIC_WRITE);

    int SerialRead(const char* buffer, unsigned int buf_size);

    bool SerialWrite(const char* buffer, unsigned int buf_size);
};
