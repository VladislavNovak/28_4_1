#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include "utilities.h"
#include "Swimmer.h"

using std::cout;
using std::endl;
using std::vector;

std::mutex asyncLock;

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

void asyncSort(vector<Swimmer*> &swimmers) {
    asyncLock.lock();
    std::sort(swimmers.begin(), swimmers.end(),
              [](Swimmer* &prev, Swimmer* &next) { return prev->getOrder() < next->getOrder(); });
    asyncLock.unlock();
}

void asyncPrint(const vector<Swimmer*> &swimmers) {
    asyncLock.lock();
    cout << " --- Winners table --- " << endl;
    cout << "   place:  swimmer id" << endl;
    for (const auto &swimmer : swimmers) {
        cout << "     " << swimmer->getOrder() << "   :      " << swimmer->getId() << endl;
    }
    asyncLock.unlock();
}

void asyncDelete(vector<Swimmer*> &swimmers) {
    asyncLock.lock();
    for (auto &swimmer : swimmers) {
        delete swimmer;
        swimmer = nullptr;
    }

    swimmers.clear();
    asyncLock.unlock();
}

int main() {
    const double distance = putNumeric({10, 100}, {}, "distance");
    const int MAX_COUNT = 6;
    vector<Swimmer*> swimmers;
    swimmers.reserve(MAX_COUNT);

    for (int i = 0; i < MAX_COUNT; ++i) {
        cout << "Data of #" << i << " swimmer" << endl;
        swimmers.emplace_back(new Swimmer(i, putNumeric({2, 3}, {}, "speed (as meters per second)")));
    }

    // Понадобится для вычисления текущей секунда
    int currentSecond = 0;
    // Понадобится для вычисления порядка победителей
    int counterOrder = 0;
    // Для безопасности ограничим количество секунд которое необходимо для преодоления дистанции
    while (currentSecond < (distance / 1.9)) {
        if (currentSecond == 0) { cout << "  ---- START! ----" << endl; }
        else { cout << " --- Second #" << currentSecond << " ---" << endl; }

        // Создаем отдельный thread, задерживаем на секунду, расчет текущей позиции в заплыве
        asyncPositionPerSec(swimmers, counterOrder, currentSecond, distance);

        if (counterOrder >= MAX_COUNT) { break; }
        ++currentSecond;
    }

    // Здесь - нужно отсортировать массив swimmers
    asyncSort(swimmers);

    // Распечатываем победителей в соответствии с занятым местом
    asyncPrint(swimmers);

    // Удаляем объекты из кучи
    asyncDelete(swimmers);

    return 0;
}
