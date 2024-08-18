#include <conio.h> // _kbhit, _getch
#include "Game.h"

const char KEYBOARD_ARROW_UP = 72;
const char KEYBOARD_ARROW_LEFT = 75;
const char KEYBOARD_ARROW_RIGHT = 77;
const char KEYBOARD_ARROW_DOWN = 80;

// -------------------------------------------------------- 사용자 지정 ------------------------------------------------------------
const char KEY_PAUSE_GAME = 'p';
const char KEY_MOVE_LEFT = 'a';
const char KEY_MOVE_RIGHT = 'd';
const char KEY_ROTATE_LEFT = KEYBOARD_ARROW_LEFT;
const char KEY_ROTATE_RIGHT = KEYBOARD_ARROW_RIGHT;
const char KEY_DROP_SOFT = 's';
const char KEY_DROP_HARD = KEYBOARD_ARROW_UP;
const char KEY_HOlD = ' ';

// TODO 키 안겹치게 전부 static_assert 할지 고민

// --------------------------------------------------------------------------------------------------------------------------------

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
            else if (key == KEY_MOVE_LEFT) game.applyMove(Move::LEFT);
            else if (key == KEY_MOVE_RIGHT) game.applyMove(Move::RIGHT);
            else if (key == KEY_ROTATE_LEFT) game.applyMove(Move::ROTATE_CCW);
            else if (key == KEY_ROTATE_RIGHT) game.applyMove(Move::ROTATE_CW);
            else if (key == KEY_DROP_SOFT) game.applyMove(Move::DOWN);
            else if (key == KEY_DROP_HARD) game.applyMove(Move::HARDDROP);
            else if (key == KEY_HOlD) game.applyMove(Move::HOLD);
        }

        game.run();

        // TODO sleep 시간 정해야 됨
        // sleep(__);
    }

    return 0;
}