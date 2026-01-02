//
// Created by HP-15S on 12/26/2025.
//
#include "simulation.h"

#include <fstream>
using namespace std;

Simulation::Simulation(): currentTick(0), totalProfit(0),
        setup(nullptr), runner(nullptr), report(nullptr),
        packageManager(nullptr), hivemind(nullptr), config(ConfigFile::getInstance()){}

/*Simulation::~Simulation() {
    delete this->packageManager;
    delete this->hivemind;
    delete this->setup;
    delete this->runner;
    delete this->report;
}*/

void Simulation::initialize() {
    SimulationSetup::simulationSetup(*this);

    this->runner = new SimulationRun(*this);
    this->report = new SimulationReport(*this);
}
void Simulation::runApp() {
    if (!runner) {
        //eroareeeeeee
        int i;
    }

    this->runner->runFullSimulation();
}
void Simulation::generateReport(const string &fileName) {
    if (!this->report) {
        //eroareeeeeeeeeeeee
        int i;
    }

    this->report->generateReport(fileName);
    this->report->displayStatistics();
}
void Simulation::displayMap() {
    cout<<"HARTA GENERATA"<<endl;

    for (int i =0; i < map.rows; i++) {
        for (int j = 0; j < map.columns; j++) {
            switch (map.grid[i][j]) {
                case SPACE: cout<<". "; break;
                case WALL: cout<<"# "; break;
                case HUB: cout<<"B "; break;
                case STATION: cout<<"S "; break;
                case DESTINATION: cout<<"D "; break;
                default: cout<<"/ "; break;
            }
        }
        cout<<endl;
    }
    cout<<endl;

    cout<<"Legenda:"<<endl;
    cout<<"[.] DRUM     [#] ZID     [B] BAZA     [S] STATIE     [D] CLIENT"<<endl<<endl;
}

Map& Simulation::getMap() const{
    return this->map;
}

AgentManager& Simulation::getAgentManager() const{
    return this->agentManager;
}

PackageManager& Simulation::getPackageManager() const{
    return *this->packageManager;
}

Hivemind& Simulation::getHivemind() const{
    return *this->hivemind;
}

ConfigFile& Simulation::getConfig() const{
    return this->config;
}

int Simulation::getCurrentTick() const{
    return this->currentTick;
}

int Simulation::getTotalProfit() const{
    return this->totalProfit;
}

void Simulation::setCurrentTick(int tick) {
    this->currentTick = tick;
}

void Simulation::setTotalProfit(int profit) {
    this->totalProfit = profit;
}


void SimulationSetup::initializeMap(Map &map, const ConfigFile &config) {
    ProceduralMapGenerator mapGenerator;

    bool validMap = false;
    int nrAttempts = 0;

    while (!validMap) {
        Map::clearVectors();

        mapGenerator.loadMap(map);
        validMap = MapValidator::validateMap(map);
        nrAttempts++;
        //POATE SA PUN UN NR MAXIM DE INCERCARI
    }
    if (validMap){
        cout<<"Harta initializata cu succes!"<<endl;
    }
}

void SimulationSetup::initializeAgents(AgentManager &agentManager, const ConfigFile &config) {
    agentManager.spawnAgents(config.nrDrones, config.nrRobots, config.nrScooters);
}

void SimulationSetup::initializePackages(Simulation &simulation, ConfigFile &config) {
    PackageManager *packageManager = new PackageManager(config.nrPackages);
    packageManager->setDestinations(simulation.getMap());
    if (packageManager) {
        cout<<"Pachete initializate cu succes!"<<endl;
    }

    simulation.packageManager = packageManager;
}

void SimulationSetup::initializeHivemind(Simulation &simulation) {
    simulation.hivemind = new Hivemind(simulation.getMap(), simulation.getAgentManager(), simulation.getPackageManager(), simulation.currentTick);
    if (simulation.hivemind) {
        cout<<"Hivemind initializat"<<endl;
    }
}

