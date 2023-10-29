#ifndef INC_28_4_1_SWIMMER_H
#define INC_28_4_1_SWIMMER_H

class Swimmer {
    int id{};
    // Количество метров в секунду. Средняя для пловца > 2 && < 3
    double mPerSec;
    // Количество времени, затраченное на заплыв. Результат вычисления дистанции / mPerSec
    double timeSpent{};
public:
    explicit Swimmer(int inId, double inSpeed);

    void setTimeSpent(double inTimeSpent);

    [[nodiscard]] int getId() const;

    [[nodiscard]] double getSpeed() const;

    [[nodiscard]] double getTimeSpent() const;

    // true, если пловец финишировал
    [[nodiscard]] bool hasFinish() const;
};

#endif //INC_28_4_1_SWIMMER_H
