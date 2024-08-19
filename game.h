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
const int sleepTime = 10;
const int dropTime = 500;
const int hardDropAnimationTime = 150;
const int breakRowAnimationTime = 70;
const int breakRowVibrationPeriod = breakRowAnimationTime / 2;

template <int ROWS, int COLS>
class Game {
private:
    // nextBlocks 개수
    static constexpr int NEXTBLOCK_COUNT = 5;

    // 격자칸 한 변에 사용할 픽셀 개수
    static constexpr int LEN = 1;

    // TUI 길이 (단위 : 격자개수)
    static constexpr int HOLD_BORDER_WIDTH = 6;
    static constexpr int HOLD_BORDER_HEIGHT = 4;
    static constexpr int MARGIN_HEIGHT = 3; // 보드판 상단에 들어갈 여백 길이(단위 : 격자개수)
    static constexpr int MARGIN_WIDTH = HOLD_BORDER_WIDTH + 2; // 보드판 좌측에 들어갈 여백 길이(단위 : 격자개수)
    static constexpr int GAMEINFO_MARGIN = 2;

    static constexpr int MIN_ROWS = max(-~HOLD_BORDER_HEIGHT + -~GAMEINFO_MARGIN * 2, HOLD_BORDER_HEIGHT * NEXTBLOCK_COUNT);
    // static_assert(ROWS >= MIN_ROWS, "게임보드는 최소 " + to_string(MIN_ROWS) + "개의 행을 가져야 합니다.");

    // HARDDROP 충격파 효과 관련
    const double HARDDROP_WAVE_VELOCITY = 1;
    const int HARDDROP_WAVE_INSENSITIVITY = 6;

    vector<pair<int, int> > borderPos;

    LazyPrinter lazyPrinter;

    // 게임 로직 관련
    vector<vector<int> > board;
    vector<int> fullRows;
    
    Bag<7> bag;
    deque<Tetromino> nextBlocks; // queue로만 쓸 거긴 한데 인덱싱 필요해서 덱으로 사용
    FallingBlock fallingBlock;
    HoldedBlock holdedBlock;

    Timer timer_drop;
    Timer timer_hardDropped;
    Timer timer_breakRow;

    bool hardDropped;
    int prevHardDropHeight;
    int prevHardDropPosX;
    int prevHardDropPosY;

    // 추가 정보
    int brokenLines;
    int playTime;

    bool paused;
    bool gameover;

    void hardDrop() {
        prevHardDropHeight = 0;
        while (checkFallingBlockCanDrop()) {
            fallingBlock.drop();
            ++prevHardDropHeight;
        }
        prevHardDropPosX = fallingBlock.getX() + fallingBlock.getCenterX();
        prevHardDropPosY = fallingBlock.getY() + fallingBlock.getMaxY();

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
        // 회전 보정
        for (auto [dx, dy] : dposWallKick) {
            bool canRotate = [&]() {
                for (Coordinate coordinate : fallingBlock.getShape()) {
                    coordinate.rotate(direction);
                    if (!checkGridCanFill(coordinate.x + fallingBlock.getX() + dx, coordinate.y + fallingBlock.getY() + dy)) return false;
                }

                return true;
            }();

            if (canRotate) {
                fallingBlock.wallKick(dx, dy);
                return true;
            }
        }

        return false;
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
        if (!kind) {
            assert (!nextBlocks.empty());
            kind = nextBlocks.front().getKind();
            nextBlocks.pop_front();
            nextBlocks.push_back(Tetromino(bag.takeOut()));
        }

        fallingBlock = FallingBlock(kind, COLS / 2);
        timer_drop.init();
    }

    void update() {
        hardDropped = !timer_hardDropped.isOver();

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

        playTime += sleepTime;
    }

    void drawGrid(int x, int y) {
        lazyPrinter.rect((MARGIN_WIDTH + x) * LEN, (MARGIN_HEIGHT + y) * LEN, LEN, LEN);
    }