void SimulationSetup::simulationSetup(Simulation &simulation) {
    cout<<"INITIALIZARE"<<endl<<endl;

    try {
        ConfigFile &config = simulation.getConfig();

        try {
            config.loadFromFile("simulation_setup.txt");

            cout<<"Datele din fisier au fost citite cu succes!"<<endl;
        }catch (const std:: exception& e) {
            cout<<e.what()<<endl;
        }

        config.printConfig();

        //initializare mapa
        initializeMap(simulation.getMap(), config);


        //initializare agenti
        initializeAgents(simulation.getAgentManager(), config);
        cout<<"Agenti initializati cu succes!"<<endl;

        //initializare pachete
        initializePackages(simulation, config);


        //initializare hivemind
        initializeHivemind(simulation);


        cout<<"sIMULARE PREGATITA!!!"<<endl<<endl;
    }catch (const std:: exception& e) {
        cout<<e.what()<<endl;
        throw;
    }
}


void SimulationRun::processSpawning() {
    ConfigFile &config = ConfigFile::getInstance();
    int tick = simulation.getCurrentTick();

    if (tick % config.spawnFrequency == 0) {
        if (simulation.getPackageManager().getNrGeneratedPackages() < config.nrPackages) {
            simulation.getPackageManager().spawnPackage(tick);
        }

    }
}
void SimulationRun::processAgentActions() {
    simulation.getAgentManager().updateAgents(simulation.getMap());
}

void SimulationRun::processAllocation() {
    simulation.getHivemind().allocatePackages();

}

void SimulationRun::processAgentMovements() {
    simulation.getAgentManager().moveAgents(simulation.getMap(), simulation.getPackageManager(), simulation.getCurrentTick());

    vector<int> completed = simulation.getAgentManager().getCompletedDeliveries();

    simulation.getAgentManager().clearCompletedDeliveries();
}

void SimulationRun::processDeadlines() {
    simulation.getPackageManager().checkDeadline(simulation.getCurrentTick());
}

void SimulationRun::displayProgress() {
    SimulationReport reportMaker(simulation);
    ConfigFile &config = ConfigFile::getInstance();

    reportMaker.displayProgress(simulation.getCurrentTick(), config.maxTicks);
}

bool SimulationRun::shouldSimulationStop() const {
    ConfigFile &config = ConfigFile::getInstance();
    PackageManager &pm = simulation.getPackageManager();

    int tick = simulation.getCurrentTick();

    if (tick > config.nrPackages * config.spawnFrequency) {
        if (simulation.getPackageManager().getNrPendingPackages() == 0) {
            return true;
        }
    }
    return false;
}

SimulationRun::SimulationRun(Simulation &simulation): simulation(simulation){}

void SimulationRun::runSingleTick() {
    processSpawning();
    processAgentActions();
    processAllocation();
    processAgentMovements();
    processDeadlines();

    int profit = simulation.getPackageManager().totalReward() -
            simulation.getPackageManager().totalLatePenalty() -
                simulation.getAgentManager().getTotalOperationsCost();

    simulation.setTotalProfit(profit);


}
void SimulationRun::runFullSimulation() {
    cout<<"Start simulare!!!!!!!!!!!!!!"<<endl<<endl;

    ConfigFile &config = ConfigFile::getInstance();

    for (int tick = 0; tick < config.maxTicks; tick++) {
        simulation.setCurrentTick(tick);
        runSingleTick();

        if (tick % 50 == 0) {
            displayProgress();
        }

        if (shouldSimulationStop()) {
            cout<<"Simulare finalizata cu anticipare!!!"<<endl;
            break;
        }
    }

    cout<<"Simulare finlazitaa!!!!"<<endl<<endl;
}


int SimulationReport::getTotalReward() const {
    return simulation.getPackageManager().totalReward();
}

int SimulationReport::getTotalCosts() const {
    return simulation.getAgentManager().getTotalOperationsCost();
}

int SimulationReport::getTotalPenalties() const {
    return simulation.getPackageManager().totalLatePenalty() + simulation.getPackageManager().totalUndeliveredPenalty() + simulation.getAgentManager().totalDeadAgentsCost();
}

SimulationReport::SimulationReport(Simulation &simulation): simulation(simulation){}

