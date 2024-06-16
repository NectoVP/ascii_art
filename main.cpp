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

Point CalculatePerspective(const Point& old) {
    Point center(0.5, -0.5, 0);

    float fov = 120;
    double precomp_tan = std::tan(fov * PI / 180 / 2);
    
    //Perspective
    Point tempPoint(old.x / old.z / precomp_tan, old.y / old.z / precomp_tan, old.z);
    
    //Translation towards center
    tempPoint.x += center.x;
    tempPoint.y += center.y;

    //Final size corresponding to window size
    tempPoint.x *= nScreenWidth;
    tempPoint.y *= nScreenHeight;

    return tempPoint;
}

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

// pass true to check if line is above, false to check below
bool CheckUnderOrAboveLine(const Point& first, const Point& second, double x, double y, bool flag) {
    double koef_k = (second.y - first.y) / (second.x - first.x);
    double koef_b = second.y - koef_k * second.x;

    bool check = koef_k * x + koef_b >= y;

    if(flag)
        return check;

    return !check;
}

bool CheckBoundaries(double x, double y, double angle) {
    Point pointA(-0.5, 0, 0.7);
    Point pointB(0.5, 0, 0.7);
    Point pointC(0, 0.5, 0.7);

    Point pA = RotatePoint(pointA, 0, angle, 0);
    Point pB = RotatePoint(pointB, 0, angle, 0);

    //Point pC = RotatePoint(pointC, angle, 0, 0);
    Point pC = Point(pointC);

    pA = CalculatePerspective(pA);
    pB = CalculatePerspective(pB);
    pC = CalculatePerspective(pC);


    return CheckUnderOrAboveLine(pA, pB, x, y, false) && CheckUnderOrAboveLine(pA, pC, x, y, true) && CheckUnderOrAboveLine(pC, pB, x, y, true);
}


int main() {
    Renderer renderer;

    double k = 0;

    while(true) {
        renderer.Fill();

        for(int i = 0; i < nScreenHeight; ++i) {
            for(int j = 0; j < nScreenWidth; ++j) {
                if(CheckBoundaries(j, -i, k))
                    renderer.screen[i * nScreenWidth + j] = '.';
            }
        }

        k += 0.1;
        if(k >= 360)
            k = 0;

        renderer.Render();
    }

    return 0;
}