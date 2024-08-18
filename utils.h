#include <iostream>
#include <windows.h>
#include <random>
using namespace std;

// 그림 그리는 기본 단위로 'X' 출력해서 사용
// 'X'는 세로길이가 가로보다 훨씬 길쭉해서 조정 필요
const char PIXEL_CHAR = 'X';
const int PIXEL_WIDTH = 2;
const int PIXEL_HEIGHT = 1;

// https://learn.microsoft.com/ko-kr/dotnet/api/system.consolecolor?view=net-8.0
enum class ConsoleColor {
    BLACK,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    DARKYELLOW,
    GRAY,
    DARKGRAY,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    WHITE,
    ORIGINALBG = 0
};

void setColor(ConsoleColor color, ConsoleColor bgColor = ConsoleColor::ORIGINALBG) { // bg = background
    int colorInt = static_cast<int>(color) & 0xf;
    int bgColorInt = static_cast<int>(bgColor) & 0xf;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgColorInt << 4) | colorInt);
}

void gotoxy(short gx, short gy) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {gx, gy});
}

void rect(int x, int y, int w, int h) { // 좌상단 꼭짓점 좌표 (x, y)부터 가로 w, 세로 h개의 픽셀 표시
    w *= PIXEL_WIDTH;
    h *= PIXEL_HEIGHT;

    for (int i = 0; i < h; i++) {
        gotoxy(x * PIXEL_WIDTH, y * PIXEL_HEIGHT + i);
        for (int _ = w; _--;) cout << PIXEL_CHAR;
    }
}