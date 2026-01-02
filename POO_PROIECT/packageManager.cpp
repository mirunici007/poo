//
// Created by HP-15S on 12/26/2025.
//
#include "packagemanager.h"
using namespace std;


PackageManager::PackageManager(int nrMax): maxPackages(nrMax), nrGeneratedPackages(0) {}
int PackageManager::nextPackageID = 1;

void PackageManager::setDestinations(const Map &map) {
    destinationPositions.clear();

    for (int i = 0; i < map.rows; i++) {
        for (int j = 0; j < map.columns; j++) {
            if (map.grid[i][j] == DESTINATION) {
                destinationPositions.push_back(Position(i, j));
            }
        }
    }
    if (destinationPositions.empty()) {
        cout<<"Nu exista destinatii pe harta!"<<endl;
        //exceptieeeeeeeeeeeeeeeee
    }

}

bool PackageManager::spawnPackage(int currentTick) {
    if (destinationPositions.empty()) {
        cout << "Nu exista destinatii pt a genera pachete!" << endl;
        return false;
    }

    if (nrGeneratedPackages >maxPackages) {
        return false;
    }
    int index = rand() % destinationPositions.size();
    Position newDestination = destinationPositions[index];

    Package* newPackage = new Package(nextPackageID++, newDestination);
    //cout<<"DEBUG PACKET ID "<<newPackage->getID()<<endl;
    newPackage->setCurrentTick(currentTick);
    newPackage->setSpawnTick(currentTick);

    pendingPackages.push_back(newPackage);
    nrGeneratedPackages++;
    nrCurrentPackages++;

    //cout<<"DEBUG PACHETE CREATE PANA ACUM:"<<this->maxPackages<<" dsgfds "<<this->nrGeneratedPackages<<endl;

    return true;
}


void PackageManager::markDelivered(int packageId, int currentTick) {
    int index = 0;
    for (auto *package: beingDeliveredPackages) {
        if (package->getID() == packageId) {
            package->setCurrentTick(currentTick);
            package->markDelivered();

            if (package->checkLate()) {
                latePackages.push_back(package);
            }
            deliveredPackages.push_back(package);

            beingDeliveredPackages.erase(beingDeliveredPackages.begin() + index);
            nrCurrentPackages--;
            return;
        }
        index++;
    }
    cout << "[DEBUG] WARNING: Nu am gasit pachetul " << packageId << " in beingDeliveredPackages!" << endl;
}


void PackageManager::checkDeadline(int currentTick) {
    auto it = beingDeliveredPackages.begin();
    while (it != beingDeliveredPackages.end()) {
        Package* p = *it;
        p->setCurrentTick(currentTick);

        if (p->checkLate()) {
            latePackages.push_back(p);
            it = beingDeliveredPackages.erase(it);
            nrCurrentPackages--;
        } else {
            it++;
        }
    }
}


Package* PackageManager::getNextPackage() {
    if (pendingPackages.empty()) {
        return nullptr;
    }

    Package* package = pendingPackages.front();
    pendingPackages.erase(pendingPackages.begin());
    addBeingDelivered(package);
    return package;
}



int PackageManager::getNrPendingPackages() {
    return this->pendingPackages.size();
}

int PackageManager::getNrDeliveredPackages() {
    return this->deliveredPackages.size();
}

int PackageManager::getNrLatePackages() {
    return this->latePackages.size();
}
int PackageManager::getNrUndeliveredPackages() {
    return this->undeliveredPackages.size();
}

int PackageManager::totalReward() const {
    int total = 0;
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
            //cout << "[DEBUG] WARNING: Pachet " << package->getID() << " deja in beingDeliveredPackages!" << endl;
            return;
        }
    }
    beingDeliveredPackages.push_back(package);
}

int PackageManager::getNrGeneratedPackages() {
    return this->nrGeneratedPackages;
}

int Package::getSpawnTick() {
    return this->spawnTick;
}