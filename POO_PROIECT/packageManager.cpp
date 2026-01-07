//
// Created by HP-15S on 12/26/2025.
//
#include "packagemanager.h"
#include "exceptions.h"
#include <iostream>

using namespace std;


PackageManager::PackageManager(int nrMax): maxPackages(nrMax), nrGeneratedPackages(0) {}
//initializarea valorii statice in afara clasei
int PackageManager::nextPackageID = 1;

void PackageManager::setDestinations(const Map &map) {
    //goleste vectorul de clienti
    destinationPositions.clear();

    //reface vectorul de clienti
    for (int i = 0; i < map.getRows(); i++) {
        for (int j = 0; j < map.getCols(); j++) {
            if (map.getCellRole(i, j) == DESTINATION) {
                destinationPositions.push_back(Position(i, j));
            }
        }
    }
    //exceptie daca nu exista clienti
    if (destinationPositions.empty()) {
        throw ClientException();
    }

}

bool PackageManager::spawnPackage(int currentTick) {
    //exceptie daca nu exista clienti pe harta
    if (destinationPositions.empty()) {
        throw ClientException();
    }

    if (nrGeneratedPackages >maxPackages) {
        return false;
    }

    //adauga o noua destinatie pentru pachet si il genereaza pe pozitia respectiva
    int index = rand() % destinationPositions.size();
    Position newDestination = destinationPositions[index];

    Package* newPackage = new Package(nextPackageID++, newDestination);
    newPackage->setCurrentTick(currentTick);
    newPackage->setSpawnTick(currentTick);

    //pune pachetul pe lista de asteptare
    pendingPackages.push_back(newPackage);
    nrGeneratedPackages++;
    nrCurrentPackages++;

    return true;
}


void PackageManager::markDelivered(int packageId, int currentTick) {
    int index = 0;
    //parcurge vectorul de pachete care trebuie livrate
    for (auto *package: beingDeliveredPackages) {
        if (package->getID() == packageId) {
            package->setCurrentTick(currentTick);
            package->markDelivered();

            //daca are intarziere il pune pe lista pachetelor livrate cu intarziere
            if (package->checkLate()) {
                latePackages.push_back(package);
            }
            deliveredPackages.push_back(package);

            //sterge pacgetul din lista daca a fost livrat
            beingDeliveredPackages.erase(beingDeliveredPackages.begin() + index);
            nrCurrentPackages--;
            return;
        }
        index++;
    }
}


void PackageManager::checkDeadline(int currentTick) {
    auto it = beingDeliveredPackages.begin();
    //parcurge pachetele care sunt in proces de livrare
    while (it != beingDeliveredPackages.end()) {
        Package* package = *it;
        package->setCurrentTick(currentTick);

        //daca e intarziat il adauga in lista cu pachetle livrate cu intarziere
        if (package->checkLate()) {
            latePackages.push_back(package);
            it = beingDeliveredPackages.erase(it);
            nrCurrentPackages--;
        }
        else {
            it++;
        }
    }
}


Package* PackageManager::getNextPackage(){
    //daca e goala returneaza null
    if (pendingPackages.empty()) {
        return nullptr;
    }

    //returneaza urmatorul pachet
    Package* package = pendingPackages.front();
    pendingPackages.erase(pendingPackages.begin());
    addBeingDelivered(package);

    return package;
}

int PackageManager::getNrPendingPackages()const {
    return this->pendingPackages.size();
}

int PackageManager::getNrDeliveredPackages()const {
    return this->deliveredPackages.size();
}

int PackageManager::getNrLatePackages()const {
    return this->latePackages.size();
}
int PackageManager::getNrUndeliveredPackages()const {
    return this->undeliveredPackages.size();
}

int PackageManager::totalReward() const {
    int total = 0;

    //calculeaza totalul rewardurilor indiferent daca a ajuns la timp sau nu
    //in alta functie se scad penalizarile
    for (auto package: this->deliveredPackages) {
        total += package->getReward();
    }
    for (auto package: this->latePackages) {
        total += package->getReward();
    }

    return total;
}
int PackageManager::totalUndeliveredPenalty() const {
    return this->undeliveredPackages.size() * 200;
}
int PackageManager::totalLatePenalty() const {
    return this->latePackages.size() * 50;
}

std::vector<Package *>& PackageManager::getPendingPackages() {
    return this->pendingPackages;
}

void PackageManager::addBeingDelivered(Package* package) {
    for (auto p : beingDeliveredPackages) {
        if (p->getID() == package->getID()) {
            return;
        }
    }
    beingDeliveredPackages.push_back(package);
}

int PackageManager::getNrGeneratedPackages()const {
    return this->nrGeneratedPackages;
}

int Package::getSpawnTick()const {
    return this->spawnTick;
}

void PackageManager::movePendingToUndelivered() {
    //daca pachetele nu au fost livrate sunt trecute ca pachete nelivrate
    while (!pendingPackages.empty()) {
        Package *package = pendingPackages.back();
        pendingPackages.pop_back();
        undeliveredPackages.push_back(package);
    }
}
