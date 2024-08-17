#include <bits/stdc++.h>
#include <windows.h>
#include <random>
using namespace std;

void gotoxy(short gx, short gy) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {gx, gy});
}

enum class Move {
    LEFT,
    RIGHT,
    DOWN,
    HARDDROP,
    ROTATE_CW,
    ROTATE_CCW,
    HOLD
};

template <int ROWS, int COLS>
class Game {
private:
    vector<vector<int> > board;

    int score;
    int playTime;

    bool paused;
    bool gameover;

    void hold() {

    }

public:
    Game() : board(ROWS, vector<int>(COLS)) {}

    void start() {

    }

    void run() {

    }

    void pause() {

    }

    void applyMove(Move move) {
        gotoxy(0, 0);
        cout << static_cast<int>(move);
    }

    
};