    void drawMargin() {
        lazyPrinter.setColor(ConsoleColor::ORIGINALBG);
        
        // board 상단
        lazyPrinter.rect(MARGIN_WIDTH * LEN, 0, (COLS + 2) * LEN, MARGIN_HEIGHT * LEN);

        // board 좌측
        lazyPrinter.rect(0, MARGIN_HEIGHT * LEN, MARGIN_WIDTH * LEN, (ROWS + 2) * LEN);

        // board 우측
        lazyPrinter.rect((MARGIN_WIDTH + COLS + 2) * LEN, MARGIN_HEIGHT * LEN, MARGIN_WIDTH * LEN, (ROWS + 2) * LEN);
    }

    void drawBorder() {
        for (auto [x, y] : borderPos) {
            lazyPrinter.setColor(ConsoleColor::BORDER_DEFAULT, ConsoleColor::BORDER_DEFAULT);
            drawGrid(x, y);
        }
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
                lazyPrinter.setColor(ConsoleColor::FULL_ROW_HIGHLIGHT, tetrominoColor[board[fullRow][j]]);
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

    void drawHoldedBlock() {
        // holdedBlock title
        lazyPrinter.setColor(ConsoleColor::ORIGINAL_FONT);
        lazyPrinter.setxyByPixel(MARGIN_WIDTH / 2 * LEN, MARGIN_HEIGHT * LEN);
        lazyPrinter.centerAlignedText("--- HOLD ---");

        // holdedBlock border
        lazyPrinter.setxyByPixel(MARGIN_WIDTH / 2 * LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT) * LEN);
        lazyPrinter.centerAlignedText("------------");

        // holded Block
        int hx = -MARGIN_WIDTH / 2 - holdedBlock.getCenterX();
        int hy = HOLD_BORDER_HEIGHT - 1 - holdedBlock.getMaxY();

        if (holdedBlock.canHold()) lazyPrinter.setColor(tetrominoColor[holdedBlock.getKind()], tetrominoColor[holdedBlock.getKind()]);
        else lazyPrinter.setColor(tetrominoColor[holdedBlock.getKind()]);

        for (auto [x, y] : holdedBlock.getShape()) drawGrid(x + hx, y + hy);
    }

    void drawNextBlocks() {
        // nextBlocks title
        lazyPrinter.setColor(ConsoleColor::ORIGINAL_FONT);
        lazyPrinter.setxyByPixel(MARGIN_WIDTH + COLS + 2 + MARGIN_WIDTH / 2 * LEN, MARGIN_HEIGHT * LEN);
        lazyPrinter.centerAlignedText("--- NEXT ---");

        // nextBlocks border
        for (int i = 0; i < NEXTBLOCK_COUNT; i++) {
            lazyPrinter.setxyByPixel(MARGIN_WIDTH + COLS + 2 + MARGIN_WIDTH / 2 * LEN, (MARGIN_HEIGHT + -~i * HOLD_BORDER_HEIGHT) * LEN);
            lazyPrinter.centerAlignedText("------------");
        }

        // nextBlocks
        for (int i = 0; i < NEXTBLOCK_COUNT; i++) {
            int nx = 1 + COLS + 1 + MARGIN_WIDTH / 2 - nextBlocks[i].getCenterX();
            int ny = -~i * HOLD_BORDER_HEIGHT - 1 - nextBlocks[i].getMaxY();
            lazyPrinter.setColor(tetrominoColor[nextBlocks[i].getKind()], tetrominoColor[nextBlocks[i].getKind()]);
            for (auto [x, y] : nextBlocks[i].getShape()) drawGrid(x + nx, y + ny);
        }
    }

