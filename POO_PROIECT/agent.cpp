//
// Created by HP-15S on 12/23/2025.
//

#include <iostream>

#include "agent.h"
#include <algorithm>

#include "path.h"

using namespace std;

//agent
bool Agent::isDead() const {
    return this->currentState == DEAD;
}

void Agent::actionPerTick(const Map &map) {
    CellRole cell = map.grid[agentPosition.getPositionX()][agentPosition.getPositionY()];

    if (currentBattery <= 0 && cell != CellRole::STATION && cell != CellRole::HUB) {
        currentBattery = 0;
        currentState = DEAD;
    }

    if (cell == CellRole::STATION) {
        currentState = CHARGING;
        return;
    }

    // -- LIVRARE MULTIPLE --
    if (cell == CellRole::DESTINATION && !assignedPackages.empty() && hasReachedClient()) {
        bool deliveredAny = false;
        for (auto it = assignedPackages.begin(); it != assignedPackages.end(); ) {
            if ((*it)->getDestPosition().getPositionX() == agentPosition.getPositionX() &&
                (*it)->getDestPosition().getPositionY() == agentPosition.getPositionY()) {
                (*it)->markDelivered();
                it = assignedPackages.erase(it);
                deliveredAny = true;
            } else {
                ++it;
            }
        }
        // Dacă am livrat, setează path către următoarea destinație optimă dacă există, altfel spre HUB
        if (deliveredAny) {
            if (!assignedPackages.empty()) {
                // ALEGE "următorul" pachet optim fără sortare (deadline apoi distanță)
                Package* bestPkg = nullptr;
                int minDeadline = INT_MAX, minDist = INT_MAX;
                int agentX = agentPosition.getPositionX();
                int agentY = agentPosition.getPositionY();

                for (auto* pkg : assignedPackages) {
                    int dl = pkg->getDeadline();
                    int dist = abs(agentX - pkg->getDestPosition().getPositionX()) +
                               abs(agentY - pkg->getDestPosition().getPositionY());

                    if (dl < minDeadline || (dl == minDeadline && dist < minDist)) {
                        minDeadline = dl;
                        minDist = dist;
                        bestPkg = pkg;
                    }
                }

                if (bestPkg) {
                    std::vector<Position> nextPath = PathFinder::getPath(map, agentPosition, bestPkg->getDestPosition(), getMovementType() == AIR);
                    if (!nextPath.empty()) {
                        setPath(nextPath);
                        targetCellRole = DESTINATION;
                        currentState = MOVING;
                    } else {
                        currentState = IDLE;
                    }
                }
            } else {
                std::vector<Position> basePath = PathFinder::getPath(map, agentPosition, basePosition, getMovementType() == AIR);
                if (!basePath.empty()) {
                    setPath(basePath);
                    targetCellRole = HUB;
                    currentState = MOVING;
                } else {
                    currentState = IDLE;
                }
            }
            return;
        }
    }

    if (cell == CellRole::HUB) {
        currentState = IDLE;
        currentBattery = maxBattery;
        assignedPackages.clear();
        return;
    }

    if (currentState == DEAD) {
        return;
    }

    if (currentState == IDLE) {
        if (currentBattery < maxBattery) {
            currentBattery += maxBattery / 4;
        }
        if (currentBattery > maxBattery) {
            currentBattery = maxBattery;
        }
        return;
    }

    if (currentState == CHARGING) {
        currentBattery += maxBattery / 4;
        if (currentBattery > maxBattery) {
            currentBattery = maxBattery;

            if (!currentPath.empty()) {
                currentState = MOVING;
                currentPath = destinationPath;
            } else {
                currentState = IDLE;
            }
        }
        return;
    }

    if (currentState == MOVING) {
        int necessaryBattery = currentPath.size() * getConsumptionPerTick();

        if (currentBattery < necessaryBattery && targetCellRole != STATION) {
            targetCellRole = STATION;
            Position nearestStation = PathFinder::findNearestStation(map, this);
            std::vector<Position> targetStation = PathFinder::getPath(map, agentPosition, nearestStation, movementType == AIR);

            if (!targetStation.empty()) {
                setPath(targetStation);
            }
        }

        currentBattery -= getConsumptionPerTick();
        move(map);
    }
}

void Agent::setPath(const std::vector<Position> &path) {
    this->currentPath = path;
}

void Agent::setCurrentState(StateMachine state) {
    this->currentState = state;
}

/*void Agent::setAssignedPackage(Package *package) {
    this->assignedPackage = package;
}*/

int Agent::getCost() const {
    return this->cost;
}

int Agent::getPackageId() const {
    if (this->assignedPackages.empty()) {
        return -1;
    }
    return this->assignedPackages.front()->getID();
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
/*Package* Agent::getAssignedPackage() const {
    return this->assignedPackage;
}*/

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
    return this->currentPath.empty() && !assignedPackages.empty();
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

void Agent::addAssignedPackage(Package *package) {
    if (assignedPackages.size() < capacity) {
        assignedPackages.push_back(package);
    }
}

void Agent::removeAssignedPackage(int packageId) {
    assignedPackages.erase(std::remove_if(assignedPackages.begin(), assignedPackages.end(),
            [packageId](Package* p) { return p->getID() == packageId; }),
        assignedPackages.end());
}

std::vector<Package *>& Agent::getAssignedPackages() {
    return this->assignedPackages;
}

int Agent::countAssignedPackages() const {
    return this->assignedPackages.size();
}