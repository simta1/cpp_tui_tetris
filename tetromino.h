#ifndef TETROMINO_H
#define TETROMINO_H

#include <bits/stdc++.h>
#include "coordinate.h"
#include "rotateDirection.h"
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
    Tetromino() : kind(0) {}
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
        return minX + maxX + 1 >> 1;
    }

    int getMinY() {
        return minY;
    }

    int getMaxY() {
        return maxY;
    }

    void rotate(RotateDirection direction) {
        if (kind == 7) return; // 네모모양, 회전해도 어차피 안 변함

        for (auto &coordinate : shape) coordinate.rotate(direction);
        init();
    }
};

#endif