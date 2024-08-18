#include "tetromino.h"

class HoldedBlock : public Tetromino {
private:
    bool activated;

public:
    HoldedBlock() = default;
    HoldedBlock(int kind) : Tetromino(kind), activated(false) {}
    
    void activate() {
        activated = true;
    }
    
    bool canHold() {
        return activated;
    }
};