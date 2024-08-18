#include <bits/stdc++.h>
#include "utils.h"
#include "bag.h"
#include "fallingBlock.h"
#include "timer.h"
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
    // 출력 관련
    const int LEN = 1; // 격자칸의 한 변에 사용될 픽셀 개수
    const int MARGIN = 3; // 보드판 위에 들어갈 여백 높이(단위 : 격자개수)
    vector<pair<int, int> > borderPos;

    // 게임 로직 관련
    vector<vector<int> > board;
    
    Bag<7> bag;
    FallingBlock fallingBlock;

    Timer timer_drop;
    Timer timer_hardDropped;
    Timer timer_breakRow;

    vector<int> fullRows;

    // 추가 정보
    int score;
    double playTime;

    bool paused;
    bool gameover;

    void hold() {

    }

    void drawGrid(int x, int y) {
        rect(x * LEN, (MARGIN + y) * LEN, LEN, LEN);
    }

    void deleteMargin() {
        setColor(ConsoleColor::ORIGINALBG);
        rect(0, 0, (COLS + 2) * LEN, MARGIN * LEN);
    }

    void drawBorder() {
        setColor(ConsoleColor::DARKGRAY);
        for (auto [i, j] : borderPos) drawGrid(i, j);
    }

    void drawBoard() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                setColor(tetrominoColor[board[i][j]]);
                drawGrid(j + 1, i + 1);
            }
        }
    }
    
    void drawFallingBlock() {
        setColor(tetrominoColor[fallingBlock.getKind()]);
        for (auto [x, y] : fallingBlock.getCoordinates()) drawGrid(x + 1, y + 1);
    }

    bool inRange(int x, int y) {
        return x >= 0 && x < COLS && y >= 0 && y < ROWS;
    }

    bool checkGridCanFill(int x, int y) {
        if (y < 0) return (x >= 0 && x < COLS);

        if (!inRange(x, y)) return false;
        return board[y][x] == 0;
    }

    bool checkFallingBlockCanMove(int dx, int dy) {
        for (auto [x, y] : fallingBlock.getCoordinates()) {
            if (!checkGridCanFill(x + dx, y + dy)) return false;
        }

        return true;
    }

    bool checkFallingBlockInBorder() {
        for (auto [x, y] : fallingBlock.getCoordinates()) {
            if (y < 0) return false;
        }

        return true;
    }

    bool checkRowFull(int row) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col] == 0) return false;
        }

        return true;
    }

    void updateFullRows() {
        for (int row = 0; row < ROWS; row++) {
            if (checkRowFull(row)) fullRows.push_back(row);
        }
    }

    bool haveFullRow() {
        return !fullRows.empty();
    }

    void breakFullRow() {

    }

    int putFallingBlock() { // 꽉 차있는 줄 개수 리턴
        int kind = fallingBlock.getKind();
        for (auto [x, y] : fallingBlock.getCoordinates()) board[y][x] = kind;

        updateFullRows();
        if (haveFullRow()) timer_breakRow.init();
        return fullRows.size();
    }

    void makeNewFallingBlock() {
        int kind = bag.takeOut();
        fallingBlock = FallingBlock(kind, COLS / 2);

        // test용 코드 삭제해야됨
        gotoxy(40, 0);
        cout << "KIND : " << kind << " ::: ";
        for (auto [x, y] : fallingBlock.getCoordinates()) cout << "(" << x << ", " << y << "), ";
    }

    void update() {
        if (haveFullRow()) breakFullRow();
        else if (timer_drop.isOver()) {
            if (checkFallingBlockCanMove(0, 1)) fallingBlock.drop();
            else {
                if (checkFallingBlockInBorder()) {
                    score += putFallingBlock();
                    // TODO HoldedBlock activate
                    makeNewFallingBlock();
                }
                else gameover = true;
            }
        }
    }

    void display() {
        // TODO 출력 최적화 해야 될 듯. 업데이트된 좌표만 출력하게 해야 될 듯 화면 깜빡임 너무 심함
        deleteMargin();
        drawBorder();
        drawBoard();
        drawFallingBlock();
    }

public:
    Game() : board(ROWS, vector<int>(COLS)), timer_drop(17, true), timer_hardDropped(1, false), timer_breakRow(4, true) {
        // border
        borderPos.reserve(ROWS + COLS + 2 << 1);

        for (int j = 0; j <= COLS + 1; j++) {
            borderPos.push_back({j, 0});
            borderPos.push_back({j, ROWS + 1});
        }

        for (int i = 1; i <= ROWS; i++) {
            borderPos.push_back({0, i});
            borderPos.push_back({COLS + 1, i});
        }
    }

    void start() {
        // board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) board[i][j] = 0;
        }

        // falling block
        makeNewFallingBlock();

        fullRows.clear();

        // init
        score = 0;
        playTime = 0;

        paused = false;
        gameover = false;
    }

    void run() {
        if (paused) return;
        
        if (!gameover) update();
        display();
    }

    void pause() {
        paused ^= 1;
    }

    void applyMove(Move move) {
        gotoxy(0, 0);
        cout << static_cast<int>(move);
    }
    
};