#ifndef HOLDEDBLOCK_H
#define HOLDEDBLOCK_H

#include "tetromino.h"

class HoldedBlock : public Tetromino {
private:
    bool activated;

public:
    HoldedBlock() : Tetromino(), activated(false) {}
    HoldedBlock(int kind) : Tetromino(kind), activated(false) {}
    
    void activate() {
        activated = true;
    }
    
    bool canHold() {
        return activated;
    }
};

#endif