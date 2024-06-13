#include <iostream>
#include <vector>
#include <windows.h>
#include <cmath>

const double PI = 3.14159265358;

int nScreenWidth = 170;
int nScreenHeight = 40;

void SetWindow(int Width, int Height) 
{ 
    _COORD coord; 
    coord.X = Width; 
    coord.Y = Height; 

    _SMALL_RECT Rect; 
    Rect.Top = 0; 
    Rect.Left = 0;
    Rect.Bottom = Height - 1; 
    Rect.Right = Width - 1; 

    HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);      // Get Handle 
    SetConsoleScreenBufferSize(Handle, coord);            // Set Buffer Size 
    SetConsoleWindowInfo(Handle, TRUE, &Rect);            // Set Window Size 
} 

int main() {

    SetWindow(nScreenWidth, nScreenHeight);

    char* screen = new char[nScreenHeight * nScreenWidth];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    double k = 0;

    while(true) {

        for(int i = 0; i < nScreenHeight; ++i) {
            for(int j = 0; j < nScreenWidth; ++j) {
                screen[i * nScreenWidth + j] = '#';
            }
        }

        for(int i = 0; i < nScreenHeight; ++i) {
            for(int j = 0; j < nScreenWidth; ++j) {
                if(std::pow(i - 5 - std::sin(k) * nScreenHeight / 4, 2) / 28 * 33 + std::pow(j - k, 2) / 66 * 28 <= 50)             
                    screen[i * nScreenWidth + j] = '.';
            }
        }

        k += 0.01;
        if(k >= nScreenWidth)
            k = 0;

        screen[nScreenHeight * nScreenWidth - 1] = '\0';
        WriteConsoleOutputCharacterA(hConsole, screen, nScreenHeight * nScreenWidth, {0, 0}, &dwBytesWritten);
    }

    delete[] screen;

    return 0;
}