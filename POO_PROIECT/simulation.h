//
// Created by HP-15S on 12/26/2025.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include "agent.h"
#include "agentmanager.h"
#include "config.h"
#include "map.h"
#include "package.h"
#include "packagemanager.h"
#include "path.h"
#include "hivemind.h"
#include <vector>

class SimulationReport;
class SimulationRun;
class SimulationSetup;
//facade pattern
class Simulation {
    mutable Map map;
    mutable AgentManager agentManager;
    PackageManager *packageManager;
    Hivemind *hivemind;
    ConfigFile &config;

    mutable int currentTick;
    mutable int totalProfit;

    SimulationSetup *setup;
    SimulationRun *runner;
    SimulationReport *report;

    friend class SimulationSetup;
public:
    Simulation();
    ~Simulation() = default;

    void initialize();
    void runApp();
    void generateReport(const std::string &fileName="simulation.txt");
    void displayMap();

    //gettere pt raport
    Map &getMap()const ;
    AgentManager &getAgentManager() const;
    PackageManager &getPackageManager() const ;
    Hivemind &getHivemind() const;
    ConfigFile &getConfig() const;
    int getCurrentTick() const;
    int getTotalProfit() const ;

    void setCurrentTick(int tick);
    void setTotalProfit(int profit);
};


//single responsibility principle pattern????
class SimulationSetup {
    static void initializeMap(Map &map, const ConfigFile &config);
    static void initializeAgents(AgentManager &agentManager, const ConfigFile &config);
    static void initializePackages(Simulation &simulation, ConfigFile &config);
    static void initializeHivemind(Simulation &simulation);
public:
    static void simulationSetup(Simulation &simulation);
};

class SimulationRun {
    Simulation &simulation;

    void processSpawning();
    void processAgentActions();
    void processAllocation();
    void processAgentMovements();
    void processDeadlines();

    void displayProgress();
    bool shouldSimulationStop() const;

public:
    SimulationRun(Simulation &simulation);

    void runSingleTick();
    void runFullSimulation();
};

class SimulationReport {
    Simulation &simulation;

    int getTotalReward() const;
    int getTotalCosts() const;
    int getTotalPenalties() const;

public:
    SimulationReport(Simulation &simulation);

    void generateReport(const std::string &filename = "simualtion.txt");
    void displayStatistics() const;
    void displayProgress(int currentTick, int maxTicks) const;
};

#endif //SIMULATION_H
