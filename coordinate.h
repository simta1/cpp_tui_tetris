enum class Direction {
    COUNTERCLOCKWISE,
    CLOCKWISE
};

struct Coordinate {
    int x, y;
    
    Coordinate(int x, int y) : x(x), y(y) {}
    
    Coordinate move(int x, int y) {
        this->x += x;
        this->y += y;
    }
    
    void rotate(Direction direction) {
        switch (direction) {
            case Direction::COUNTERCLOCKWISE:
                int tmpX = x;
                int tmpY = y;
                x = tmpY;
                y = -tmpX;
                break;
            
            case Direction::CLOCKWISE:
                int tmpX = x;
                int tmpY = y;
                x = -tmpY;
                y = tmpX;
                break;
        }
    }
};