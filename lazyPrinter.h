#ifndef LAZYPRINTER_H
#define LAZYPRINTER_H

#include <iostream>
#include <vector>
#include <windows.h>
#include "consoleColor.h"
using namespace std;

const char PIXEL_CHAR = 'X';
const int PIXEL_WIDTH = 2;
const int PIXEL_HEIGHT = 1;

class LazyPrinter {
private:
    const int WIDTH;
    const int HEIGHT;

    struct Data {
        char ch;
        int color;

        bool operator!=(const Data &other) const { return ch != other.ch || color != other.color; }
    };

    int curColor;
    int curX, curY;
    vector<vector<Data> > prevConsole, console;

    void lazyPrint(char ch) {
        if (curX >= 0 && curX < WIDTH && curY >= 0 && curY < HEIGHT) console[curX++][curY] = Data{ch, curColor};
        else cerr << "out of bounds at (" << curX << ", " << curY << ")\n";
    }

    int convertColorToInt(ConsoleColor color, ConsoleColor bgColor = ConsoleColor::ORIGINALBG) {
        int colorInt = static_cast<int>(color) & 0xf;
        int bgColorInt = static_cast<int>(bgColor) & 0xf;
        return (bgColorInt << 4) | colorInt;
    }

    void printOnConsole(short x, short y, int colorData, char ch) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x, y});
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorData);
        cout << ch;
    }

public:
    LazyPrinter(int width, int height) : \
        WIDTH(width * PIXEL_WIDTH), HEIGHT(height * PIXEL_HEIGHT), \
        curColor(convertColorToInt(ConsoleColor::WHITE)), curX(0), curY(0), \
        prevConsole(WIDTH, vector<Data>(HEIGHT, {PIXEL_CHAR, curColor})), \
        console(WIDTH, vector<Data>(HEIGHT, {PIXEL_CHAR, curColor})) {}

    void setColor(ConsoleColor color, ConsoleColor bgColor = ConsoleColor::ORIGINALBG) {
        curColor = convertColorToInt(color, bgColor);
    }

    void setXY(int gx, int gy) {
        curX = gx;
        curY = gy;
    }

    void rect(int x, int y, int w, int h) { // 좌상단 꼭짓점 좌표 (x, y)부터 가로 w, 세로 h개의 픽셀 표시
        w *= PIXEL_WIDTH;
        h *= PIXEL_HEIGHT;

        for (int i = 0; i < h; i++) {
            setXY(x * PIXEL_WIDTH, y * PIXEL_HEIGHT + i);
            for (int _ = w; _--;) lazyPrint(PIXEL_CHAR);
        }
    }

    void rectBySubPixel(int x, int y, int w, int h) {
        for (int i = 0; i < h; i++) {
            setXY(x, y + i);
            for (int _ = w; _--;) lazyPrint(PIXEL_CHAR);
        }
    }

    void centerAlignedText(string st) {
        curX -= st.size() / 2;
        for (auto ch : st) lazyPrint(ch);
    }

    void render() {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                if (prevConsole[x][y] != console[x][y]) {
                    printOnConsole(x, y, console[x][y].color, console[x][y].ch);
                    prevConsole[x][y] = console[x][y];
                }
            }
        }
    }


};

#endif