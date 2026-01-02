#include <iostream>
#include <cstdlib>
#include <ctime>
#include "map.h"
#include "package.h"

using namespace std;
#include "config.h"
#include "simulation.h"

int main() {
    srand(time(NULL));

    try {
        Simulation simulation;
        simulation.initialize();
        simulation.displayMap();
        simulation.runApp();
        simulation.generateReport();

    }catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}