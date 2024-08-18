#include "tetromino.h"

class FallingBlock : public Tetromino {
private:
    int x, y;
    
public:
    FallingBlock() = default;
    FallingBlock(int kind, int boardCenterX) : Tetromino(kind) {
        // board 중앙 최상단으로 이동
        x = boardCenterX - getCenterX();
        y = -getMaxY();
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

};