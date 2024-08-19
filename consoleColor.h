#ifndef CONSOLECOLOR_H
#define CONSOLECOLOR_H

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


#endif