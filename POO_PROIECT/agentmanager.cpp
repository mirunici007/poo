//
// Created by HP-15S on 12/29/2025.
//
#include "agentmanager.h"
#include "path.h"


AgentManager::AgentManager():nrDeadAgents(0), totalOperationsCost(0){}


void AgentManager::spawnAgents(int nrDrones, int nrRobots, int nrScooters) {
    for (int i = 0; i < nrDrones; i++) {
        agents.push_back(AgentFactory::createAgent(Agents::DRONE, basePosition));
    }
    for (int i = 0; i < nrRobots; i++) {
        agents.push_back(AgentFactory::createAgent(Agents::ROBOT, basePosition));
    }
    for (int i = 0; i < nrScooters; i++) {
        agents.push_back(AgentFactory::createAgent(Agents::SCOOTER, basePosition));
    }

}
void AgentManager::updateAgents(const Map &map) {
    for (auto &agent: this->agents) {
        if (agent->isDead()) {
            continue;
        }

        agent->actionPerTick(map);

        if (agent->isDead()) {
            this->nrDeadAgents++;
        }

    }
}

void AgentManager::moveAgents(const Map &map, PackageManager &packageManager, int currentTick) {
    for (auto &agent : agents) {
        if (agent->getCurrentState() != MOVING)
            continue;

        agent->move(map);
        this->totalOperationsCost += agent->getCost();

        // Dacă agentul a ajuns la client și are cel puțin un pachet de livrat:
        if (agent->hasReachedClient() && agent->countAssignedPackages() > 0) {
            Position agentPos = agent->getAgentPosition();
            std::vector<Package*>& pkgs = agent->getAssignedPackages();
            bool deliveredAny = false;

            // Livrăm TOATE pachetele potrivite cu destinația curentă
            for (auto it = pkgs.begin(); it != pkgs.end(); ) {
                if ((*it)->getDestPosition().getPositionX() == agentPos.getPositionX() &&
                    (*it)->getDestPosition().getPositionY() == agentPos.getPositionY()) {

                    completedDeliveries.push_back((*it)->getID());

                    if ((*it)->checkLate())
                        (*it)->markLate();

                    (*it)->markDelivered();
                    packageManager.markDelivered((*it)->getID(), currentTick);

                    it = pkgs.erase(it);
                    deliveredAny = true;
                } else {
                    ++it;
                }
            }

            // Dacă mai sunt pachete de livrat, găsește direct pachetul cel mai apropiat (fără sort)
            if (!pkgs.empty()) {
                Package* nearestPkg = nullptr;
                int minDist = INT_MAX;
                for (auto* pkg : pkgs) {
                    int dist = abs(agentPos.getPositionX() - pkg->getDestPosition().getPositionX()) +
                               abs(agentPos.getPositionY() - pkg->getDestPosition().getPositionY());
                    if (dist < minDist) {
                        minDist = dist;
                        nearestPkg = pkg;
                    }
                }

                if (nearestPkg) {
                    bool canFly = agent->getMovementType() == AIR;
                    std::vector<Position> nextPath = PathFinder::getPath(map, agentPos, nearestPkg->getDestPosition(), canFly);
                    if (!nextPath.empty()) {
                        agent->setPath(nextPath);
                        agent->setDestinationPath(nextPath);
                        agent->setCurrentState(MOVING);
                    } else {
                        agent->setCurrentState(IDLE);
                    }
                } else {
                    agent->setCurrentState(IDLE);
                }
            } else {
                // Nu mai are pachete, se întoarce la bază (HUB)
                bool canFly = agent->getMovementType() == AIR;
                std::vector<Position> returnPath = PathFinder::getPath(map, agentPos, basePosition, canFly);

                if (!returnPath.empty()) {
                    agent->setPath(returnPath);
                    agent->setDestinationPath(returnPath);
                    agent->setCurrentState(MOVING);
                } else {
                    agent->setCurrentState(IDLE);
                }
            }
        }
    }
}

//pt alocare pachete
std::vector<Agent *> AgentManager::getFreeAgents() {
    std::vector<Agent *> freeAgents;

    for (auto &agent: this->agents) {
        if (agent->getCurrentState() == IDLE &&
            agent->countAssignedPackages() == 0 &&
            !agent->isDead()) {

            freeAgents.push_back(agent.get());
        }
    }

    return freeAgents;
}
void AgentManager::assignPackageToAgent(const Map &map, Agent *agent, Package *package, PackageManager &packageManager) {
    if (!agent || !package) {
        //eroareeeeeeeeeeeeeeeee
        return;
    }

    Position startPath = agent->getAgentPosition();
    Position endPath = package->getDestPosition();

    bool canFly = agent->getMovementType() == AIR;
    std::vector<Position> goodPath = PathFinder::getPath(map, startPath, endPath, canFly);

    if (goodPath.empty()) {
        //eroareeeeeeeeeeeeeeeeeeeee
        return;
    }

    agent->setCurrentState(MOVING);
    agent->setPath(goodPath);
    agent->setDestinationPath(goodPath);
    agent->addAssignedPackage(package);
    packageManager.addBeingDelivered(package);

}

//gettere
int AgentManager::getTotalOperationsCost() const {
    return this->totalOperationsCost;
}
int AgentManager::getNrDeadAgents() const {
    return this->nrDeadAgents;
}

int AgentManager::totalDeadAgentsCost() const {
    return this->nrDeadAgents * 500;
}

std::vector<int> AgentManager::getCompletedDeliveries() const {
    return this->completedDeliveries;
}
void AgentManager::clearCompletedDeliveries() {
    this->completedDeliveries.clear();
}