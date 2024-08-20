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
#include "userControlKey.h"
using namespace std;

template <int ROWS, int COLS>
class Game {
private:
    // 게임 로직 관련 상수
        static constexpr int NEXTBLOCK_COUNT = 5;

    // 출력 관련 상수

        // 진동효과 세기 (단위 : pixel 개수)
        static constexpr int BREAKROW_VIBRATION_LEN = 1;

        // 격자칸 한 변에 사용할 픽셀 개수
        static constexpr int LEN = 1;

        // UI 크기 (단위 : 격자개수)
        static constexpr int BOARD_BORDER_THICKNESS = 1;
        static constexpr int HOLD_BORDER_WIDTH = 8;
        static constexpr int HOLD_BORDER_HEIGHT = 4;
        static constexpr int MARGIN_HEIGHT = 3; // 보드판 상단 여백 길이
        static constexpr int MARGIN_WIDTH = HOLD_BORDER_WIDTH + 2; // 보드판 좌측 여백 길이
        static constexpr int GAMEINFO_MARGIN = 2;

        static constexpr int MIN_ROWS = max(HOLD_BORDER_HEIGHT + -~GAMEINFO_MARGIN * 2 + GAMEINFO_MARGIN + USER_CONTROL_KEY_COUNT, HOLD_BORDER_HEIGHT * NEXTBLOCK_COUNT) - 2 * BOARD_BORDER_THICKNESS;
        static constexpr int TETROMINO_MAX_WIDTH = 4; // 테트로미노 최대 길이
        static_assert(ROWS >= MIN_ROWS, "행 개수 부족");
        static_assert(COLS >= TETROMINO_MAX_WIDTH, "열 개수 부족");
        static_assert(HOLD_BORDER_WIDTH >= TETROMINO_MAX_WIDTH, "공백 길이 부족");

        // HARDDROP 충격파효과
        static constexpr double HARDDROP_WAVE_VELOCITY = 1.5;
        static constexpr int HARDDROP_WAVE_INSENSITIVITY = 6;

        // 애니메이션 시간 (단위 : ms)
        static constexpr int sleepDuration = 10;
        static constexpr int dropTime = 500;
        static constexpr int dropTimeLowerLimit = 45;
        static constexpr int dropSpeedUpPeriod = 10000;
        static constexpr int hardDropAnimationDuration = 50;
        static constexpr int breakRowAnimationDuration = 30;
        static constexpr int breakRowFlashDuration = 20;
        static constexpr int breakRowVibrationPeriod = breakRowAnimationDuration / 2;
        static constexpr int rainbowAnimationDuration = 2000;
        static constexpr int rainbowAnimationMoveDuration = 100;

        static_assert(breakRowVibrationPeriod >= sleepDuration, "breakRowVibrationPeriod too small");
        static_assert(breakRowFlashDuration >= sleepDuration, "breakRowFlashDuration too small");
        static_assert(rainbowAnimationMoveDuration >= sleepDuration, "rainbowAnimationMoveDuration too small");

        // 테트로미노 떨어지는 시간간격 감소 비율
        static constexpr double dropSpeedUpRate = 0.9;

        // lock delay 초기화 제한 횟수
        static constexpr int lockDelayResetLimit = 8;

    // board 관련
    vector<vector<int> > board;
    vector<int> fullRows;
    vector<pair<int, int> > borderPos;

    // tetromino 관련
    Bag<7> bag;
    deque<Tetromino> nextBlocks; // queue로만 쓸 거긴 한데 인덱싱 필요해서 덱으로 사용
    FallingBlock fallingBlock;
    HoldedBlock holdedBlock;

    int lockDelayResetCount;

    // timer
    Timer timer_drop;
    Timer timer_hardDropped;
    Timer timer_breakRow;
    Timer timer_rainbowBorder;

    // 게임 정보
    int brokenLines;
    int playTime;

    bool paused;
    bool gameover;

    // printer // 콘솔창 출력 담당
    mutable LazyPrinter lazyPrinter;

    // harddrop 애니메이션 관련
    bool hardDropped;
    int prevHardDropHeight;
    int prevHardDropPosX;
    int prevHardDropPosY;

