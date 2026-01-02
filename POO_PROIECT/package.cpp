//
// Created by HP-15S on 12/24/2025.
//
#include <iostream>

#include "package.h"

Position basePosition(0,0);

Package::Package(int packageId, Position destPosition): id(packageId), destPosition(destPosition), status(PENDING), isLateReported(false), delivered(false), late(false) {
    this->reward = 200 + rand() % 601;
    this->deadline = 10+ rand() % 11;

    this->late = false;
    this->delivered = false;
}

void Position::setPosition(int x, int y) {
    this->positionX = x;
    this->positionY = y;
}

int Position::getPositionX() const {
    return positionX;
}

int Position::getPositionY() const {
    return positionY;
}


void Package::markDelivered() {
    this->delivered = true;
}
void Package::markLate() {
    this->late = true;
}
bool Package::checkLate() {
    if (isLateReported) {
        return false;
    }

    if (currentTick - spawnTick > deadline) {
        late = true;
        isLateReported = true;
        delivered = true;
        return true;
    }
    return false;
}

int Package::getID() const {
    return this->id;
}

Position Package::getDestPosition() const {
    return this->destPosition;
}

int Package::getReward() const {
    return this->reward;
}

int Package::getDeadline() const {
    return this->deadline;
}

bool Package::isDelivered() const {
    return this->delivered;
}

bool Package::isLate() const {
    return this->late;
}


void Package::setSpawnTick(int spawnTick) {
    this->spawnTick = spawnTick;
}

void Package::setCurrentTick(int currentTick) {
    this->currentTick = currentTick;
}

