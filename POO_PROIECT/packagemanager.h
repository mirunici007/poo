 //
// Created by HP-15S on 12/26/2025.
//

#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include "package.h"
#include "map.h"
#include <vector>

class PackageManager {
 std::vector<Package*> pendingPackages;

 std::vector<Package* > deliveredPackages;
 std::vector<Package* > latePackages;
 std::vector<Package* > undeliveredPackages;

 std::vector<Position> destinationPositions;

 int maxPackages;
 int nrCurrentPackages;
 int nrGeneratedPackages;
 static int nextPackageID;

 public:
 std::vector<Package*> beingDeliveredPackages;
 PackageManager(int nrMax);
 ~PackageManager() = default;

 void setDestinations(const Map &map);
 bool spawnPackage(int currentTick);
 void markDelivered(int packageId, int currentTick);
 void checkDeadline(int currentTick);

 int getNrPendingPackages();
 int getNrDeliveredPackages();
 int getNrLatePackages();
 int getNrUndeliveredPackages();
 int getNrGeneratedPackages();

 std::vector<Package *> &getPendingPackages();
 void addBeingDelivered(Package* package);


 int totalReward() const;
 int totalUndeliveredPenalty() const;
 int totalLatePenalty() const;

 Package *getNextPackage();
};
#endif //PACKAGEMANAGER_H
