#ifndef FALLINGBLOCK_H
#define FALLINGBLOCK_H

#include "tetromino.h"
#include "move.h"

class FallingBlock : public Tetromino {
private:
    int x, y;
    bool fixed;

public:
    FallingBlock() = default;
    FallingBlock(int kind, int boardCenterX) : Tetromino(kind) {
        // board 중앙 최상단으로 이동
        x = boardCenterX - getCenterX();
        y = -(getMaxY() + 1);
        fixed = false;
    }

    // shape(상대좌표)에 현재 fallingBlock의 좌표만큼 평행이동시킨 절대좌표 리턴
    vector<Coordinate> getCoordinates() {
        vector<Coordinate> res = getShape();
        for (auto &[cx, cy] : res) {
            cx += x;
            cy += y;
        }

        return res;
    }

    void drop() {
        ++y;
    }

    void put() {
        fixed = true;
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
                // TODO
                break;

            case Move::ROTATE_CW:
                // TODO
                break;

            case Move::HARDDROP:
                // TODO
                break;
        }
    }
};

#endif