#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include "utilities.h"

using std::cout;
using std::endl;
using std::vector;

std::mutex asyncLock;

class Swimmer {
    int id{};
    // Количество метров в секунду. Средняя для пловца > 2 && < 3
    double mPerSec;
    int order{};
public:
    explicit Swimmer(int inId, double inSpeed) { id = inId, mPerSec = inSpeed; }
    void setOrder(int inOrder) { order = inOrder; }
    [[nodiscard]] int getId() const { return id; }
    [[nodiscard]] double getSpeed() const { return mPerSec; }
    [[nodiscard]] int getOrder() const { return order; }
};

void asyncPositionPerSec(vector<Swimmer*> &list, int &counterOrder, int currentSecond, int distance) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    asyncLock.lock();
    for (auto* &swimmer : list) {
        if (swimmer->getOrder() > 0) { continue; }

        double position = swimmer->getSpeed() * currentSecond;
        if (currentSecond) { cout << "Swimmer #" << swimmer->getId() << " has overcome " << position << endl; }
        if (position >= distance) {
            swimmer->setOrder(++counterOrder);
            cout << " - and finished with order #" << swimmer->getOrder() << endl;
        }
    }
    asyncLock.unlock();
}

int main() {
    const int DISTANCE = 20;
    const int MAX_COUNT = 6;
    vector<Swimmer*> swimmers;
    swimmers.reserve(MAX_COUNT);

    for (int i = 0; i < MAX_COUNT; ++i) {
        cout << "#" << i << " swimmer" << endl;
        swimmers.emplace_back(new Swimmer(i, putNumeric({2, 3}, {}, "speed (as meter per sec)")));
    }

    // Понадобится для вычисления текущей секунда
    int currentSecond = 0;
    // Понадобится для вычисления порядка победителей
    int counterOrder = 0;
    // Для безопасности ограничим количество секунд которое необходимо для преодоления дистанции
    while (currentSecond < (DISTANCE / 1.9)) {
        cout << " --- Second #" << currentSecond << " ---" << endl;
        asyncPositionPerSec(swimmers, counterOrder, currentSecond, DISTANCE);
        if (counterOrder >= MAX_COUNT) { break; }
        ++currentSecond;
    }

    // Здесь - нужно отсортировать массив swimmers

    asyncLock.lock();
    cout << "Winners table:" << endl;
    for (const auto &swimmer : swimmers) {
        cout << swimmer->getId() << ": " << swimmer->getOrder() << endl;
    }
    asyncLock.unlock();

    asyncLock.lock();
    for (auto &swimmer : swimmers) {
        delete swimmer;
        swimmer = nullptr;
    }

    swimmers.clear();
    asyncLock.unlock();

    return 0;
}
