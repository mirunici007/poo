//
// Created by HP-15S on 12/24/2025.
//
#include <iostream>
#include "config.h"
#include "package.h"

Position basePosition(0,0);


//gettere clasa Position
int Position::getPositionX() const {
    return this->positionX;
}

int Position::getPositionY() const {
    return this->positionY;
}

//setter clasa Position
void Position::setPosition(int x, int y) {
    this->positionX = x;
    this->positionY = y;
}


Package::Package(int packageId, Position destPosition): id(packageId), destPosition(destPosition), status(PENDING), isLateReported(false), delivered(false), late(false) {
    //generam random un castig si deadline
    this->reward = 200 + rand() % 601;
    this->deadline = 10+ rand() % 11;

    this->late = false;
    this->delivered = false;
}


//gettere clasa Package
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


void Package::markDelivered() {
    this->delivered = true;
}

void Package::markLate() {
    this->late = true;
}

bool Package::checkLate() {
    if (this->isLateReported) {
        return false;
    }

    if (this->currentTick - this->spawnTick > this->deadline) {
        this->late = true;
        this->isLateReported = true;
        this->delivered = true;
        return true;
    }
    return false;
}


//settere
void Package::setSpawnTick(int spawnTick) {
    this->spawnTick = spawnTick;
}

void Package::setCurrentTick(int currentTick) {
    this->currentTick = currentTick;
}

