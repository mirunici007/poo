//
// Created by HP-15S on 12/23/2025.
//

#include <iostream>

#include "agent.h"

#include "path.h"

using namespace std;

//agent
bool Agent::isDead() const {
    return this->currentState == DEAD;
}

void Agent::actionPerTick(const Map &map) {

    CellRole cell = map.grid[this->agentPosition.getPositionX()][this->agentPosition.getPositionY()];

    if (this->currentBattery <= 0 && cell != CellRole::STATION && cell != CellRole::HUB) {
        this->currentBattery = 0;
        this->currentState = DEAD;
    }

    if (cell == CellRole::STATION) {
        this->currentState = CHARGING;
        return;
    }

    if (cell == CellRole::DESTINATION && this->hasReachedClient()&& this->assignedPackage != nullptr) {
        this->assignedPackage->markDelivered();
        this->assignedPackage = nullptr;
        std::vector<Position> basePath = PathFinder::getPath(map, this->agentPosition, basePosition, this->getMovementType() == AIR);

        if (!basePath.empty()) {
            this->setPath(basePath);
            this->targetCellRole = CellRole::HUB;
            this->currentState = MOVING;
        }
        else {
            //+eroareeeeeeeeeeeee
        }
        return;
    }

    if (cell == CellRole::HUB) {
        this->currentState = IDLE;
        this->currentBattery = this->maxBattery;
        this->assignedPackage = nullptr;
        return;
    }

    if (this->currentState == DEAD) {
        return;
    }

    if (this->currentState == IDLE) {
        if (this->currentBattery < this->maxBattery) {
            this->currentBattery += this->maxBattery / 4;
        }
        if (this->currentBattery > this->maxBattery) {
            this->currentBattery = this->maxBattery;
        }
        return;
    }

    if (this->currentState == CHARGING) {
        this->currentBattery += this->maxBattery / 4;
        if (this->currentBattery > this->maxBattery) {
            this->currentBattery = this->maxBattery;

            if (!this->currentPath.empty()) {
                this->currentState = MOVING;
                this->currentPath = this->destinationPath;
            }
            else {
                this->currentState = IDLE;
                //this->targetCellRole = HUB;
            }
        }

        return;
    }

    if (this->currentState == MOVING) {
        int necessaryBattery = this->currentPath.size() * this->getConsumptionPerTick();

        if (this->currentBattery < necessaryBattery && this->targetCellRole != STATION) {
            this->targetCellRole = STATION;
            Position nearestStation = PathFinder::findNearestStation(map, this);
            std::vector<Position> targetStation = PathFinder::getPath(map, this->agentPosition, nearestStation, this->movementType == AIR);

            if (!targetStation.empty()) {
                this->setPath(targetStation);
            }
        }

        this->currentBattery -= this->getConsumptionPerTick();
        this->move(map);
    }
}

void Agent::setPath(const std::vector<Position> &path) {
    this->currentPath = path;
}

void Agent::setCurrentState(StateMachine state) {
    this->currentState = state;
}

void Agent::setAssignedPackage(Package *package) {
    this->assignedPackage = package;
}

int Agent::getCost() const {
    return this->cost;
}

int Agent::getPackageId() const {
    if (this->assignedPackage == nullptr) {
        return -1;
    }
    return this->assignedPackage->getID();
}


StateMachine Agent::getCurrentState() const {
    return this->currentState;
}

Position Agent::getAgentPosition() const {
    return this->agentPosition;
}

MovementType Agent::getMovementType() const {
    return this->movementType;
}
Package* Agent::getAssignedPackage() const {
    return this->assignedPackage;
}

int Agent::getMaxBattery() const {
    return this->maxBattery;
}

int Agent::getCurrentBattery() const {
    return this->currentBattery;
}

int Agent::getSpeed() const {
    return this->speed;
}

int Agent::getConsumptionPerTick() const {
    return this->consumptionPerTick;
}



void Agent::move(const Map &map) {
    if (this->currentState != MOVING || this->currentPath.empty()) {
        return;
    }

    for (int i = 0; i < this->speed && !this->currentPath.empty(); i++) {
        this->agentPosition = this->currentPath.front();
        this->currentPath.erase(this->currentPath.begin());
    }
}

bool Agent::hasReachedClient() const {
    return this->currentPath.empty() && assignedPackage != nullptr;
}

std::unique_ptr<Agent> AgentFactory::createAgent(Agents agentType, Position initPosition) {
    switch (agentType) {
        case Agents::DRONE: return std::make_unique<Drone>(initPosition);
        case Agents::ROBOT: return make_unique<Robot>(initPosition);
        case Agents::SCOOTER: return make_unique<Scooter>(initPosition);
        default: return nullptr;
    }
}


//drona
Drone::Drone(Position basePosition) {
    this->speed = 3;
    this->maxBattery = 100;
    this->currentBattery = 100;
    this->consumptionPerTick = 10;
    this->cost = 15;
    this->capacity = 1;

    this->simbol = '^';

    this->agentPosition = basePosition;
    this->currentState = IDLE;
    this->movementType = AIR;
}

string Drone::getName()const{
    return "Drona";
}


//robot
Robot::Robot(Position basePosition) {
    this->speed = 1;
    this->maxBattery = 300;
    this->currentBattery = 300;
    this->consumptionPerTick = 2;
    this->cost = 1;
    this->capacity = 4;

    this->simbol = 'R';

    this->agentPosition = basePosition;
    this->currentState = IDLE;
    this->movementType = GROUND;
}

string Robot::getName()const {
    return "Robot";
}


//scuter
Scooter::Scooter(Position basePosition) {
    this->speed = 2;
    this->maxBattery = 200;
    this->currentBattery = 200;
    this->consumptionPerTick = 5;
    this->cost = 4;
    this->capacity = 2;

    this->simbol = 'S';

    this->agentPosition = basePosition;
    this->currentState = IDLE;
    this->movementType = GROUND;
}

string Scooter::getName()const {
    return "Scooter";
}

void Agent::setAgentDestination(const Position &position) {
    this->agentPosition = position;
}
void Agent::setCurrentBattery(const int &battery) {
    this->currentBattery = battery;
}

void Agent::setDestinationPath(const std::vector<Position> &path) {
    this->destinationPath = path;
}