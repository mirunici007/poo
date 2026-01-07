#include <iostream>
#include <cstdlib>
#include <ctime>
#include "config.h"
#include "exceptions.h"
#include "simulation.h"
#include "tests.h"
using namespace std;


int main() {
    //pentru generarea de fiecare data a unui numar random pentru reward pe pachet sau coordonatele clientilor
    srand(time(NULL));

    //facem testele pentru a ne asigura ca totul merge cum trebuie
    cout<<"~~~TESTE~~~"<<endl;
    runAllTests();

    //daca ceva nu merge, afiseaza eroare
    try {
        Simulation simulation;
        simulation.initialize();
        simulation.displayMap();
        simulation.runApp();
        simulation.generateReport();

    }catch (const ConfigFileException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }catch (const FileException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }catch (const MapException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }catch (const AgentException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }catch (const PackageException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }catch (const PathException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }catch (const SimulationException &e) {
        cerr<<e.what()<<endl;
        return 1;
    }

    return 0;
}