void SimulationReport::generateReport(const std::string &filename) {
    ofstream fileOut(filename);
    if (!fileOut.is_open()) {
        //eroareeeeeeeeeeeeeeeeeee
        return;
    }

    ConfigFile &config = ConfigFile::getInstance();
    const Map &map = simulation.getMap();
    PackageManager &pm = simulation.getPackageManager();
    AgentManager &am = simulation.getAgentManager();

    fileOut<<"RAPORT SIMULARE"<<endl<<endl;

    fileOut<<"CONFIGURARI"<<endl;
    fileOut<<"Harta: "<<map.rows<<"x"<<map.columns<<endl;
    fileOut<<"Durata simularii: "<<simulation.getCurrentTick()<<" ticks"<<endl;
    fileOut<<"Agenti: "<<config.nrDrones<<" drone, "<<config.nrRobots<<" roboti, "<<config.nrScooters<<" scutere"<<endl<<endl;

    fileOut<<"REZULTATE"<<endl;
    fileOut<<"Pachete livrate: "<<pm.getNrDeliveredPackages()<<endl;
    fileOut<<"Pachete livrate cu intarziere: "<<pm.getNrLatePackages()<<endl;
    fileOut<<"Pachete nelivrate: "<<pm.getNrUndeliveredPackages()<<endl;
    fileOut<<"Agenti morti: "<<am.getNrDeadAgents()<<endl;

    fileOut<<"FINANTE"<<endl<<endl;
    fileOut<<"Castiguri: +"<<getTotalReward()<<endl;
    fileOut<<"Costuri de operare: -"<<getTotalCosts()<<endl;
    fileOut<<"Penalizari: -"<< getTotalPenalties()<<endl;
    fileOut<<"PROFIT: "<<simulation.getTotalProfit()<<endl;
    fileOut<<(simulation.getTotalProfit() > 0 ? "PROFITABIL" : "NEPROFITABIL");
}
void SimulationReport::displayStatistics() const {
    PackageManager &pm = simulation.getPackageManager();
    AgentManager &am = simulation.getAgentManager();
    //apelam metoda noncost pe obiect const si aveam eroare!!!!!!!!!!!!!!!

    cout<<"RAPORT"<<endl;

    cout<<"Pachete livrate: "<<pm.getNrDeliveredPackages()<<endl;
    cout<<"Pachete livrate cu intarziere: "<<pm.getNrLatePackages()<<endl;
    cout<<"Pachete nelivrate: "<<pm.getNrUndeliveredPackages()<<endl;
    cout<<"Rewards: "<<simulation.getPackageManager().totalReward()<<endl;
    cout<<"Agenti morti: "<<am.getNrDeadAgents()<<endl;
    cout<<"Castiguri: "<<getTotalReward()<<endl;
    cout<<"Costuri de operare: -"<<getTotalCosts()<<endl;
    cout<<"Penalizari: "<< getTotalPenalties()<<endl;
    cout<<"PROFIT: "<<simulation.getTotalProfit()<<endl;
}
void SimulationReport::displayProgress(int currentTick, int maxTicks) const {
    PackageManager &pm = simulation.getPackageManager();
    AgentManager &am = simulation.getAgentManager();

    int profit = getTotalReward() - getTotalCosts() - getTotalPenalties();

    cout<<"Progres: "<<endl;
    cout<<"Tick curent: "<<simulation.getCurrentTick()<<endl;
    cout<<"Profit curent: "<<profit<<endl;
    cout<<"Pachete livrate: "<<pm.getNrDeliveredPackages()<<endl;
    cout<<"Pachete livrate cu intarziere: "<<pm.getNrLatePackages()<<endl;
    cout<<"Pachete in curs de livrare: "<<pm.getNrPendingPackages()<<endl;
    cout<<"Agenti morti: "<<am.getNrDeadAgents()<<endl;
    cout<<"Rewards: "<<simulation.getPackageManager().totalReward()<<endl;
    cout<<"Castiguri: "<<getTotalReward()<<endl;
    cout<<"Costuri de operare: -"<<getTotalCosts()<<endl;
    cout<<"Penalizari: "<< getTotalPenalties()<<endl<<endl;
    
}