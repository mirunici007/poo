//
// Created by HP-15S on 12/24/2025.
//

#ifndef PACKAGE_H
#define PACKAGE_H

#include "config.h"


class Position {
    int positionX;
    int positionY;

public:
    Position(int baseX, int baseY) : positionX(baseX), positionY(baseY) {}
    ~Position() = default;

    void setPosition(int x, int y);

    int getPositionX() const;
    int getPositionY() const;
};

enum StatusPackage {
    PENDING,
    IN_TRANSIT,
    DELIVERED
};

extern Position basePosition;

class Package {
    Position destPosition;
    StatusPackage status;

    int id;
    int reward;
    int deadline;
    int currentTick;
    int spawnTick;

    bool late;
    bool delivered;
    bool isLateReported;

    public:
    Package(int packageId, Position destPosition);
    ~Package() = default;

    void markDelivered();
    void markLate();
    bool checkLate();

    int getID()const;
    Position getDestPosition() const;
    int getReward()const;
    int getDeadline() const;
    bool isDelivered() const;
    bool isLate() const;

    void setSpawnTick(int spawnTick);
    void setCurrentTick(int currentTick);
};

#endif //PACKAGE_H
