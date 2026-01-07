//
// Created by HP-15S on 1/7/2026.
//

#ifndef TESTS_H
#define TESTS_H

#include <iostream>
#include <vector>
#include <string>
#include "agent.h"
#include "map.h"
#include "path.h"
#include "package.h"
#include "packagemanager.h"
#include "agentmanager.h"
#include "config.h"

using namespace std;

//functie care sa printeze statusul testului
void testStatus(const string &test, bool passed) {
    cout << "[" << (passed ? "DA" : "NU") << "] " << test << endl << endl;
}

//test pentru generarea procedurala a hartii
void testProceduralMap() {
    cout << "~Test: Generare procedurala a hartii~" << endl;
    ConfigFile &configFile = ConfigFile::getInstance();
    Map testMap(configFile.getRows(), configFile.getColumns(), configFile.getNrClients(), configFile.getMaxStations());
    ProceduralMapGenerator generator;
    generator.loadMap(testMap);

    //verificari minime: daca exista baze, statii, clienti
    int hubs=0, clients=0, stations=0;
    for (int i = 0; i < testMap.getRows(); ++i)
        for (int j = 0; j < testMap.getCols(); ++j)
            switch(testMap.getCellRole(i, j)) {
                case HUB: hubs++; break;
                case DESTINATION: clients++; break;
                case STATION: stations++; break;
                default: break;
            }
    bool ok = (hubs == 1 && clients >= 1 && stations >= 1);
    string text = "date extrase: " + to_string(hubs)+ "/1, "+ to_string(stations) + "/" +
        to_string(configFile.getMaxStations()) + ", " + to_string(clients) + "/" + to_string(configFile.getNrClients());
    testStatus(text, ok);
}

//test pentru verificarea ciclului de viata al agentului: tranzitia intre stari si gestionarea bateriei
void testAgentLifeCycle() {
    cout << "~Test: Testarea ciclului de viata al bateriei robotului~" << endl;
    Drone drone(basePosition);

    //test daca porneste cu bateria maxima
    testStatus("Drona porneste cu bateria incarcata.", drone.getCurrentBattery() == drone.getMaxBattery());

    //test la descarcarea bateriei
    drone.setCurrentBattery(10);
    drone.setCurrentState(MOVING);
    vector<Position> fakePath = { Position(1,1), Position(2,2) };
    drone.setPath(fakePath);
    Map dummyMap;
    drone.actionPerTick(dummyMap);

    testStatus("Bateria dronei se descarca pe masura ce se deplaseaza.", drone.getCurrentBattery() < 10);

    //testam daca drona trece corect in state DEAD
    drone.setCurrentBattery(0);
    drone.setCurrentState(MOVING);
    drone.actionPerTick(dummyMap);
    testStatus("Drona a trecut in state DEAD in afara statiilor sau a bazei.", drone.isDead());
}

//test verificarea alocarii pachetelor si livrarea lor la client
void testDeliveryFlow() {
    cout << "~Test: Testarea livrarii pachetelor~" << endl;
    Map testMap(10,10,2,3);
    ProceduralMapGenerator gen;
    gen.loadMap(testMap);

    //gasim baza
    int hubX = -1, hubY = -1;
    for(int i = 0 ; i < testMap.getRows() ; i++)
        for(int j = 0; j < testMap.getCols(); j++)
            if(testMap.getCellRole(i, j) == HUB) {
                hubX = i;
                hubY = j;
            }

    AgentManager agentManager;
    //alegem cati agenti se spawneaza
    agentManager.spawnAgents(3,2,1);
    PackageManager pkgMan(10);
    pkgMan.setDestinations(testMap);

    pkgMan.spawnPackage(0);
    vector<Package*> pending = pkgMan.getPendingPackages();
    if (pending.empty()) {
        testStatus("~Test: Crearea pachetelor~", false);
        return;
    }

    Package *package = pending[0];
    Agent *agent = agentManager.getFreeAgents()[0];
    try {
        agentManager.assignPackageToAgent(testMap, agent, package, pkgMan);
        testStatus("Pachet atribuit cu succes!", true);
    } catch (...) {
        testStatus("Pachetul nu a putut fi atribuit!", false);
    }
}

//test pentru PathFinder
void testPathfinding() {
    cout << "~Test: Gasirea unui drum valid~" << endl;
    Map map(10,10,2,3);
    map.setCellRole(0, 0, HUB);
    map.setCellRole(0, 9, DESTINATION);
    vector<Position> res = PathFinder::getPath(map, Position(0,0), Position(0,9), false);
    bool ok = !res.empty() && res.back().getPositionY()==9;
    testStatus("Metoda PathFinder returneaza drumul catre destinatie.", ok);
}

//functia care apeleaza toate testele si pe care o apelam in main
inline void runAllTests() {
    testProceduralMap();
    testAgentLifeCycle();
    testDeliveryFlow();
    testPathfinding();
    cout << endl;
}

#endif //TESTS_H
