//
// Created by HP-15S on 12/29/2025.
//

#ifndef HIVEMIND_H
#define HIVEMIND_H

#include "agent.h"
#include "agentmanager.h"
#include "map.h"
#include "package.h"
#include "packagemanager.h"

class Hivemind {
    Map &map;
    AgentManager &agentManager;
    PackageManager &packageManager;
    int &currentTick;

public:
    Hivemind(Map &map, AgentManager &agentManager, PackageManager &packageManager, int &currentTick);
    ~Hivemind() = default;

    //estimam un profit si un drum pt fiecare tip de agent si decidem care e mai profitabil
    int estimateProfit(Agent *agent, Package *package);
    int estimatePathTicks(Agent *agent, const Position &start, const Position &end, int &battery, int &chargingTicks, bool allowCharging);
    void allocatePackages();
};

#endif //HIVEMIND_H
