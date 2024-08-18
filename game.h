#ifndef GAME_H
#define GAME_H

#include <bits/stdc++.h>
#include "consoleColor.h"
#include "lazyPrinter.h"
#include "bag.h"
#include "fallingBlock.h"
#include "holdedBlock.h"
#include "timer.h"
#include "move.h"
#include "rotateDirection.h"
using namespace std;

// ms 기준
const int dropTime = 500;
const int hardDropAnimationTime = 30;
const int breakRowAnimationTime = 90;

template <int ROWS, int COLS>
class Game {
private:
    // 출력 관련
    static constexpr int LEN = 1; // 격자칸의 한 변에 사용될 픽셀 개수
    static constexpr int HOLD_BORDER_WIDTH = 6;
    static constexpr int HOLD_BORDER_HEIGHT = 4;
    static constexpr int MARGIN_HEIGHT = 3; // 보드판 상단에 들어갈 여백 길이(단위 : 격자개수)
    static constexpr int MARGIN_WIDTH = HOLD_BORDER_WIDTH + 2; // 보드판 좌측에 들어갈 여백 길이(단위 : 격자개수)

    vector<pair<int, int> > borderPos;

    LazyPrinter lazyPrinter;

    // 게임 로직 관련
    vector<vector<int> > board;
    
    Bag<7> bag;
    FallingBlock fallingBlock;
    HoldedBlock holdedBlock;

    Timer timer_drop;
    Timer timer_hardDropped;
    Timer timer_breakRow;

    vector<int> fullRows;

    // 추가 정보
    int brokenLines;
    double playTime;

    bool paused;
    bool gameover;

    void hardDrop() {
        while (checkFallingBlockCanDrop()) fallingBlock.drop();

        // hard drop 후에는 바로 put되도록 타이머 조정
        timer_drop.end();
        timer_hardDropped.init();
    }

