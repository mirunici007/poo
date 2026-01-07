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

//le declaram pentru casa Simulation
class SimulationReport;
class SimulationRun;
class SimulationSetup;

//facade pattern pentru a lucra cu o singura clasa(Simulation) si nu cu 3(cele mentionate mai sus)
class Simulation {
    Map map;
    AgentManager agentManager;
    PackageManager *packageManager;
    Hivemind *hivemind;
    ConfigFile &config;

    int currentTick;
    int totalProfit;

    SimulationSetup *setup;
    SimulationRun *runner;
    SimulationReport *report;

    //pentru a accesa usor atributele clasei din SimulationReport
    friend class SimulationSetup;
public:
    Simulation();
    ~Simulation() = default;

    //metode pentru functionarea simularii
    void initialize();
    void runApp();
    void generateReport(const std::string &fileName="simulation.txt");
    void displayMap();

    //gettere pt raport
    //metodele nu sunt const pentru ca modifica indirect valorile pe care le returneaza
    //(in alte metode care apeleaza getterul respectiv)
    //le putem face const, dar atributele trebuie sa fie mutable
    Map &getMap() ;
    AgentManager &getAgentManager();
    PackageManager &getPackageManager() const ;
    Hivemind &getHivemind() const;
    ConfigFile &getConfig() const;
    int getCurrentTick() const;
    int getTotalProfit() const ;

    void setCurrentTick(int tick);
    void setTotalProfit(int profit);
};

//clasa se ocupa de initializarea simularii
class SimulationSetup {
    static void initializeMap(Map &map, const ConfigFile &config);
    static void initializeAgents(AgentManager &agentManager, const ConfigFile &config);
    static void initializePackages(Simulation &simulation, ConfigFile &config);
    static void initializeHivemind(Simulation &simulation);
public:
    static void simulationSetup(Simulation &simulation);
};

//clasa se ocupa de rularea propriu-zisa a simularii
class SimulationRun {
    Simulation &simulation;

    //metodele proceseaza operatii de baza, alocare, spawn, deadline-uri
    void processSpawning();
    void processAgentActions();
    void processAllocation();
    void processAgentMovements();
    void processDeadlines();

    void displayProgress();
    bool shouldSimulationStop() const;

public:
    SimulationRun(Simulation &simulation);

    //metoda ruleaza simularea pt un singur tick
    void runSingleTick();

    //metoda ajuta la rularea intregii simulari
    void runFullSimulation();
};

class SimulationReport {
    Simulation &simulation;

    //gettere care returneaza castigurile, penalizarile
    int getTotalReward() const;
    int getTotalCosts() const;
    int getTotalPenalties() const;

public:
    SimulationReport(Simulation &simulation);

    //metode care genereaza statistici ale simularii cu informatii despre agenti, pachete si finante
    void generateReport(const std::string &filename = "simualtion.txt");
    void displayStatistics() const;
    void displayProgress(int currentTick, int maxTicks) const;
};

#endif //SIMULATION_H