    int getTaxiDist(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
        // return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }

    void drawHardDropShockWave() {
        if (!hardDropped || !haveFullRow()) return;

        int harddropWaveMaxDistance = prevHardDropHeight / HARDDROP_WAVE_INSENSITIVITY;
        if (harddropWaveMaxDistance <= 1) return;
        
        int shockwavePos = min(int(timer_hardDropped.getTime() * HARDDROP_WAVE_VELOCITY), harddropWaveMaxDistance);

        lazyPrinter.setColor(ConsoleColor::WHITE, ConsoleColor::WHITE);
        for (int i = 0; i <= ROWS + 1; i++) {
            for (int j = 0; j <= COLS + 1; j++) {
                if (getTaxiDist(prevHardDropPosX, prevHardDropPosY, j, i) == shockwavePos) drawGrid(j + 1, i + 1);
            }
        }
    }

    void drawGameInfo() {
        lazyPrinter.setColor(ConsoleColor::ORIGINAL_FONT);

        // broken lines
        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + GAMEINFO_MARGIN) * LEN);
        lazyPrinter.leftAlignedText("lines:");

        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + GAMEINFO_MARGIN + 1) * LEN);
        lazyPrinter.leftAlignedText(to_string(brokenLines));

        // play time
        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + 2 * GAMEINFO_MARGIN + 1) * LEN);
        lazyPrinter.leftAlignedText("time");

        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + 2 * GAMEINFO_MARGIN + 2) * LEN);
        lazyPrinter.leftAlignedText(to_string(playTime / 1000) + "." + to_string(playTime % 1000 / 10));

        // gameover, paused 여부
        if (gameover) {
            lazyPrinter.setColor(ConsoleColor::BLACK, ConsoleColor::WHITE);
            lazyPrinter.setxyByPixel((-~MARGIN_WIDTH + COLS / 2) * LEN, (-~MARGIN_HEIGHT + ROWS / 2) * LEN);
            lazyPrinter.centerAlignedText("GAME OVER");
        }
        else if (paused) {
            lazyPrinter.setxyByPixel((-~MARGIN_WIDTH + COLS / 2) * LEN, (-~MARGIN_HEIGHT + ROWS / 2) * LEN);
            lazyPrinter.centerAlignedText("PAUSED");
        }
    }

    void display() {
        lazyPrinter.init();

        // harddrop 진동효과 (세로)
        // if (!timer_hardDropped.isOver()) lazyPrinter.translate(0, HARDDROP_VIBRATION_LEN);

        // breakRow 진동효과 (가로)
        if (haveFullRow()) {
            int sign = 1 - 2 * (timer_breakRow.getTime() / (breakRowVibrationPeriod / sleepTime) % 2);
            lazyPrinter.translate(sign * BREAKROW_VIBRATION_LEN, 0);
        }

        drawMargin();
        drawBorder();
        drawBoard();
        drawFallingBlock();
        drawHoldedBlock();
        drawNextBlocks();
        drawHardDropShockWave();
        drawGameInfo();

        lazyPrinter.render();
    }

public:
    Game() : \
        board(ROWS, vector<int>(COLS)), lazyPrinter((MARGIN_WIDTH + 1 + COLS + 1 + MARGIN_WIDTH) * LEN, (MARGIN_HEIGHT + 1 + ROWS + 1) * LEN), \
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
        // board 관련
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) board[i][j] = 0;
        }
        fullRows.clear();

        // tetromino 관련
        bag.clear();
        nextBlocks.clear();
        for (int i = 0; i < NEXTBLOCK_COUNT; i++) nextBlocks.push_back(Tetromino(bag.takeOut()));
        makeNewFallingBlock();
        holdedBlock = HoldedBlock();

        // timer 관련
        timer_drop.init();
        timer_hardDropped.end();
        timer_breakRow.end();

        // 추가 게임 정보
        brokenLines = 0;
        playTime = 0;
        paused = false;
        gameover = false;
    }

    void run() {
        if (!paused && !gameover) update();
        display();
    }

    void pause() {
        paused ^= 1;
    }

    void replay() {
        if (gameover) start();
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