    // 무지개 애니메이션 관련
    bool rainbowAnimationOn;

    void hardDrop() {
        // 줄 부수는 애니메이션 끝나기 전에 hard drop된 경우 즉시 애니메이션 종료 후 줄 부순 뒤 hard drop 적용
        if (haveFullRow()) breakFullRowsInstantly();

        // 테트로미노 이동
        prevHardDropHeight = 0;
        while (checkFallingBlockCanDrop()) {
            fallingBlock.drop();
            ++prevHardDropHeight;
        }
        prevHardDropPosX = fallingBlock.getX() + fallingBlock.getCenterX();
        prevHardDropPosY = fallingBlock.getY() + fallingBlock.getMaxY();

        // hard drop 후에는 바로 put되도록 타이머 보정
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

    bool inRange(int x, int y) const {
        return x >= 0 && x < COLS && y >= 0 && y < ROWS;
    }

    bool checkGridCanFill(int x, int y) const {
        if (y < 0) return (x >= 0 && x < COLS);

        if (!inRange(x, y)) return false;
        return board[y][x] == 0;
    }

    bool checkFallingBlockCanMove(int dx, int dy) const {
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
    
    bool checkFallingBlockCanDrop() const {
        return checkFallingBlockCanMove(0, 1);
    }

    int howFarCanDrop() const {
        int dy = 0;
        while (checkFallingBlockCanMove(0, dy + 1)) ++dy;
        return dy;
    }

    bool checkFallingBlockInBorder() const {
        for (auto [x, y] : fallingBlock.getCoordinates()) {
            if (y < 0) return false;
        }

        return true;
    }

    bool checkRowFull(int row) const {
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

    bool haveFullRow() const {
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

    void breakFullRowsInstantly() {
        while (!fullRows.empty()) {
            int rowToBreak = fullRows.back();
            fullRows.pop_back();

            for (int col = 0; col < COLS; col++) {
                for (int row = rowToBreak; row > 0; row--) board[row][col] = board[row - 1][col];
                board[0][col] = 0;
            }

            for (auto &fullRow : fullRows) ++fullRow;
        }
    }

    int putFallingBlock() { // 테트로미노 보드에 고정 후 꽉 찬 줄 개수 리턴
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
        lockDelayResetCount = 0;
        timer_drop.init();
    }

    void updateFallingBlockDropSpeed() {
        if (dropSpeedUpPeriod < sleepDuration || (playTime / sleepDuration) % (dropSpeedUpPeriod / sleepDuration) == 0) timer_drop.speedUp(dropSpeedUpRate, dropTimeLowerLimit / sleepDuration);
    }

    void update() {
        if (haveFullRow()) breakFullRow();
        else if (timer_drop.isOver()) {
            if (checkFallingBlockCanDrop()) fallingBlock.drop();
            else {
                if (checkFallingBlockInBorder()) {
                    int brokenLineCnt = putFallingBlock();
                    brokenLines += brokenLineCnt;
                    if (brokenLineCnt >= 3) timer_rainbowBorder.init();

                    holdedBlock.activate();
                    makeNewFallingBlock();
                }
                else gameover = true;
            }
        }

        hardDropped = !timer_hardDropped.isOver();

        // 무지개 효과.
        // breakRow 애니메이션이랑 끝난 후 시작
        // 만약 이미 무지개 효과가 나오는 중이었는데 breakRow로 인해 애니메이션 끊기면 이상하기 때문에 애니메이션 진행 중이었다면 계속 진행
        rainbowAnimationOn = (!rainbowAnimationOn && haveFullRow()) ? false : !timer_rainbowBorder.isOver();

        playTime += sleepDuration;
        updateFallingBlockDropSpeed();
    }

    void drawGrid(int x, int y) const {
        lazyPrinter.rect((MARGIN_WIDTH + x) * LEN, (MARGIN_HEIGHT + y) * LEN, LEN, LEN);
    }

    void drawMargin() const {
        lazyPrinter.setColor(ConsoleColor::ORIGINALBG);
        
        // board 상단
        lazyPrinter.rect(MARGIN_WIDTH * LEN, 0, (COLS + 2) * LEN, MARGIN_HEIGHT * LEN);

        // board 좌측
        lazyPrinter.rect(0, MARGIN_HEIGHT * LEN, MARGIN_WIDTH * LEN, (ROWS + 2) * LEN);

        // board 우측
        lazyPrinter.rect((MARGIN_WIDTH + COLS + 2) * LEN, MARGIN_HEIGHT * LEN, MARGIN_WIDTH * LEN, (ROWS + 2) * LEN);
    }

    void drawBorder() const {
        if (rainbowAnimationOn) {
            for (auto [x, y] : borderPos) {
                ConsoleColor rainbowColor = getRainbowColor(getTaxiDist(0, 0, x, y) + timer_rainbowBorder.getTime() / (rainbowAnimationMoveDuration / sleepDuration));
                lazyPrinter.setColor(rainbowColor, ConsoleColor::ORIGINALBG);
                drawGrid(x, y);
            }
        }
        else {
            lazyPrinter.setColor(ConsoleColor::BORDER_DEFAULT, ConsoleColor::BORDER_DEFAULT);
            for (auto [x, y] : borderPos) drawGrid(x, y);
        }
    }

    void drawBoard() const {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                lazyPrinter.setColor(tetrominoColor[board[i][j]], tetrominoColor[board[i][j]]);
                drawGrid(j + BOARD_BORDER_THICKNESS, i + BOARD_BORDER_THICKNESS);
            }
        }

        // highlight Full Rows
        for (auto fullRow : fullRows) {
            for (int j = 0; j < COLS; j++) {
                if (timer_breakRow.getTime() / (breakRowFlashDuration / sleepDuration) & 1) lazyPrinter.setColor(ConsoleColor::WHITE, ConsoleColor::ORIGINALBG);
                else lazyPrinter.setColor(tetrominoColor[board[fullRow][j]], ConsoleColor::ORIGINALBG);
                drawGrid(j + BOARD_BORDER_THICKNESS, fullRow + BOARD_BORDER_THICKNESS);
            }
        }
    }
    
    void drawFallingBlock() const {
        // shadow
        int depth = howFarCanDrop();
        lazyPrinter.setColor(tetrominoColor[fallingBlock.getKind()]);
        for (auto [x, y] : fallingBlock.getCoordinates()) drawGrid(x + BOARD_BORDER_THICKNESS, y + depth + BOARD_BORDER_THICKNESS);

        // falling block
        lazyPrinter.setColor(tetrominoColor[fallingBlock.getKind()], tetrominoColor[fallingBlock.getKind()]);
        for (auto [x, y] : fallingBlock.getCoordinates()) drawGrid(x + BOARD_BORDER_THICKNESS, y + BOARD_BORDER_THICKNESS);
    }

    string getBorderString(string st="") const {
        int len = HOLD_BORDER_WIDTH * PIXEL_WIDTH - st.size() >> 1;
        return string(len, '-') + st + string(len, '-');
    }

    void drawHoldedBlock() const {
        // holdedBlock title
        lazyPrinter.setColor(ConsoleColor::ORIGINAL_FONT);
        lazyPrinter.setxyByPixel(MARGIN_WIDTH / 2 * LEN, MARGIN_HEIGHT * LEN);
        lazyPrinter.centerAlignedText(getBorderString(" HOLD "));

        // holdedBlock border
        lazyPrinter.setxyByPixel(MARGIN_WIDTH / 2 * LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT) * LEN);
        lazyPrinter.centerAlignedText(getBorderString());

        // holded Block
        int blockX = -MARGIN_WIDTH / 2 - holdedBlock.getCenterX();
        int blockY = HOLD_BORDER_HEIGHT - 1 - holdedBlock.getMaxY();

        if (holdedBlock.canHold()) lazyPrinter.setColor(tetrominoColor[holdedBlock.getKind()], tetrominoColor[holdedBlock.getKind()]);
        else lazyPrinter.setColor(tetrominoColor[holdedBlock.getKind()]);

        for (auto [x, y] : holdedBlock.getShape()) drawGrid(x + blockX, y + blockY);
    }

    void drawNextBlocks() const {
        // nextBlocks title
        lazyPrinter.setColor(ConsoleColor::ORIGINAL_FONT);
        lazyPrinter.setxyByPixel(MARGIN_WIDTH + COLS + BOARD_BORDER_THICKNESS * 2 + MARGIN_WIDTH / 2 * LEN, MARGIN_HEIGHT * LEN);
        lazyPrinter.centerAlignedText(getBorderString(" NEXT "));

        // nextBlocks border
        for (int i = 0; i < NEXTBLOCK_COUNT; i++) {
            lazyPrinter.setxyByPixel(MARGIN_WIDTH + COLS + BOARD_BORDER_THICKNESS * 2 + MARGIN_WIDTH / 2 * LEN, (MARGIN_HEIGHT + -~i * HOLD_BORDER_HEIGHT) * LEN);
            lazyPrinter.centerAlignedText(getBorderString());
        }

        // nextBlocks
        for (int i = 0; i < NEXTBLOCK_COUNT; i++) {
            int blockX = COLS + BOARD_BORDER_THICKNESS * 2 + MARGIN_WIDTH / 2 - nextBlocks[i].getCenterX();
            int blockY = -~i * HOLD_BORDER_HEIGHT - 1 - nextBlocks[i].getMaxY();
            lazyPrinter.setColor(tetrominoColor[nextBlocks[i].getKind()], tetrominoColor[nextBlocks[i].getKind()]);
            for (auto [x, y] : nextBlocks[i].getShape()) drawGrid(x + blockX, y + blockY);
        }
    }

    int getTaxiDist(int x1, int y1, int x2, int y2) const {
        return abs(x1 - x2) + abs(y1 - y2);
        // return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }

    void drawHardDropShockWave() const {
        if (!hardDropped || !haveFullRow()) return;

        int harddropWaveMaxDistance = prevHardDropHeight / HARDDROP_WAVE_INSENSITIVITY;
        if (harddropWaveMaxDistance <= 1) return;
        
        int shockwavePos = min(int(timer_hardDropped.getTime() * HARDDROP_WAVE_VELOCITY), harddropWaveMaxDistance);

        lazyPrinter.setColor(ConsoleColor::WHITE, ConsoleColor::WHITE);
        for (int x = 0; x < COLS + BOARD_BORDER_THICKNESS * 2; x++) {
            for (int y = 0; y < ROWS + BOARD_BORDER_THICKNESS * 2; y++) {
                if (getTaxiDist(prevHardDropPosX, prevHardDropPosY, x, y) == shockwavePos) drawGrid(x, y);
            }
        }
    }

    void drawGameInfo() const {
        lazyPrinter.setColor(ConsoleColor::ORIGINAL_FONT);

        // broken lines
        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + GAMEINFO_MARGIN) * LEN);
        lazyPrinter.leftAlignedText("lines:");

        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + -~GAMEINFO_MARGIN) * LEN);
        lazyPrinter.leftAlignedText(to_string(brokenLines));

        // play time
        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + -~GAMEINFO_MARGIN + GAMEINFO_MARGIN) * LEN);
        lazyPrinter.leftAlignedText("time");

        lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + -~GAMEINFO_MARGIN * 2) * LEN);
        lazyPrinter.leftAlignedText(to_string(playTime / 1000) + "." + to_string(playTime % 1000 * 0.001).substr(2, 2));

        // gameover, paused 여부
        if (gameover) {
            lazyPrinter.setColor(ConsoleColor::BLACK, ConsoleColor::WHITE);
            lazyPrinter.setxyByPixel((MARGIN_WIDTH + BOARD_BORDER_THICKNESS + COLS / 2) * LEN, (MARGIN_HEIGHT + BOARD_BORDER_THICKNESS + ROWS / 2 - 1) * LEN);
            lazyPrinter.centerAlignedText("GAME OVER");

            lazyPrinter.setColor(ConsoleColor::WHITE, ConsoleColor::BLACK);
            lazyPrinter.setxyByPixel((MARGIN_WIDTH + BOARD_BORDER_THICKNESS + COLS / 2) * LEN, (MARGIN_HEIGHT + BOARD_BORDER_THICKNESS + ROWS / 2) * LEN);
            lazyPrinter.centerAlignedText("press " + string(1, KEY_REPLAY_GAME) + " to replay");
        }
        else if (paused) {
            lazyPrinter.setxyByPixel((MARGIN_WIDTH + BOARD_BORDER_THICKNESS + COLS / 2) * LEN, (MARGIN_HEIGHT + BOARD_BORDER_THICKNESS + ROWS / 2) * LEN);
            lazyPrinter.centerAlignedText("PAUSED");
        }
    }

    void drawManual() const {
        for (int i = 0; i < USER_CONTROL_KEYS.size(); i++) {
            lazyPrinter.setxyByPixel(LEN, (MARGIN_HEIGHT + HOLD_BORDER_HEIGHT + -~GAMEINFO_MARGIN * 2 + GAMEINFO_MARGIN + i) * LEN);
            lazyPrinter.leftAlignedText(USER_CONTROL_KEYS[i]);
        }
    }

    void display() const {
        lazyPrinter.init();

        // breakRow 진동효과
        int vibrate = 1;
        if (haveFullRow()) {
            if (timer_breakRow.getTime() / (breakRowVibrationPeriod / sleepDuration) & 1) vibrate += 1;
            else vibrate -= 1;
        }
        lazyPrinter.translate(vibrate * BREAKROW_VIBRATION_LEN, 0);

        drawMargin();
        drawBorder();
        drawBoard();
        drawFallingBlock();
        drawHoldedBlock();
        drawNextBlocks();
        drawHardDropShockWave();
        drawGameInfo();
        drawManual();

        lazyPrinter.render();
    }

