#ifndef COORDINATE_H
#define COORDINATE_H

#include "rotateDirection.h"

struct Coordinate {
    int x, y;
    
    Coordinate() = default;
    Coordinate(int x, int y) : x(x), y(y) {}
    
    void move(int x, int y) {
        this->x += x;
        this->y += y;
    }
    
    void rotate(RotateDirection direction) {
        int tmpX = x;
        int tmpY = y;

        switch (direction) {
            case RotateDirection::CLOCKWISE:
                x = -tmpY;
                y = tmpX;
                break;
            
            case RotateDirection::COUNTERCLOCKWISE:
                x = tmpY;
                y = -tmpX;
                break;
        }
    }
};

#endif