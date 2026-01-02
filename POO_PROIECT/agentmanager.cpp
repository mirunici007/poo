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
        else {
            this->totalOperationsCost += agent->getCost();
        }
    }
}

void AgentManager::moveAgents(const Map &map, PackageManager &packageManager, int currentTick) {
    for (auto &agent: this->agents) {
        if (agent->getCurrentState() != MOVING) {
            continue;
        }

        agent->move(map);

        if (agent->hasReachedClient() && agent->getAssignedPackage()) {
            completedDeliveries.push_back(agent->getPackageId());

            if (agent->getAssignedPackage()->checkLate()) {
                    agent->getAssignedPackage()->markLate();
            }
            agent->getAssignedPackage()->markDelivered();
            packageManager.markDelivered(agent->getPackageId(), currentTick);

            //Package *package = packageManager


            bool canFly = agent->getMovementType() == AIR;
            std::vector<Position> returnPath = PathFinder::getPath(map, agent->getAgentPosition(), basePosition, canFly);

            if (!returnPath.empty()) {
                agent->setPath(returnPath);
                agent->setDestinationPath((returnPath));
                agent->setCurrentState(MOVING);
            }
            else {
                agent->setCurrentState(IDLE);
            }
            agent->setAssignedPackage(nullptr);
            returnPath.clear();
        }
    }
}

//pt alocare pachete
std::vector<Agent *> AgentManager::getFreeAgents() {
    std::vector<Agent *> freeAgents;

    for (auto &agent: this->agents) {
        if (agent->getCurrentState() == IDLE &&
            agent->getAssignedPackage() == nullptr &&
            !agent->isDead()) {
            freeAgents.push_back(agent.get());//de ce cu get?????
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
    agent->setAssignedPackage(package);
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