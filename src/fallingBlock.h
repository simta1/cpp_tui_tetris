#ifndef FALLINGBLOCK_H
#define FALLINGBLOCK_H

#include "tetromino.h"
#include "move.h"
#include "rotateDirection.h"

class FallingBlock : public Tetromino {
private:
    int x, y;
    bool fixed;

public:
    FallingBlock() : Tetromino() {}
    explicit FallingBlock(int kind, int boardCenterX) : Tetromino(kind) {
        // board 중앙 최상단으로 이동
        x = boardCenterX - getCenterX();
        y = -(getMaxY() + 1);
        fixed = false;
    }

    // shape(상대좌표)에 현재 fallingBlock의 좌표만큼 평행이동시킨 절대좌표 리턴
    vector<Coordinate> getCoordinates() const {
        vector<Coordinate> res = getShape();
        for (auto &[cx, cy] : res) {
            cx += x;
            cy += y;
        }

        return res;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    void drop() {
        ++y;
    }

    void put() {
        fixed = true;
    }

    void wallKick(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void applyMove(Move move) {
        if (fixed) return;

        switch (move) {
            case Move::LEFT:
                --x;
                break;

            case Move::DOWN:
                ++y;
                break;

            case Move::RIGHT:
                ++x;
                break;

            case Move::ROTATE_CCW:
                rotate(RotateDirection::COUNTERCLOCKWISE);
                break;

            case Move::ROTATE_CW:
                rotate(RotateDirection::CLOCKWISE);
                break;
        }
    }
};

#endif