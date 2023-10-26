#include "Swimmer.h"

Swimmer::Swimmer(int inId, double inSpeed) { id = inId, mPerSec = inSpeed; }

void Swimmer::setTimeSpent(double distance) { timeSpent = distance / this->mPerSec; }

[[nodiscard]] int Swimmer::getId() const { return id; }

[[nodiscard]] double Swimmer::getSpeed() const { return mPerSec; }

[[nodiscard]] double Swimmer::getTimeSpent() const { return timeSpent; }

[[nodiscard]] bool Swimmer::hasFinish() const { return timeSpent > 0; }
