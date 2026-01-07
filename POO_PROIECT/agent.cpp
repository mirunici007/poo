//
// Created by HP-15S on 12/23/2025.
//

#include <iostream>
#include <algorithm>
#include "agent.h"
#include "exceptions.h"
#include "path.h"

using namespace std;

//metodele clasei agent

//metode de get
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

char Agent::getsimbol() const {
    return this->simbol;
}

int Agent::getCapacity()const {
    return this->capacity;
}

bool Agent::isDead() const {
    return this->currentState == DEAD;
}


//metode de set
void Agent::setCurrentBattery(const int &battery) {
    this->currentBattery = battery;
}

void Agent::setDestinationPath(const std::vector<Position> &path) {
    this->destinationPath = path;
}

void Agent::setPath(const std::vector<Position> &path) {
    this->currentPath = path;
}

void Agent::setCurrentState(StateMachine state) {
    this->currentState = state;
}


//metode de adaugare si stergere de pachete din assignedPackages
void Agent::addAssignedPackage(Package *package) {
    //exceptie daca pachetul este null la adaugare
    if (!package) {
        throw AgentException("a primit pachet null la adaugare!\n");
    }
    if (assignedPackages.size() < capacity) {
        assignedPackages.push_back(package);
    }
}

void Agent::removeAssignedPackage(int packageId) {
    assignedPackages.erase(std::remove_if(assignedPackages.begin(), assignedPackages.end(),
            [packageId](Package* p) { return p->getID() == packageId; }),
        assignedPackages.end());
}


//celelalte metode ale clasei agent
std::vector<Package *>& Agent::getAssignedPackages() {
    return this->assignedPackages;
}

int Agent::countAssignedPackages() const {
    return this->assignedPackages.size();
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

void Agent::actionPerTick(const Map &map) {
    //rolul pozitiei pe care se afla agentul
    CellRole cell = map.getCellRole(agentPosition.getPositionX(), agentPosition.getPositionY());

    //verificare ca robotul e mort daca are bateria<=0 si nu e intr-o statie de incarcare
    if (currentBattery <= 0 && cell != CellRole::STATION && cell != CellRole::HUB) {
        this->currentBattery = 0;
        this->currentState = DEAD;
    }

    if (cell == CellRole::STATION) {
        this->currentState = CHARGING;
        return;
    }

    //livrari multiple
    if (cell == CellRole::DESTINATION && !assignedPackages.empty() && hasReachedClient()) {
        bool deliveredAny = false;
        for (auto package = assignedPackages.begin(); package != assignedPackages.end(); ) {
            //verificam daca agentul a ajuns la destinatia pachetului, adica la client
            if ((*package)->getDestPosition().getPositionX() == agentPosition.getPositionX() &&
                (*package)->getDestPosition().getPositionY() == agentPosition.getPositionY()) {
                (*package)->markDelivered();
                package = assignedPackages.erase(package);
                deliveredAny = true;
            }
            //altfel trecem la urmatorul pachet
            else {
                ++package;
            }
        }
        //daca a livrat pachetul seteaza path catre urmatorul client daca exista, altfel se duce la hub
        if (deliveredAny) {
            if (!assignedPackages.empty()) {
                //alege urmatorul pachet dupa deadline apoi distanta
                Package* bestPackage = nullptr;
                int minDeadline = INT_MAX;
                int minDist = INT_MAX;

                for (auto* package : assignedPackages) {
                    int deadline = package->getDeadline();
                    //calculam distanta de la agent la packet
                    int dist = PathFinder::getPath(map, agentPosition, package->getDestPosition(), getMovementType() == AIR).size();

                    //selectam pachetul cu cel mai mic deadline
                    if (deadline < minDeadline || (deadline == minDeadline && dist < minDist)) {
                        minDeadline = deadline;
                        minDist = dist;
                        bestPackage = package;
                    }
                }

                if (bestPackage) {
                    std::vector<Position> nextPath = PathFinder::getPath(map, agentPosition, bestPackage->getDestPosition(), getMovementType() == AIR);
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

    //modificam atributele agentului pt pozitia hub
    if (cell == CellRole::HUB) {
        currentState = IDLE;
        currentBattery = maxBattery;
        assignedPackages.clear();
        return;
    }

    //daca e mort nu face nimic
    if (currentState == DEAD) {
        return;
    }

    //daca e in hub, se incarca automat
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

    //daca se misca cosnuma baterie si cauta o statie daca nu are suficienta baterie pana la destiantie
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


//constructor pentru clasa drona
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


//constructor pentru clasa robot
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


//constructor pentru clasa scuter
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


//implementare pt factory pattern
std::unique_ptr<Agent> AgentFactory::createAgent(Agents agentType, Position initPosition) {
    switch (agentType) {
        case Agents::DRONE: return std::make_unique<Drone>(initPosition);
        case Agents::ROBOT: return make_unique<Robot>(initPosition);
        case Agents::SCOOTER: return make_unique<Scooter>(initPosition);
        default: return nullptr;
    }
}