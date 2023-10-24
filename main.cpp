#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "utilities.h"

using std::cout;
using std::endl;
using std::vector;

class Swimmer {
    int id;
    int speed;
public:
    explicit Swimmer(int inId, int inSpeed) { id = inId, speed = inSpeed; }
    [[nodiscard]] int getId() const { return id; }
    [[nodiscard]] int getSpeed() const { return speed; }
};

int main() {
    const int MAX_COUNT = 6;
    vector<Swimmer*> swimmers;
    swimmers.reserve(MAX_COUNT);

    for (int i = 0; i < MAX_COUNT; ++i) {
        cout << "#" << i << "swimmer" << endl;
        int speed = putNumeric({1, 100}, {}, "speed");
        swimmers.emplace_back(new Swimmer(i, speed));
    }

    // Тело выполнения

    for (auto &swimmer : swimmers) {
        delete swimmer;
        swimmer = nullptr;
    }

    swimmers.clear();

    return 0;
}
