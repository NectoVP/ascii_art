#include <iostream>
#include <windows.h>
#include <cmath>
#include<unistd.h>

const double PI = 3.14159265358;

const int nScreenWidth = 60;
const int nScreenHeight = 30;

std::string light_ascii = ".,-~:;!*#$@";

struct Point {
    Point() {
        x = 0;
        y = 0;
        z = 0;
    }

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y * 33 / 28;
        this->z = z;
    }

    Point(const Point& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    const Point& operator=(Point other) {
        x = other.x;
        y = other.y;
        z = other.z;    
        
        return *this;
    }

    double x;
    double y;
    double z;
};

class Renderer {
public:
    Renderer() {
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
        memset(screen, 32, nScreenHeight * nScreenWidth);
    }

private:


public:
    char* screen;

private:
    HANDLE hConsole;
    DWORD dwBytesWritten;
};

Point RotatePoint(const Point& point, double x, double y, double z) {
    x = x * PI / 180; // phi
    y = y * PI / 180; // psi
    z = z * PI / 180; // teta

    Point rotated;

    using namespace std;
    rotated.x = cos(z) * cos(y) * point.x + ( -cos(x) * sin(y) + sin(x) * sin (z) * cos(y)) * point.y + (sin(x) * sin(y) + cos(x) * sin(z) * cos(y)) * point.z;
    rotated.y = cos(z) * sin(y) * point.x + (cos(x) * cos(y) + sin(x) * sin (z) * sin(y)) * point.y + ( -sin(x) * cos(y) + cos(x) * sin(z) * sin(y)) * point.z;
    rotated.z = -sin(z) * point.x + sin(x) * cos(z) * point.y + cos(x) * cos(z) * point.z;

    return rotated;
}

Point CalculatePerspective(const Point& old, double k, double n) {
    float fov = 90;
    double precomp_tan = std::tan(fov * PI / 180 / 2);
    
    Point center(0.7, 0.3, -5);

    double offset = 0.8;

    Point rotated = RotatePoint(old, 0, n, k);

    //Magic numbers
    rotated.x += 3;
    rotated.z += 4;

    Point light(1.0, 5.0, 3.0);

    double light_level = rotated.x * light.x + rotated.y * light.y + rotated.z * light.z;

    //Perspective
    Point tempPoint(rotated.x / (rotated.z + offset) / precomp_tan, rotated.y / (rotated.z + offset) / precomp_tan, (rotated.z + offset));

    //Centering
    tempPoint.y += center.y;
    tempPoint.x += center.x;

    //Final size corresponding to window size
    tempPoint.x *= nScreenWidth;
    tempPoint.y *= nScreenHeight;
    tempPoint.z = light_level;

    return tempPoint;
}

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

int main() {
    SetWindow(nScreenWidth, nScreenHeight);
    Renderer renderer;
    double n = 0;

    while(true) {
        renderer.Fill();

        for(double x = -5; x <= 5; x += 0.1) {
            for(double y = -5; y <= 5; y += 0.1) {
                if(x  * x  + y  * y <= 25) {
                    for(int k = 0; k < 360; k+=7) {
                        Point temp = CalculatePerspective(Point(x / 15, y / 15, 1), k, n);
                        int j = std::round(temp.x);
                        int i = std::round(temp.y);
                        renderer.screen[i * nScreenWidth + j] = light_ascii[(int)temp.z % light_ascii.length()];
                    }
                }
            }
        }

        if(n >= 360)
            n = 0;
        n+=2;

        renderer.Render();
    }

    return 0;
}