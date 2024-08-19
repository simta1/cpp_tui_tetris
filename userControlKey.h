#ifndef USERCONTROLKEY_H
#define USERCONTROLKEY_H

#include <string>
#include <vector>
#define KEYBOARD_ARROW_UP 72
#define KEYBOARD_ARROW_LEFT 75
#define KEYBOARD_ARROW_RIGHT 77
#define KEYBOARD_ARROW_DOWN 80
using namespace std;

// -------------------------------------------------------- 사용자 지정 ------------------------------------------------------------
const char KEY_PAUSE_GAME = 'p';
const char KEY_REPLAY_GAME = 'r';
const char KEY_MOVE_LEFT = 'a';
const char KEY_MOVE_RIGHT = 'd';
const char KEY_DROP_SOFT = 's';
const char KEY_ROTATE_LEFT = KEYBOARD_ARROW_LEFT;
const char KEY_ROTATE_RIGHT = KEYBOARD_ARROW_RIGHT;
const char KEY_DROP_HARD = KEYBOARD_ARROW_UP;
const char KEY_HOlD = ' ';
// --------------------------------------------------------------------------------------------------------------------------------

string convertKeyToString(char key) {
    switch (key) {
        case 72: return "UP";
        case 75: return "LEFT";
        case 77: return "RIGHT";
        case 80: return "DOWN";
        case ' ': return "SPACE";
    }
    
    return string(1, key);
}

const vector<string> USER_CONTROL_KEYS = {
    "pause   : " + convertKeyToString(KEY_PAUSE_GAME),
    "moveL   : " + convertKeyToString(KEY_MOVE_LEFT),
    "moveR   : " + convertKeyToString(KEY_MOVE_RIGHT),
    "moveD   : " + convertKeyToString(KEY_DROP_SOFT),
    "rotateL : " + convertKeyToString(KEY_ROTATE_LEFT),
    "rotateR : " + convertKeyToString(KEY_ROTATE_RIGHT),
    "drop    : " + convertKeyToString(KEY_DROP_HARD),
    "hold    : " + convertKeyToString(KEY_HOlD)
};

constexpr int USER_CONTROL_KEY_COUNT = 8; // replay 제외

#endif