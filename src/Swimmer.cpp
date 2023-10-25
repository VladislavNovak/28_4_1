#include "Swimmer.h"

Swimmer::Swimmer(int inId, double inSpeed) { id = inId, mPerSec = inSpeed; }

void Swimmer::setOrder(int inOrder) { order = inOrder; }

[[nodiscard]] int Swimmer::getId() const { return id; }

[[nodiscard]] double Swimmer::getSpeed() const { return mPerSec; }

[[nodiscard]] int Swimmer::getOrder() const { return order; }


