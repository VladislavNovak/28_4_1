#ifndef INC_28_4_1_SWIMMER_H
#define INC_28_4_1_SWIMMER_H

class Swimmer {
    int id{};
    // Количество метров в секунду. Средняя для пловца > 2 && < 3
    double mPerSec;
    // Место в зачёте
    int order{};
public:
    explicit Swimmer(int inId, double inSpeed);

    void setOrder(int inOrder);

    [[nodiscard]] int getId() const;

    [[nodiscard]] double getSpeed() const;

    [[nodiscard]] int getOrder() const;
};

#endif //INC_28_4_1_SWIMMER_H
