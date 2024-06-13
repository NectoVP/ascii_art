#include <iostream>
#include <vector>
#include <windows.h>
#include <cmath>

const double PI = 3.14159265358;

int nScreenWidth = 170;
int nScreenHeight = 40;

struct Point {
    Point() {
        x = 0;
        y = 0;
    }

    Point(double x, double y) {
        this->x = x;
        this->y = y;
    }

    double x;
    double y;
};

class Renderer {
public:
    Renderer() {
        SetWindow(nScreenWidth, nScreenHeight);

        screen = new char[nScreenHeight * nScreenWidth];
        hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hConsole);
        dwBytesWritten = 0;
    }

    void Render() {
        screen[nScreenHeight * nScreenWidth - 1] = '\0';
        WriteConsoleOutputCharacterA(hConsole, screen, nScreenHeight * nScreenWidth, {0, 0}, &dwBytesWritten);
    }

    ~Renderer() {
        delete[] screen;
    }

    void Fill() {
        for(int i = 0; i < nScreenHeight; ++i) {
            for(int j = 0; j < nScreenWidth; ++j) {
                screen[i * nScreenWidth + j] = '#';
            }
        }
    }

private:
    void SetWindow(int Width, int Height) { 
        _COORD coord; 
        coord.X = Width; 
        coord.Y = Height; 

        _SMALL_RECT Rect; 
        Rect.Top = 0; 
        Rect.Left = 0;
        Rect.Bottom = Height - 1; 
        Rect.Right = Width - 1; 

        HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleScreenBufferSize(Handle, coord);
        SetConsoleWindowInfo(Handle, TRUE, &Rect);
    }

public:
    char* screen;

private:
    HANDLE hConsole;
    DWORD dwBytesWritten;
};

int main() {
    Renderer renderer;
    
    double k = 0;

    while(true) {

        renderer.Fill();

        for(int i = 0; i < nScreenHeight; ++i) {
            for(int j = 0; j < nScreenWidth; ++j) {
                if(std::pow(i - 5 - std::sin(k) * nScreenHeight / 4, 2) / 28 * 33 + std::pow(j - k, 2) / 66 * 28 <= 50)             
                    renderer.screen[i * nScreenWidth + j] = '.';
            }
        }

        k += 0.01;
        if(k >= nScreenWidth)
            k = 0;

        renderer.Render();
    }

    return 0;
}