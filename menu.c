#include <windows.h>
#include <stdio.h>

#include "menu.h"

void BufferMessage(const char* message, int milliseconds) {
    printf("%s\n", message);
    Sleep(milliseconds);
}

void clearScreen(){ //change for embedded
    system("cls");
}