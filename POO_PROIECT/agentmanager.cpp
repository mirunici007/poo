//
// Created by HP-15S on 12/29/2025.
//
#include "agentmanager.h"
#include "path.h"
#include "exceptions.h"


AgentManager::AgentManager():nrDeadAgents(0), totalOperationsCost(0){}


//metoda pentru crearea agentilor si adaugarea lor in 3 liste separate
void AgentManager::spawnAgents(int nrDrones, int nrRobots, int nrScooters) {
    //vectorul de drone
    for (int i = 0; i < nrDrones; i++) {
        agents.push_back(AgentFactory::createAgent(Agents::DRONE, basePosition));
    }
    //vectorul de roboti
    for (int i = 0; i < nrRobots; i++) {
        agents.push_back(AgentFactory::createAgent(Agents::ROBOT, basePosition));
    }
    //vectorul de scutere
    for (int i = 0; i < nrScooters; i++) {
        agents.push_back(AgentFactory::createAgent(Agents::SCOOTER, basePosition));
    }

}

//metoda pentru actualizarea actiunilor agentilor
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

//metoda pentru deplasarea agentilor pe harta
void AgentManager::moveAgents(const Map &map, PackageManager &packageManager, int currentTick) {
    for (auto &agent : agents) {
        if (agent->getCurrentState() == MOVING) {
            this->totalOperationsCost += agent->getCost();
        }
        if (agent->getCurrentState() != MOVING)
            continue;

        agent->move(map);
        
        //daca agentul a ajuns la client si are cel cel putin un pachet de livrat
        if (agent->hasReachedClient() && agent->countAssignedPackages() > 0) {
            Position agentPos = agent->getAgentPosition();
            std::vector<Package*>& packages = agent->getAssignedPackages();

            // agentul livreaza toate pachetele potrivite cu destinatia curenta
            for (auto package = packages.begin(); package != packages.end(); ) {
                if ((*package)->getDestPosition().getPositionX() == agentPos.getPositionX() &&
                    (*package)->getDestPosition().getPositionY() == agentPos.getPositionY()) {

                    completedDeliveries.push_back((*package)->getID());

                    if ((*package)->checkLate()) {
                        (*package)->markLate();
                    }

                    (*package)->markDelivered();
                    packageManager.markDelivered((*package)->getID(), currentTick);
                    package = packages.erase(package);
                }
                else {
                    ++package;
                }
            }

            //daca mai sunt pachete de livrat, gaseste pachetul cel mai apropiat de livrat
            if (!packages.empty()) {
                Package* nearestPackage = nullptr;
                int minDistance = INT_MAX;
                for (auto* package : packages) {
                    int distance = PathFinder::getPath(map, agentPos, package->getDestPosition(), agent->getMovementType() == AIR).size();
                    if (distance < minDistance) {
                        minDistance = distance;
                        nearestPackage = package;
                    }
                }

                //atribuim agentului pachetul pt a-l livra
                if (nearestPackage) {
                    bool canFly = agent->getMovementType() == AIR;
                    //vectorul contine drumul catre hub
                    std::vector<Position> nextPath = PathFinder::getPath(map, agentPos, nearestPackage->getDestPosition(), canFly);
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
                //nu mai are pachete si se întoarce la hub
                bool canFly = agent->getMovementType() == AIR;
                std::vector<Position> returnPath = PathFinder::getPath(map, agentPos, basePosition, canFly);

                //daca are drum, se duce la baza
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

//metoda care returneaza agentii ce nu au alocat niciun pachet
std::vector<Agent *> AgentManager::getFreeAgents() {
    std::vector<Agent *> freeAgents;

    //e liber daca e iddle, nu e mort si nu are pachete atribuite
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
    //exceptie: daca nu se da un agent valid
    if (!agent) {
        throw AgentException(" pointer este null la alocare in Agentmanager!\n");
    }

    //exceptie daca nu se da un pachet valid
    if (!package) {
        throw PackageException(" pointer este null la alocare in Agentmanager!\n");
    }

    Position startPath = agent->getAgentPosition();
    Position endPath = package->getDestPosition();

    //drumul de la agent catre destinatie, adica catre client
    bool canFly = agent->getMovementType() == AIR;
    std::vector<Position> goodPath = PathFinder::getPath(map, startPath, endPath, canFly);

    //exceptie daca nu exista drum de la agent la client
    if (goodPath.empty()) {
        throw PathException("agent", "destinatie");
    }

    //pregatim agentul de plecare
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