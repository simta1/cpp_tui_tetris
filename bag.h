#include <vector>
#include <queue>
#include <numeric>
#include <cstdlib>
#include <ctime>
using namespace std;

// TODO 7-bag 시스템 추가
template <int N>
class Bag {
private:

public:
    Bag() {
        srand(time(0));
    }

    int takeOut() {
        return rand() % N + 1;
    }
};