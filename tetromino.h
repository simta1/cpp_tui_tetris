#include <bits/stdc++.h>
#include "coordinate.h"
using namespace std;

const vector<ConsoleColor> tetrominoColor = {
    ConsoleColor::BLACK,
    ConsoleColor::CYAN,
    ConsoleColor::BLUE,
    ConsoleColor::MAGENTA,
    ConsoleColor::YELLOW,
    ConsoleColor::GREEN,
    ConsoleColor::DARKRED,
    ConsoleColor::DARKYELLOW
};

class Tetromino {
private:
    static constexpr int tetrominoShapes[8][4][2] = {
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},       // BLACK(dummy)
        {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},      // CYAN
        {{-1, 0}, {0, 0}, {1, 0}, {-1, -1}},    // BLUE
        {{-1, 0}, {0, 0}, {1, 0}, {0, -1}},     // MAGENTA
        {{-1, 0}, {0, 0}, {1, 0}, {1, -1}},     // YELLOW
        {{-1, 1}, {0, 0}, {0, 1}, {1, 0}},      // GREEN
        {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},      // DARKRED
        {{0, 0}, {0, 1}, {1, 0}, {1, 1}}        // DARKYELLOW
    };

    static constexpr int INF = 1e9;

    int kind;
    vector<Coordinate> shape;
    int minX, maxX, minY, maxY;

    void init() {
        minX = minY = INF;
        maxX = maxY = -INF;

        for (auto [x, y] : shape) {
            minX = min(minX, x);
            maxX = max(maxX, x);
            minY = min(minY, y);
            maxY = max(maxY, y);
        }
    }

public:
    Tetromino() = default;
    Tetromino(int kind) : kind(kind) {
        for (auto pos : tetrominoShapes[kind]) shape.push_back(Coordinate(pos[0], pos[1]));
        init();
    }
    
    int getKind() {
        return kind;
    }

    vector<Coordinate> getShape() {
        return shape;
    }
    
    int getCenterX() {
        return minX + maxX >> 1;
    }

    int getMaxY() {
        return maxY;
    }
};