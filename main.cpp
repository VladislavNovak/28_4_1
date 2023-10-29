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

// Счетчик спортсменов, которые достигли финиша
int finisherCounter = 0;
// Флаг о достижении всеми спортсменами финиша
bool isFinish = false;

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
    cout << "   reached   :   id" << endl;

    for (const auto &swimmer : swimmers) {
        cout << std::setw(9) << std::setprecision(3) << swimmer->getTimeSpent();
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
    // Понадобится для отображения текущей секунды и вычисления положения спортсмена на дистанции
    int currentSecond = 0;

    while (!isFinish) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Обратим внимание: внутри mutex не выйти из цикла по break;
        watchSwimmerList.lock();
        double position = swimmer->getSpeed() * currentSecond;

        // Если достигнут финиш, то:
        if (position >= distance) {
            cout << "(" << currentSecond << ") Swimmer #" << swimmer->getId() << " reaches the finish line.\n";
            // Установим затраченное спортсменом время на преодоление дистанции
            swimmer->setTimeSpent(distance / swimmer->getSpeed());

            ++finisherCounter;
        } else if (currentSecond) {
            cout << "(" << currentSecond << ") Swimmer #" << swimmer->getId() << " has overcome " << position << "\n";
        }
        watchSwimmerList.unlock();

        // Если ранее был достигнут финиш, проверяем это и если true, то этот цикл завершаем
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

    // Будет постоянно работать, пока все спортсмены не достигнут финиша
    // ВАЖНО: выполняется параллельно с потоками threads спортсменов. Зависит от них и влияет на каждый из них
    while (finisherCounter < swimmersCount) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Если все спортсмены достигли финиша:
    isFinish = true;

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
