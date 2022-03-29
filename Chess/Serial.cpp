#include "Serial.h"

bool SerialController::init(const LPCSTR port ,int function) {
    handle = CreateFileA(
        port,
        function,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    //returns false if it doesn't connect
    if (handle == INVALID_HANDLE_VALUE) {
        active = false;
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::cout << "Could not connect to " << port << std::endl;
        }
        return 0;
    }
    else {
        std::cout << "Connected to " << port << std::endl;

        //set up serial settings
        DCB dcbSerialParameters = { 0 };

        dcbSerialParameters.BaudRate = CBR_9600;
        dcbSerialParameters.ByteSize = 8;
        dcbSerialParameters.StopBits = ONESTOPBIT;
        dcbSerialParameters.Parity = NOPARITY;
        dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

        if (!SetCommState(handle, &dcbSerialParameters))
        {
            std::cout << "Error could not set serial port parameters\n";
            active = false;
            return 0;
        }
        else std::cout << "Succesfully set serial parameters\n";

        //set timeouts
        COMMTIMEOUTS timeout = { 0 };
        timeout.ReadIntervalTimeout = 50;
        timeout.ReadTotalTimeoutConstant = 50;
        timeout.ReadTotalTimeoutMultiplier = 50;
        timeout.WriteTotalTimeoutConstant = 50;
        timeout.WriteTotalTimeoutMultiplier = 10;

        SetCommTimeouts(handle, &timeout);

        Sleep(200);

        active = true;
        return 1;
    }
}

int SerialController::SerialRead(const char* buffer, unsigned int buf_size)
{
    DWORD bytesRead;

    ClearCommError(handle, &errors, &status);

    int bytesToRead = 0;

    //if there is data in the recieve buffer
    if (status.cbInQue > 0) {
        //only read as much data that has been recieved
        if (status.cbInQue < buf_size)  bytesToRead = status.cbInQue;
        else bytesToRead = buf_size;
    }

    //try to read buffer, if not print error msg
    if (!ReadFile(handle, (void*)buffer, bytesToRead, &bytesRead, 0)) {
        std::cout << "Read failed";
        if (!active) std::cout << " not connected";
        std::cout << std::endl;
        return 0;
    }

    return bytesRead;
}

bool SerialController::SerialWrite(const char* buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    Sleep(700);
    
    //print command that is sent
    switch ((int)buffer[0]) {
    case 1:
        std::cout << "Move to:  " << ((int)buffer[1] * 256 + (int)buffer[2]) << "   " << ((int)buffer[3] * 256 + (int)buffer[4]) << '\n';
        break;
    case 3:
        std::cout << "Home\n";
        break;
    case 5:
        if (buffer[4])
            std::cout << "Magnet on\n";
        else
            std::cout << "Magnet off\n";
        break;
    }

    //try to write to buffer, if not print error msg
    if (!WriteFile(handle, (void*)buffer, buf_size, &bytesSend, 0)) {
        std::cout << "Write failed";
        if (!active) std::cout << " not connected";
        std::cout << std::endl;
        return false;
    }

    return true;
}