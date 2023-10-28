#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include "utilities.h"
#include "Swimmer.h"

using std::cout;
using std::endl;
using std::vector;

std::mutex watchSwimmerList;

void sortSwimmers(vector<Swimmer*> &swimmers) {
    watchSwimmerList.lock();
    std::sort(swimmers.begin(), swimmers.end(),
              [](Swimmer* &prev, Swimmer* &next) { return prev->getTimeSpent() < next->getTimeSpent(); });
    watchSwimmerList.unlock();
}

void printResults(const vector<Swimmer*> &swimmers) {
    watchSwimmerList.lock();
    cout << " --- Winners table --- " << endl;
    cout << "  time spent :   id" << endl;

    for (const auto &swimmer : swimmers) {
        cout << std::setw(9) << std::setprecision(2) << swimmer->getTimeSpent();
        cout << "    :    ";
        cout << swimmer->getId() << endl;
    }
    watchSwimmerList.unlock();
}

void clearHeap(vector<Swimmer*> &swimmers) {
    watchSwimmerList.lock();
    for (auto &swimmer : swimmers) {
        delete swimmer;
        swimmer = nullptr;
    }

    swimmers.clear();
    watchSwimmerList.unlock();
}

// Сделано лишь для демонстрации многопоточности
// Для каждой дорожки запускает индивидуальный таймер:
void asyncCountdown(Swimmer* swimmer, double distance) {
    // Понадобится для вычисления текущей секунды
    int currentSecond = 0;

    // Можно просто true, но для безопасности ограничим кол. секунд которое необходимо для преодоления дистанции
    while (currentSecond < (distance / 1.9)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Обратим внимание: внутри mutex не выйти из цикла по break;
        watchSwimmerList.lock();
        double position = swimmer->getSpeed() * currentSecond;

        if (position >= distance) {
            // Установим затраченное спортсменом время на преодоление дистанции
            swimmer->setTimeSpent(distance);
            cout << "(" << currentSecond << ") Swimmer #" << swimmer->getId() << " reaches the finish line.\n";
        } else if (currentSecond) {
            cout << "(" << currentSecond << ") Swimmer #" << swimmer->getId() << " has overcome " << position << "\n";
        }
        watchSwimmerList.unlock();

        if (swimmer->hasFinish() > 0) { return; }

        ++currentSecond;
    }
}

void doSwim(vector<Swimmer*> swimmers, double distance, int swimmersCount) {
    // Создаем шесть параллельных потоков:
    vector<std::thread> threads(swimmersCount);

    for (int i = 0; i < swimmersCount; ++i) {
        // Альтернативные способы создания массива потоков:
        threads.emplace_back(asyncCountdown, swimmers[i], distance);
        // threads.emplace_back([&swimmers, distance, i]() { asyncCountdown(swimmers[i], distance); });
    }

    cout << "START!" << endl;
    // Потоки начинают выполняться параллельно
    for (auto &thread : threads) { if (thread.joinable()) { thread.join(); } }
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
    std::thread threadMain(doSwim, swimmers, distance, SWIMMERS_COUNT);
    if (threadMain.joinable()) { threadMain.join(); }

    // Здесь - сортируем (на месте) массив swimmers
    sortSwimmers(swimmers);

    // Распечатываем победителей в соответствии с занятым местом
    printResults(swimmers);

    // Удаляем объекты из кучи
    clearHeap(swimmers);

    return 0;
}
