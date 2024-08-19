#ifndef CONSOLECOLOR_H
#define CONSOLECOLOR_H

#include <vector>
using namespace std;

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
    ORIGINALBG = BLACK,
    SHADOW = DARKGRAY,
    ORIGINAL_FONT = WHITE,
    BORDER_DEFAULT = DARKGRAY
};

const vector<ConsoleColor> rainbowColors = {
    ConsoleColor::RED,
    ConsoleColor::YELLOW,
    ConsoleColor::GREEN,
    ConsoleColor::CYAN,
    ConsoleColor::BLUE,
    ConsoleColor::MAGENTA,
    ConsoleColor::WHITE,
    ConsoleColor::DARKRED,
    ConsoleColor::DARKYELLOW,
    ConsoleColor::DARKGREEN,
    ConsoleColor::DARKCYAN,
    ConsoleColor::DARKBLUE,
    ConsoleColor::DARKMAGENTA
};

ConsoleColor getRainbowColor(int num) {
    int mod = rainbowColors.size();
    return rainbowColors[(num % mod + mod) % mod]; // 음수일 경우도 포함
}


#endif