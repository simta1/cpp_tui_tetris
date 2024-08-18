class Timer {
private:
    int elapsedTime, endTime;
    bool repeat;
    
    void work() {
        elapsedTime++;
    }
    
public:
    Timer() = default;
    Timer(int endTime, bool repeat) : endTime(endTime), repeat(repeat) {
        init();
    }

    void init() {
        elapsedTime = 0;
    }
    
    bool isOver() {
        work();
        if (elapsedTime < endTime) return false;
        if (repeat) elapsedTime = 0;
        return true;
    }
    
    int getTime() {
        return elapsedTime;
    }
};