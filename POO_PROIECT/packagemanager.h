 //
// Created by HP-15S on 12/26/2025.
//

#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include "package.h"
#include "map.h"
#include <vector>

class PackageManager {
 //vectori pentru a stoca separat pachetele in functie de statusul lor
 std::vector<Package*> pendingPackages;
 std::vector<Package*> beingDeliveredPackages;
 std::vector<Package* > deliveredPackages;
 std::vector<Package* > latePackages;
 std::vector<Package* > undeliveredPackages;
 std::vector<Position> destinationPositions;

 int maxPackages;
 int nrCurrentPackages;
 int nrGeneratedPackages;
 //pentru continuitatea id-urilor pachetelor
 static int nextPackageID;

 public:
 PackageManager(int nrMax);
 ~PackageManager() = default;

 //settere
 void setDestinations(const Map &map);
 bool spawnPackage(int currentTick);
 void markDelivered(int packageId, int currentTick);
 void checkDeadline(int currentTick);

 //gettere
 int getNrPendingPackages()const;
 int getNrDeliveredPackages()const;
 int getNrLatePackages()const;
 int getNrUndeliveredPackages()const;
 int getNrGeneratedPackages()const;
 //metoda nu e const, pentru returneaza o referinta ce poate fi modifcata dupa
 std::vector<Package *> &getPendingPackages();
 std::vector<Package *> &getBeingDeliveredPackages()const;
 //metoda nu e const pentru ca elimina pachetul din lista si dupa il returneaza
 Package *getNextPackage();

 //metode pentru gestionarea pachetelor
 void addBeingDelivered(Package* package);
 void movePendingToUndelivered();
 //metode care returneaza sume
 int totalReward() const;
 int totalUndeliveredPenalty() const;
 int totalLatePenalty() const;

};
#endif //PACKAGEMANAGER_H
