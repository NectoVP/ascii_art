#include <iostream>
#include <vector>
#include <windows.h>
#include <cmath>

const double PI = 3.14159265358;

int nScreenWidth = 238;
int nScreenHeight = 40;

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

Point CalculatePerspective(const Point& old, double k) {
    float fov = 120;
    double precomp_tan = std::tan(fov * PI / 180 / 2);
    
    Point center(0.25, 0.1, -10);

    double offset = 0.8;

    Point rotated = RotatePoint(old, 0, 0, k);

    //Perspective
    Point tempPoint(rotated.x / (rotated.z + offset) / precomp_tan, rotated.y / (rotated.z + offset) / precomp_tan, rotated.z);

    tempPoint.x += center.x;
    tempPoint.y += center.y;

    if(tempPoint.x > 1)
        tempPoint.x = 1;
        
    if(tempPoint.y > 1)
        tempPoint.y = 1;

    //Final size corresponding to window size
    tempPoint.x *= nScreenWidth;
    tempPoint.y *= nScreenHeight;

    return tempPoint;
}

bool CheckTorus(double x, double y, double z) {
    double small_rad = 2;
    double big_rad = 4;
    return std::pow(((x-7)*(x-7) + (y-7)*(y-7) + (z-3)*(z-3) + big_rad*big_rad - small_rad*small_rad), 2) <= 4 * big_rad * big_rad * ((x-7)*(x-7) + (y-7)*(y-7));
}

int main() {
    Renderer renderer;

    double k = 0;

    while(true) {
        renderer.Fill();

        for(double x = 1; x <= 13; x += 0.1) {
            for(double y = 1; y <= 13; y += 0.1) {
                for(double z = 1; z <= 5; z += 0.1) {
                    if(CheckTorus(x, y, z)) {
                        Point temp = CalculatePerspective(Point(x / 15, y / 15, z / 15), k);
                        int j = std::round(temp.x) - 1;
                        int i = std::round(temp.y) - 1;
                        renderer.screen[i * nScreenWidth + j] = '.';
                    }
                }
            }    
        }

        k += 0.5;
        if(k >= 360)
            k = 0;

        renderer.Render();
    }

    return 0;
}