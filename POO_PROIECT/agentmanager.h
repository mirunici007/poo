//
// Created by HP-15S on 12/29/2025.
//

#ifndef AGENTMANAGER_H
#define AGENTMANAGER_H

#include "agent.h"
#include "package.h"
#include "packagemanager.h"
#include "map.h"

#include <vector>
#include <memory>

class AgentManager {

    std::vector<int> completedDeliveries;

    int nrDeadAgents;
    int totalOperationsCost;

public:
    std::vector<std::unique_ptr<Agent>> agents;
    AgentManager();
    ~AgentManager() = default;

    void spawnAgents(int nrDrones, int nrRobots, int nrScooters);
    void updateAgents(const Map &map);
    void moveAgents(const Map &map, PackageManager &packageManager, int currentTick);

    //pt alocare pachete
    std::vector<Agent *> getFreeAgents();
    void assignPackageToAgent(const Map &map, Agent *agent, Package *package, PackageManager &packageManager);

    //gettere
    int getTotalOperationsCost() const;
    int getNrDeadAgents() const;

    int totalDeadAgentsCost() const;

    std::vector<int> getCompletedDeliveries() const;
    void clearCompletedDeliveries();
};

#endif //AGENTMANAGER_H
