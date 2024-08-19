#include <windows.h>
#include <conio.h> // _kbhit, _getch
#include "game.h"
#include "userControlKey.h"

void consoleInit() {
    // console title
    SetConsoleTitle(TEXT("tetris"));

    // cursor
    CONSOLE_CURSOR_INFO cursorInfo = { 0, };
    cursorInfo.dwSize = 1; // 커서 굵기 (1 ~ 100)
    cursorInfo.bVisible = FALSE; // 커서 Visible 여부 (TRUE, FALSE)
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

int main() {
    consoleInit();

    Game<20, 10> game;
    game.start();

    while (1) {
        if (_kbhit()) {
            char key = _getch(); // input

            if (key == KEY_PAUSE_GAME) game.pause();
            else if (key == KEY_REPLAY_GAME) game.replay();
            else if (key == KEY_MOVE_LEFT) game.applyMove(Move::LEFT);
            else if (key == KEY_MOVE_RIGHT) game.applyMove(Move::RIGHT);
            else if (key == KEY_ROTATE_LEFT) game.applyMove(Move::ROTATE_CCW);
            else if (key == KEY_ROTATE_RIGHT) game.applyMove(Move::ROTATE_CW);
            else if (key == KEY_DROP_SOFT) game.applyMove(Move::DOWN);
            else if (key == KEY_DROP_HARD) game.applyMove(Move::HARDDROP);
            else if (key == KEY_HOlD) game.applyMove(Move::HOLD);
            else if (key == KEY_CHANGE_THEMA) game.changeThema();
        }

        game.run();
    }

    return 0;
}