#include <bits/stdc++.h>
#include "utils.h"
using namespace std;

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
    const int LEN = 1; // 격자칸의 한 변에 사용될 픽셀 개수
    vector<pair<int, int> > updatedPos; // 0-based // 출력 최적화용. 화면 출력할 때 이전 화면과 달라진 좌표만 출력.

    vector<vector<int> > board;
    vector<pair<int, int> > borderPos; // first : i(=y), second : j(=x)

    int score;
    int playTime;

    bool paused;
    bool gameover;

    void hold() {

    }

    void drawPixel(int i, int j) {
        rect(j * LEN, i * LEN, LEN, LEN);
    }

    void update() {
        // test
            int ri = rand() % ROWS;
            int rj = rand() % COLS;
            board[ri][rj] = rand() % 15;
            updatedPos.push_back({ri, rj});
        //
    }

    void display() {
        for (auto [i, j] : updatedPos) {
            setColor(static_cast<ConsoleColor>(board[i][j]));
            drawPixel(-~i, -~j); // 1-based
        }
        updatedPos.clear();
    }

public:
    Game() : board(ROWS, vector<int>(COLS)) {
        // border
        borderPos.reserve(ROWS + COLS + 2 << 1);

        for (int j = 0; j <= COLS + 1; j++) {
            borderPos.push_back({0, j});
            borderPos.push_back({ROWS + 1, j});
        }

        for (int i = 1; i <= ROWS; i++) {
            borderPos.push_back({i, 0});
            borderPos.push_back({i, COLS + 1});
        }
    }

    void start() {
        setColor(ConsoleColor::GRAY);
        for (auto [i, j] : borderPos) drawPixel(i, j);
    }

    void run() {
        update();
        display();
    }

    void pause() {

    }

    void applyMove(Move move) {
        gotoxy(0, 0);
        cout << static_cast<int>(move);
    }

    
};