    void hold() {
        if (!holdedBlock.getKind() || holdedBlock.canHold()) {
            int prevHoldedBlockKind = holdedBlock.getKind();
            holdedBlock = HoldedBlock(fallingBlock.getKind());
            makeNewFallingBlock(prevHoldedBlockKind);
        }
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
    
    bool checkFallingBlockCanRotate(RotateDirection direction) {
        for (Coordinate coordinate : fallingBlock.getShape()) {
            coordinate.rotate(direction);
            if (!checkGridCanFill(coordinate.x + fallingBlock.getX(), coordinate.y + fallingBlock.getY())) return false;
        }

        return true;
    }
    
    bool checkFallingBlockCanDrop() {
        return checkFallingBlockCanMove(0, 1);
    }

    int howFarCanDrop() {
        int dy = 0;
        while (checkFallingBlockCanMove(0, dy + 1)) ++dy;
        return dy;
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
        if (timer_breakRow.isOver()) {
            int rowToBreak = fullRows.back();
            fullRows.pop_back();

            for (int col = 0; col < COLS; col++) {
                for (int row = rowToBreak; row > 0; row--) board[row][col] = board[row - 1][col];
                board[0][col] = 0;
            }

            for (auto &fullRow : fullRows) ++fullRow;
        }
    }

    int putFallingBlock() { // 꽉 차있는 줄 개수 리턴
        int kind = fallingBlock.getKind();
        for (auto [x, y] : fallingBlock.getCoordinates()) board[y][x] = kind;
        fallingBlock.put();

        updateFullRows();
        if (haveFullRow()) timer_breakRow.init();
        return fullRows.size();
    }

    void makeNewFallingBlock(int kind=0) {
        if (!kind) kind = bag.takeOut();
        fallingBlock = FallingBlock(kind, COLS / 2);
        timer_drop.init();
    }

    void update() {
        if (haveFullRow()) breakFullRow();
        else if (timer_drop.isOver()) {
            if (checkFallingBlockCanDrop()) fallingBlock.drop();
            else {
                if (checkFallingBlockInBorder()) {
                    brokenLines += putFallingBlock();
                    holdedBlock.activate();
                    makeNewFallingBlock();
                }
                else gameover = true;
            }
        }
    }

    void drawGrid(int x, int y) {
        lazyPrinter.rect((MARGIN_WIDTH + x) * LEN, (MARGIN_HEIGHT + y) * LEN, LEN, LEN);
    }

    void drawMargin() {
        lazyPrinter.setColor(ConsoleColor::ORIGINALBG);
        // lazyPrinter.rect(0, 0, (MARGIN_WIDTH + COLS + 2) * LEN, (MARGIN_HEIGHT + ROWS + 2) * LEN);
        lazyPrinter.rect(MARGIN_WIDTH * LEN, 0, (COLS + 2) * LEN, MARGIN_HEIGHT * LEN);
        lazyPrinter.rect(0, MARGIN_HEIGHT * LEN, MARGIN_WIDTH * LEN, (ROWS + 2) * LEN);
    }

    void drawBorder() {
        lazyPrinter.setColor(ConsoleColor::BORDER_DEFAULT, ConsoleColor::BORDER_DEFAULT);
        // if (!timer_hardDropped.isOver()) lazyPrinter.setColor(ConsoleColor::WHITE, ConsoleColor::WHITE);
        for (auto [i, j] : borderPos) drawGrid(i, j);
    }

    void drawBoard() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                lazyPrinter.setColor(tetrominoColor[board[i][j]], tetrominoColor[board[i][j]]);
                drawGrid(j + 1, i + 1);
            }
        }

        // highlight Full Rows
        for (auto fullRow : fullRows) {
            for (int j = 0; j < COLS; j++) {
                lazyPrinter.setColor(tetrominoColor[board[fullRow][j]], ConsoleColor::FULL_ROW_HIGHLIGHT);
                drawGrid(j + 1, fullRow + 1);
            }
        }
    }
    
    void drawFallingBlock() {
        // shadow
        int depth = howFarCanDrop();
        lazyPrinter.setColor(tetrominoColor[fallingBlock.getKind()]);
        for (auto [x, y] : fallingBlock.getCoordinates()) drawGrid(x + 1, y + depth + 1);

        // falling block
        lazyPrinter.setColor(tetrominoColor[fallingBlock.getKind()], tetrominoColor[fallingBlock.getKind()]);
        for (auto [x, y] : fallingBlock.getCoordinates()) drawGrid(x + 1, y + 1);
    }

    void drawHold() {
        // hold Border 가로경계
        lazyPrinter.setColor(ConsoleColor::BORDER_DEFAULT, ConsoleColor::BORDER_DEFAULT);
        for (int x = 2; x < MARGIN_WIDTH; x++) {
            drawGrid(-x, 0);
            drawGrid(-x, HOLD_BORDER_HEIGHT);
        }

        // hold Border 세로경계
        // lazyPrinter.rectBySubPixel(LEN * PIXEL_WIDTH, MARGIN_HEIGHT * LEN * PIXEL_HEIGHT, LEN * PIXEL_WIDTH / 2, LEN * HOLD_BORDER_HEIGHT);
        // lazyPrinter.rectBySubPixel((HOLD_BORDER_WIDTH + 1) * LEN * PIXEL_WIDTH - PIXEL_WIDTH / 2, MARGIN_HEIGHT * LEN * PIXEL_HEIGHT, LEN * PIXEL_WIDTH / 2, LEN * HOLD_BORDER_HEIGHT);

        // hold Border title
        lazyPrinter.setColor(ConsoleColor::BORDER_DEFAULT);
        lazyPrinter.setXY(MARGIN_WIDTH / 2 * LEN * PIXEL_WIDTH, MARGIN_HEIGHT * LEN * PIXEL_HEIGHT);
        lazyPrinter.centerAlignedText(" HOLD ");

        // holded Block
        int hx = -MARGIN_WIDTH / 2 - holdedBlock.getCenterX();
        int hy = HOLD_BORDER_HEIGHT - 1 - holdedBlock.getMaxY();

        if (holdedBlock.canHold()) lazyPrinter.setColor(tetrominoColor[holdedBlock.getKind()], tetrominoColor[holdedBlock.getKind()]);
        else lazyPrinter.setColor(tetrominoColor[holdedBlock.getKind()]);

        for (auto [x, y] : holdedBlock.getShape()) drawGrid(x + hx, y + hy);
    }

    void display() {
        // harddrop 진동효과
        // if (!timer_hardDropped.isOver()) lazyPrinter.translate(0, HARDDROP_EFFECT_PIXEL_LEN);

        drawMargin();
        drawBorder();
        drawBoard();
        drawFallingBlock();
        drawHold();

        lazyPrinter.render();
    }

public:
    Game(int sleepTime) : \
        board(ROWS, vector<int>(COLS)), lazyPrinter((MARGIN_WIDTH + COLS + 2) * LEN, (MARGIN_HEIGHT + ROWS + 2) * LEN), \
        timer_drop(dropTime / sleepTime, true), \
        timer_hardDropped(hardDropAnimationTime / sleepTime, false), \
        timer_breakRow(breakRowAnimationTime / sleepTime, true) {

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

        fullRows.clear();

        // init
        brokenLines = 0;
        playTime = 0;

        paused = false;
        gameover = false;
        
        // falling block
        makeNewFallingBlock();
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
        if (gameover) return;

        paused = false;

        bool canApply = false;
        switch (move) {
            case Move::LEFT:
                canApply = checkFallingBlockCanMove(-1, 0);
                break;

            case Move::DOWN:
                canApply = checkFallingBlockCanMove(0, 1);
                break;

            case Move::RIGHT:
                canApply = checkFallingBlockCanMove(1, 0);
                break;

            case Move::ROTATE_CCW:
                canApply = checkFallingBlockCanRotate(RotateDirection::COUNTERCLOCKWISE);
                break;

            case Move::ROTATE_CW:
                canApply = checkFallingBlockCanRotate(RotateDirection::CLOCKWISE);
                break;

            case Move::HARDDROP:
                hardDrop();
                return;

            case Move::HOLD:
                hold();
                return;
        }

        if (canApply) {
            fallingBlock.applyMove(move);
            if (!checkFallingBlockCanDrop()) timer_drop.init();
        }
    }
    
};

#endif