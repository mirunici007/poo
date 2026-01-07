//
// Created by HP-15S on 12/24/2025.
//

#ifndef PACKAGE_H
#define PACKAGE_H



//clasa pentru pozitii
class Position {
    int positionX;
    int positionY;

public:
    Position(int baseX, int baseY) : positionX(baseX), positionY(baseY) {}
    ~Position() = default;

    void setPosition(int x, int y);

    int getPositionX() const;
    int getPositionY() const;
};

//stari pentru pachete
enum StatusPackage {
    PENDING,
    IN_TRANSIT,
    DELIVERED
};

//pozitia hub-ului pe harta
extern Position basePosition;

class Package {
    Position destPosition;
    StatusPackage status;

    int id;
    int reward;
    int deadline;
    int currentTick;
    int spawnTick;

    bool late;
    bool delivered;
    bool isLateReported;

    public:
    Package(int packageId, Position destPosition);
    ~Package() = default;

    void markDelivered();
    void markLate();
    bool checkLate();

    //gettere
    int getID()const;
    int getReward()const;
    int getDeadline() const;
    int getSpawnTick() const;
    bool isDelivered() const;
    bool isLate() const;
    Position getDestPosition() const;

    //metode pentru setare
    void setSpawnTick(int spawnTick);
    void setCurrentTick(int currentTick);
};

#endif //PACKAGE_H
