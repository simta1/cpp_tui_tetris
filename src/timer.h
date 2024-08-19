#ifndef TIMER_H
#define TIMER_H

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

    void end() {
        elapsedTime = endTime;
    }
    
    bool isOver() {
        work();
        if (elapsedTime < endTime) return false;
        if (repeat) elapsedTime = 0;
        return true;
    }
    
    int getTime() const {
        return elapsedTime;
    }

    void speedUp(double rate, int lowerLimit=1) {
        endTime = max(lowerLimit, int(endTime * rate));
    }
};

#endif