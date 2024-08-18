#ifndef BAG_H
#define BAG_H

#include <vector>
#include <queue>
#include <numeric>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

template <int N>
class Bag {
private:
    vector<int> origList;
    vector<int> inventory;

    random_device rd;
    mt19937 engine;

public:
    Bag() : origList(N), engine(rd()) {
        iota(origList.begin(), origList.end(), 1);
    }

    void pack() {
        shuffle(origList.begin(), origList.end(), engine);
        inventory.insert(inventory.end(), origList.begin(), origList.end());
    }

    int takeOut() {
        if (inventory.empty()) pack();
        
        int tmp = inventory.back();
        inventory.pop_back();
        return tmp;
    }
};

#endif