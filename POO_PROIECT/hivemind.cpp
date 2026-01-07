//
// Created by HP-15S on 12/29/2025.
//

#include "hivemind.h"
#include "path.h"
#include <limits>
#include <algorithm>
#include "exceptions.h"

using namespace std;

Hivemind::Hivemind(Map &map, AgentManager &agentManager,PackageManager &packageManager, int &currentTick): map(map), agentManager(agentManager), packageManager(packageManager), currentTick(currentTick) {}

int Hivemind::estimateProfit(Agent *agent, Package *package) {
    //clonam agentul pentru a simula drumul catre destinatie
    Agent *simAgent = agent->clone();

    int battery = agent->getCurrentBattery();
    int totalTicks = 0;
    int chargeTicks = 0;

    int toClient = estimatePathTicks(simAgent, simAgent->getAgentPosition(), package->getDestPosition(), battery, chargeTicks, true);

    if (toClient < 0) {
        delete simAgent;
        return -1;
    }

    //toti pasii dus-intors
    totalTicks += toClient + chargeTicks;
    int deliveryTick = currentTick + totalTicks;

    int penalty;
    if (deliveryTick > package->getDeadline()) {
        penalty = 50;
    }


    //drumul inapoi la hub
    int backCharge = 0;
    int backToBase = estimatePathTicks(simAgent, package->getDestPosition(), basePosition, battery, backCharge, true);

    if (backToBase < 0) {
        delete simAgent;
        return -1;
    }

    totalTicks += backToBase + backCharge;

    int cost = agent->getConsumptionPerTick() * totalTicks;
    int profit = package->getReward() - cost - penalty;

    //stergem clona
    delete simAgent;

    //returnam profitul estimativ al agentului
    return profit;
}

int Hivemind::estimatePathTicks(Agent *agent, const Position &start, const Position &end, int &battery, int &chargingTicks, bool allowCharging) {
    std::vector<Position> path = PathFinder::getPath(map, start, end, agent->getMovementType() == AIR);

    //exceptie daca nu exista drum de la agent la destinatie
    if (path.empty()) {
        throw PathException("start", "destinatie");
    }

    int distance = path.size();
    int speed = agent->getSpeed();
    int moveTicks = (distance + speed - 1) / speed;
    int necessaryBattery = moveTicks * agent->getConsumptionPerTick();

    chargingTicks = 0;

    //daca nu putem incarca, nu e un agent potrivit
    if (battery < necessaryBattery) {
        if (!allowCharging) {
            return -1;
        }

        //estimam tick-urile minime necesare pt a se incarca agentul suficient
        //dupa el se intoarce la statie pt a se incarca ca sa ajunga la baza
        int remainingBatteryNedeed = necessaryBattery - battery;
        int chargePerTick = agent->getMaxBattery() / 4;

        int ticksToCharge = (remainingBatteryNedeed + chargePerTick -1) / chargePerTick;

        chargingTicks += ticksToCharge;
        battery += ticksToCharge * chargePerTick;
    }

    battery -= necessaryBattery;
    //daca ramane fara baterie agentul moare
    if (battery < 0) {
        return -1;
    }
    return moveTicks;
}

void Hivemind::allocatePackages() {
    auto &pendingPackages = packageManager.getPendingPackages();
    //sorteaza pachetele dupa deadline(prioritate au pachetele care expira cel mai repede)
    std::sort(pendingPackages.begin(), pendingPackages.end(), [](Package* a, Package* b) {
        return a->getDeadline() < b->getDeadline();
    });

    //ia agentii liberi pt a atribui pachete
    auto agents = agentManager.getFreeAgents();

    for (auto it = pendingPackages.begin(); it != pendingPackages.end();) {
        Package* package = *it;
        Agent* bestAgent = nullptr;
        int minTicks = INT_MAX;

        //cauta agentul care poate livra cel mai rapid pachetul
        for (auto* agent : agents) {
            int battery = agent->getCurrentBattery();
            int chargingTicks = 0;
            int ticks = estimatePathTicks(agent,
                            agent->getAgentPosition(),
                            package->getDestPosition(),
                            battery, chargingTicks, true);

            if (ticks >= 0 && ticks < minTicks) {
                minTicks = ticks;
                bestAgent = agent;
            }
        }

        //daca am gasit un agent, atribuie si scoate pachetul din pending
        if (bestAgent && (minTicks <= package->getDeadline())) {
            agentManager.assignPackageToAgent(map, bestAgent, package, packageManager);
            agents.erase(std::remove(agents.begin(), agents.end(), bestAgent), agents.end());
            it = pendingPackages.erase(it); //scoate pachetul din vector
        }
        else {
            ++it; //ramane neatribuit pentru ca niciun agent nu il poate lua
        }
    }
}