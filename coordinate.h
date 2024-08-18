enum class Direction {
    COUNTERCLOCKWISE,
    CLOCKWISE
};

struct Coordinate {
    int x, y;
    
    Coordinate() = default;
    Coordinate(int x, int y) : x(x), y(y) {}
    
    void move(int x, int y) {
        this->x += x;
        this->y += y;
    }
    
    void rotate(Direction direction) {
        int tmpX = x;
        int tmpY = y;

        switch (direction) {
            case Direction::CLOCKWISE:
                x = tmpY;
                y = -tmpX;
                break;
            
            case Direction::COUNTERCLOCKWISE:
                x = -tmpY;
                y = tmpX;
                break;
        }
    }
};