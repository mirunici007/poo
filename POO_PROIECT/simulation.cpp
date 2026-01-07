//
// Created by HP-15S on 12/26/2025.
//
#include "simulation.h"

#include <fstream>
#include "exceptions.h"
using namespace std;

Simulation::Simulation(): currentTick(0), totalProfit(0),
        setup(nullptr), runner(nullptr), report(nullptr),
        packageManager(nullptr), hivemind(nullptr), config(ConfigFile::getInstance()){}

void Simulation::initialize() {
    SimulationSetup::simulationSetup(*this);

    this->runner = new SimulationRun(*this);
    this->report = new SimulationReport(*this);
}
//rularea aplicatiei
void Simulation::runApp() {
    //exceptie daca nu a pornit rularea simularii
    if (!runner) {
    throw SimulationException("nu a putut fi pornita!");
    }

    this->runner->runFullSimulation();
}

//generarea raportului si afisarea statisticilor
void Simulation::generateReport(const string &fileName) {
    //exceptie daca nu se poate genera raportul
    if (!this->report) {
        throw SimulationException("nu a putut genera raportul!");
    }

    this->report->generateReport(fileName);
    this->report->displayStatistics();
}

//afiseaza harta generata
void Simulation::displayMap() {
    cout<<"HARTA GENERATA"<<endl;

    for (int i =0; i < map.getRows(); i++) {
        for (int j = 0; j < map.getCols(); j++) {
            switch (map.getCellRole(i, j)) {
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

//gettere pentru atributele private
Map& Simulation::getMap(){
    return this->map;
}

AgentManager& Simulation::getAgentManager(){
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

    while (!validMap && nrAttempts < 10) {
        mapGenerator.loadMap(map);
        validMap = MapValidator::validateMap(map);
        nrAttempts++;
    }

    //exceptie daca nu s-a generat harta repede(in maxim 10 incercari)
    if (nrAttempts >= 10) {
        throw MapException("nu a putut fi generata!");
    }
    if (validMap){
        cout<<"Harta initializata cu succes!"<<endl;
    }
}

//initializeaza agentii in hub
void SimulationSetup::initializeAgents(AgentManager &agentManager, const ConfigFile &config) {
    agentManager.spawnAgents(config.getNrDrones(), config.getNrRobots(), config.getNrScooters());
    cout<<"Agenti initializati cu succes!"<<endl;
}

//initializeaza pachetele
void SimulationSetup::initializePackages(Simulation &simulation, ConfigFile &config) {
    PackageManager *packageManager = new PackageManager(config.getNrPackages());
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
        }catch (FileException &e) {
            cout<<e.what()<<endl;
        }

        config.printConfig();

        //initializare mapa
        initializeMap(simulation.getMap(), config);

        //initializare agenti
        initializeAgents(simulation.getAgentManager(), config);

        //initializare pachete
        initializePackages(simulation, config);

        //initializare hivemind
        initializeHivemind(simulation);


        cout<<"SIMULARE PREGATITA!!!"<<endl<<endl;
    }catch (ConfigFileException &e) {
        cout<<e.what()<<endl;
        //exceptie daca simularea nu se poate rula
        throw SimulationException("nu poate fi initiata!");
    }
}

//metoda care asigura ca se spawneaza pachete la un anume tick, la o anumita frecventa
void SimulationRun::processSpawning() {
    ConfigFile &config = ConfigFile::getInstance();
    int tick = simulation.getCurrentTick();

    if (tick % config.getSpawnFrequency() == 0) {
        if (simulation.getPackageManager().getNrGeneratedPackages() < config.getNrPackages()) {
            simulation.getPackageManager().spawnPackage(tick);
        }
    }
}

//actualizeaza actiunile agentilor per tick
void SimulationRun::processAgentActions() {
    simulation.getAgentManager().updateAgents(simulation.getMap());
}

//asigura alocarea pachetelor
void SimulationRun::processAllocation() {
    simulation.getHivemind().allocatePackages();
}

//asigura miscarea agentilor
void SimulationRun::processAgentMovements() {
    simulation.getAgentManager().moveAgents(simulation.getMap(), simulation.getPackageManager(), simulation.getCurrentTick());

    //creeaza o lista cu numarul pachetelor expediate
    vector<int> completed = simulation.getAgentManager().getCompletedDeliveries();

    simulation.getAgentManager().clearCompletedDeliveries();
}

//proceseaza respectarea deadline-urilor
void SimulationRun::processDeadlines() {
    simulation.getPackageManager().checkDeadline(simulation.getCurrentTick());
}

//afiseaza progresul pana la un anume tick
void SimulationRun::displayProgress() {
    SimulationReport reportMaker(simulation);
    ConfigFile &config = ConfigFile::getInstance();

    reportMaker.displayProgress(simulation.getCurrentTick(), config.getMaxTicks());
}

//metoda care decide daca trebuie oprita simularea(daca a fost atins numarul maxim de tick-uri si nu mai sunt pachete in pending)
bool SimulationRun::shouldSimulationStop() const {
    ConfigFile &config = ConfigFile::getInstance();
    PackageManager &pm = simulation.getPackageManager();

    int tick = simulation.getCurrentTick();

    if (tick > config.getNrPackages() * config.getSpawnFrequency()) {
        if (simulation.getPackageManager().getNrPendingPackages() == 0) {
            return true;
        }
    }
    return false;
}


SimulationRun::SimulationRun(Simulation &simulation): simulation(simulation){}

//metoda care ruleaza simularea pentru un singur tick
void SimulationRun::runSingleTick() {
    processSpawning();
    processAgentActions();
    processAllocation();
    processAgentMovements();
    processDeadlines();

    //calculeaza profitul
    int profit = simulation.getPackageManager().totalReward() -
            simulation.getPackageManager().totalLatePenalty() -
                simulation.getAgentManager().getTotalOperationsCost();

    simulation.setTotalProfit(profit);

}

//metoda ruleaza toata simularea
void SimulationRun::runFullSimulation() {
    cout<<"Start simulare!"<<endl<<endl;

    ConfigFile &config = ConfigFile::getInstance();

    for (int tick = 0; tick < config.getMaxTicks(); tick++) {
        simulation.setCurrentTick(tick);
        runSingleTick();

        //afiseaza progresul din 50 in 50 de tick-uri
        if (tick % 50 == 0) {
            displayProgress();
        }

        if (shouldSimulationStop()) {
            cout<<"Simulare finalizata cu anticipare!"<<endl;
            break;
        }
    }
    simulation.getPackageManager().movePendingToUndelivered();

    cout<<"Simulare finalizata!"<<endl<<endl;
}

//gettere
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

    //exceptie daca nu s-a putut deschide fisierul
    if (!fileOut.is_open()) {
        throw FileException("nu a putut fi deschis!");
    }

    //adaugarea variabilelor pentru o scriere mai usoara
    ConfigFile &config = ConfigFile::getInstance();
    const Map &map = simulation.getMap();
    PackageManager &pm = simulation.getPackageManager();
    AgentManager &am = simulation.getAgentManager();

    //afisarea statisticilor
    fileOut<<"RAPORT SIMULARE"<<endl<<endl;

    fileOut<<"CONFIGURARI"<<endl;
    fileOut<<"Harta: "<<map.getRows()<<"x"<<map.getCols()<<endl;
    fileOut<<"Durata simularii: "<<simulation.getCurrentTick()<<" ticks"<<endl;
    fileOut<<"Agenti: "<<config.getNrDrones()<<" drone, "<<config.getNrRobots()<<" roboti, "<<config.getNrScooters()<<" scutere"<<endl<<endl;

    fileOut<<"REZULTATE"<<endl<<endl;

    fileOut<<"PACHETE"<<endl;
    fileOut<<"Pachete livrate: "<<pm.getNrDeliveredPackages()<<endl;
    fileOut<<"Pachete livrate cu intarziere: "<<pm.getNrLatePackages()<<endl;
    fileOut<<"Pachete nelivrate: "<<pm.getNrUndeliveredPackages()<<endl<<endl;

    fileOut<<"AGENTI"<<endl;
    int nrAgents = simulation.getConfig().getNrDrones() + simulation.getConfig().getNrRobots() + simulation.getConfig().getNrScooters();
    fileOut<<"Agentii in viata: "<<nrAgents - am.getNrDeadAgents()<<endl;
    fileOut<<"Agenti morti: "<<am.getNrDeadAgents()<<endl<<endl;

    fileOut<<"FINANTE"<<endl;
    fileOut<<"Castiguri: +"<<getTotalReward()<<endl;
    fileOut<<"Costuri de operare: -"<<getTotalCosts()<<endl;

    if (getTotalPenalties() > 0) {
        fileOut<<"Penalizari: -"<< getTotalPenalties()<<endl;
    }
    else {
        fileOut<<"Penalizari: "<< getTotalPenalties()<<endl;
    }

    if (simulation.getTotalProfit() > 0) {
        fileOut<<"PROFIT: +"<<simulation.getTotalProfit()<<endl;
    }
    else {
        fileOut<<"PROFIT: "<<simulation.getTotalProfit()<<endl;
    }

    fileOut<<(simulation.getTotalProfit() > 0 ? "PROFITABIL" : "NEPROFITABIL");
}

//afiseaza informatiile finale
void SimulationReport::displayStatistics() const {
    PackageManager &pm = simulation.getPackageManager();
    AgentManager &am = simulation.getAgentManager();
    //apelam metoda noncost pe obiect const si aveam eroare

    cout<<"~~~RAPORT~~~"<<endl<<endl;

    cout<<"PACHETE"<<endl;
    cout<<"Pachete livrate: "<<pm.getNrDeliveredPackages()<<endl;
    cout<<"Pachete livrate cu intarziere: "<<pm.getNrLatePackages()<<endl;
    cout<<"Pachete nelivrate: "<<pm.getNrUndeliveredPackages()<<endl<<endl;

    cout<<"AGENTI"<<endl;
    int nrAgents = simulation.getConfig().getNrDrones() + simulation.getConfig().getNrRobots() + simulation.getConfig().getNrScooters();
    cout<<"Agentii in viata: "<<nrAgents - am.getNrDeadAgents()<<endl;
    cout<<"Agenti morti: "<<am.getNrDeadAgents()<<endl<<endl;


    cout<<"FINANTE"<<endl;
    cout<<"Castiguri: +"<<getTotalReward()<<endl;
    cout<<"Costuri de operare: -"<<getTotalCosts()<<endl;

    //afiseaza cu "-" daca e mai mare ca 0
    if (getTotalPenalties() > 0) {
        cout<<"Penalizari: -"<< getTotalPenalties()<<endl;
    }
    else {
        cout<<"Penalizari: "<< getTotalPenalties()<<endl;
    }

    if (simulation.getTotalProfit() > 0) {
        cout<<"PROFIT: +"<<simulation.getTotalProfit()<<endl;
        cout<<"PROFITABIL"<<endl;
    }
    else {
        cout<<"PROFIT: "<<simulation.getTotalProfit()<<endl;
        cout<<"NEPROFITABIL"<<endl;
    }
}

//afiseaza progresul la fiecare 50 de tick-uri pe masura ce decurge simularea
void SimulationReport::displayProgress(int currentTick, int maxTicks) const {
    PackageManager &pm = simulation.getPackageManager();
    AgentManager &am = simulation.getAgentManager();

    cout<<"~~~PROGRES~~~"<<endl;
    cout<<"Tick curent: "<<simulation.getCurrentTick()<<endl<<endl;

    cout<<"PACHETE"<<endl;
    cout<<"Pachete livrate: "<<pm.getNrDeliveredPackages()<<endl;
    cout<<"Pachete livrate cu intarziere: "<<pm.getNrLatePackages()<<endl<<endl;

    cout<<"FINANTE"<<endl;
    cout<<"Castiguri: +"<<getTotalReward()<<endl;
    cout<<"Costuri de operare: -"<<getTotalCosts()<<endl;

    //afiseaza cu "-" daca e mai mare ca 0
    if (getTotalPenalties() > 0) {
        cout<<"Penalizari: -"<< getTotalPenalties()<<endl<<endl;
    }
    else {
        cout<<"Penalizari: "<< getTotalPenalties()<<endl<<endl;
    }

    cout<<"AGENTI"<<endl;
    int nrAgents = simulation.getConfig().getNrDrones() + simulation.getConfig().getNrRobots() + simulation.getConfig().getNrScooters();
    cout<<"Agentii in viata: "<<nrAgents - am.getNrDeadAgents()<<endl;
    cout<<"Agenti morti: "<<am.getNrDeadAgents()<<endl<<endl;

}