//
// Created by HP-15S on 12/29/2025.
//

#ifndef HIVEMIND_H
#define HIVEMIND_H

#include "agent.h"
#include "agentmanager.h"
#include "config.h"
#include "map.h"
#include "package.h"
#include "packagemanager.h"
#include "path.h"
#include <vector>
#include <limits>
#include <algorithm>

class Hivemind {
    Map &map;
    AgentManager &agentManager;
    PackageManager &packageManager;
    int &currentTick;

public:
    Hivemind(Map &map, AgentManager &agentManager, PackageManager &packageManager, int &currentTick);
    ~Hivemind() = default;

    int estimateProfit(Agent *agent, Package *package);
    int estimatePathTicks(Agent *agent, const Position &start, const Position &end, int &battery, int &chargingTicks, bool allowCharging);
    void allocatePackages();
};

#endif //HIVEMIND_H
