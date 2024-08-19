#ifndef HOLDEDBLOCK_H
#define HOLDEDBLOCK_H

#include "tetromino.h"

class HoldedBlock : public Tetromino {
private:
    bool activated;

public:
    HoldedBlock() : Tetromino(), activated(false) {}
    explicit HoldedBlock(int kind) : Tetromino(kind), activated(false) {}
    
    void activate() {
        activated = true;
    }
    
    bool canHold() const {
        return activated;
    }
};

#endif