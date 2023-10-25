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

void asyncCalcPositions(vector<Swimmer*> &list, int &counterOrder, int currentSecond, double distance) {
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

void sortObjects(vector<Swimmer*> &swimmers) {
    asyncLock.lock();
    std::sort(swimmers.begin(), swimmers.end(),
              [](Swimmer* &prev, Swimmer* &next) { return prev->getOrder() < next->getOrder(); });
    asyncLock.unlock();
}

void printResults(const vector<Swimmer*> &swimmers) {
    asyncLock.lock();
    cout << " --- Winners table --- " << endl;
    cout << "   place:  swimmer id" << endl;
    for (const auto &swimmer : swimmers) {
        cout << "     " << swimmer->getOrder() << "   :      " << swimmer->getId() << endl;
    }
    asyncLock.unlock();
}

void clearHeap(vector<Swimmer*> &swimmers) {
    asyncLock.lock();
    for (auto &swimmer : swimmers) {
        delete swimmer;
        swimmer = nullptr;
    }

    swimmers.clear();
    asyncLock.unlock();
}

void mainSwim(vector<Swimmer*> swimmers, double distance, int swimmersCount) {
    cout << "LOG! Started new thread #" << &std::this_thread::get_id << endl;
    // Понадобится для вычисления текущей секунды
    int currentSecond = 0;
    // Понадобится для вычисления порядка победителей
    int counterOrder = 0;
    // Можно просто true, но для безопасности ограничим кол. секунд которое необходимо для преодоления дистанции
    while (currentSecond < (distance / 1.9)) {
        if (currentSecond == 0) { cout << "  ---- START! ----" << endl; }
        else { cout << " --- Second #" << currentSecond << " ---" << endl; }

        // Создаем отдельный thread, задерживаем на секунду, расчет текущей позиции в заплыве
        asyncCalcPositions(swimmers, counterOrder, currentSecond, distance);

        if (counterOrder >= swimmersCount) { break; }
        ++currentSecond;
    }
    cout << "LOG! Ended thread #" << &std::this_thread::get_id << endl;
}

int main() {
    // Дистанция заплыва. Рекомендуемая: 25
    const double distance = putNumeric({10, 100}, {}, "distance");
    // Количество участников
    const int SWIMMERS_COUNT = 6;

    vector<Swimmer*> swimmers;
    swimmers.reserve(SWIMMERS_COUNT);

    for (int i = 0; i < SWIMMERS_COUNT; ++i) {
        cout << "Data of #" << i << " swimmer" << endl;
        swimmers.emplace_back(new Swimmer(i, putNumeric({2, 3}, {}, "speed (as meters per second)")));
    }

    // Создаём новый thread
    // В цикле отсчитываются секунды по истечению которых идёт по преодоленной дистанции
    std::thread threadCall(mainSwim, swimmers, distance, SWIMMERS_COUNT);
    if (threadCall.joinable()) { threadCall.join(); }

    // Здесь - сортируем (на месте) массив swimmers по полю order
    sortObjects(swimmers);

    // Распечатываем победителей в соответствии с занятым местом
    printResults(swimmers);

    // Удаляем объекты из кучи
    clearHeap(swimmers);

    return 0;
}
