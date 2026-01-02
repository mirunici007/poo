//
// Created by HP-15S on 12/29/2025.
//

#include "hivemind.h"
#include <limits>
#include <algorithm>

using namespace std;

Hivemind::Hivemind(Map &map, AgentManager &agentManager,PackageManager &packageManager, int &currentTick): map(map), agentManager(agentManager), packageManager(packageManager), currentTick(currentTick) {}

int Hivemind::estimateProfit(Agent *agent, Package *package) {
    Agent *simAgent = agent->clone();

    int battery = agent->getCurrentBattery();
    int totalTicks = 0;
    int chargeTicks = 0;

    int toClient = estimatePathTicks(simAgent, simAgent->getAgentPosition(), package->getDestPosition(), battery, chargeTicks, true);

    if (toClient < 0) {
        delete simAgent;
        return -1;
    }

    totalTicks += toClient + chargeTicks;
    int deliveryTick = currentTick + totalTicks;

    int penalty;
    if (deliveryTick > package->getDeadline()) {
        penalty = 50;
    }


    int backCharge = 0;
    int backToBase = estimatePathTicks(simAgent, package->getDestPosition(), basePosition, battery, backCharge, true);

    if (backToBase < 0) {
        delete simAgent;
        return -1;
    }

    totalTicks += backToBase + backCharge;

    int cost = agent->getConsumptionPerTick() * totalTicks;
    int profit = package->getReward() - cost;

    delete simAgent;

    return profit;
}

int Hivemind::estimatePathTicks(Agent *agent, const Position &start, const Position &end, int &battery, int &chargingTicks, bool allowCharging) {
    std::vector<Position> path = PathFinder::getPath(map, start, end, agent->getMovementType() == AIR);
    if (path.empty()) {
        return -1;
        //+eorareeeeeeeeeeeeeee
    }

    int distance = path.size();
    int speed = agent->getSpeed();
    int moveTicks = (distance + speed - 1) / speed;
    int necessaryBattery = moveTicks * agent->getConsumptionPerTick();

    chargingTicks = 0;

    if (battery < necessaryBattery) {
        if (!allowCharging) {
            return -1;
        }

        int remainingBatteryNedeed = necessaryBattery - battery;
        int chargePerTick = agent->getMaxBattery() / 4;

        int ticksToCharge = (remainingBatteryNedeed + chargePerTick -1) / chargePerTick;

        chargingTicks += ticksToCharge;
        battery += ticksToCharge * chargePerTick;
    }

    battery -= necessaryBattery;
    if (battery < 0) {
        return -1;
    }
    return moveTicks;
}

void Hivemind::allocatePackages() {
    auto agents = agentManager.getFreeAgents();

    while (true) {
        Package* packageToAssign = packageManager.getNextPackage();
        if (!packageToAssign) {
            break;
        }

        int bestProfit = INT_MIN;
        Agent *bestAgent = nullptr;

        for (auto agent: agents) {
            if (agent->isDead() || agent->getAssignedPackage()) {
                continue;
            }

            int profit = estimateProfit(agent, packageToAssign);
            if (profit > bestProfit) {
                bestProfit = profit;
                bestAgent = agent;
            }
        }

        if (bestAgent && bestProfit ) {
            agentManager.assignPackageToAgent(map, bestAgent, packageToAssign, packageManager);
            agents.erase(std::remove(agents.begin(), agents.end(), bestAgent), agents.end());
        } else {
            packageManager.getPendingPackages().push_back(packageToAssign);
        }
    }
}