public:
    Game() : \
        board(ROWS, vector<int>(COLS)), lazyPrinter((MARGIN_WIDTH + 2 * BOARD_BORDER_THICKNESS + COLS + MARGIN_WIDTH) * LEN, 2 * BREAKROW_VIBRATION_LEN, (MARGIN_HEIGHT + 2 * BOARD_BORDER_THICKNESS + ROWS) * LEN, 1), \
        timer_drop(dropTime / sleepDuration, true), \
        timer_hardDropped(hardDropAnimationDuration / sleepDuration, false), \
        timer_breakRow(breakRowAnimationDuration / sleepDuration, true), \
        timer_rainbowBorder(rainbowAnimationDuration / sleepDuration, false) {

        // border
        borderPos.reserve((ROWS + COLS - 4 << 1) * BOARD_BORDER_THICKNESS + 4 * BOARD_BORDER_THICKNESS * BOARD_BORDER_THICKNESS);

        for (int x = 0; x < COLS + 2 * BOARD_BORDER_THICKNESS; x++) {
            for (int y = 0; y < BOARD_BORDER_THICKNESS; y++) {
                borderPos.push_back({x, y});
                borderPos.push_back({x, ROWS + 2 * BOARD_BORDER_THICKNESS - 1 - y});
            }
        }

        for (int y = BOARD_BORDER_THICKNESS; y < ROWS + BOARD_BORDER_THICKNESS; y++) {
            for (int x = 0; x < BOARD_BORDER_THICKNESS; x++) {
                borderPos.push_back({x, y});
                borderPos.push_back({COLS + 2 * BOARD_BORDER_THICKNESS - 1 - x, y});
            }
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
        timer_drop.set(dropTime / sleepDuration);
        timer_hardDropped.end();
        timer_breakRow.end();
        timer_rainbowBorder.end();

        // 추가 게임 정보
        brokenLines = 0;
        playTime = 0;
        paused = false;
        gameover = false;
    }

    void run() {
        if (!paused && !gameover) update();
        display();

        Sleep(sleepDuration);
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
            
            // lock delay 초기화 보정
            if (!checkFallingBlockCanDrop() && lockDelayResetCount++ <= lockDelayResetLimit) timer_drop.init();
        }
    }
    
    void changeThema() {
        lazyPrinter.changeThema();
    }
};

#endif