//
// Created by HP-15S on 12/23/2025.
//

#ifndef AGENT_H
#define AGENT_H

#include "map.h"
#include "package.h"
#include <memory>
#include <vector>

//state-ul in care e agentul in momentului tick-ului respectiv
enum StateMachine {
    IDLE,
    MOVING,
    CHARGING,
    DEAD
};

//daca robotul se misca la sol sau in mediu aerian
enum MovementType {
    AIR,
    GROUND
};

//tipul de agent
enum Agents {
    DRONE,
    ROBOT,
    SCOOTER
};

class Agent {
protected:      //atributele sunt protected ca sa fie accesibile numai claselor care o mostenesc
    int speed;
    int maxBattery;
    int currentBattery;
    int consumptionPerTick;
    int cost;   //cost per tick
    int capacity;

    char simbol;

    Position agentPosition; //pozitia curenta a agentului
    StateMachine currentState;
    MovementType movementType;  //daca sa misca in aer sau la sol
    std::vector<Package*> assignedPackages; //pachetele primite de livrat
    std::vector<Position> currentPath;  //path-ul pe care il are activ de urmat

    //un path auxiliar pt cazul in care trebuie sa opreasca la o statie in drumul spre client
    std::vector<Position> destinationPath;

    CellRole targetCellRole;    //ce tip de casuta este destinatia: client, hub, statie

public:
    Agent(): speed(0),
    maxBattery(100),
    currentBattery(100),
    consumptionPerTick(2),
    cost(1),
    capacity(1),
    agentPosition(basePosition),
    movementType(GROUND),
    //initializam agentul in hub, in stare idle
    currentState(IDLE),
    targetCellRole(HUB){}
    virtual ~Agent() = default;

    virtual void actionPerTick(const Map &map);

    //metode de get pt atributele protected
    int getCost()const;
    int getPackageId() const;
    int getSpeed()  const;
    int getCurrentBattery() const;
    int getMaxBattery() const;
    int getConsumptionPerTick()const;
    int getCapacity()const;

    char getsimbol()const;

    bool isDead()const;

    std::vector<Package*>& getAssignedPackages();
    StateMachine getCurrentState()const;
    Position getAgentPosition()const;
    MovementType getMovementType()const;


    //metode de set
    void setPath(const std::vector<Position> &path);
    void setCurrentState(StateMachine state);
    void setDestinationPath(const std::vector<Position> &path);
    void setCurrentBattery(const int &battery);


    //metode de adaugare si stergere de pachete din assignedPackages
    void addAssignedPackage(Package *package);
    void removeAssignedPackage(int packageId);

    //returneaza nr de pachete alocate unui agent
    int countAssignedPackages() const;

    //misca agentul pe harta
    void move(const Map &map);

    //daca robotul a ajuns la client este 1
    bool hasReachedClient()const;

    //metoda care ne ajuta sa cream o clona pentru simularea path-ului
    virtual Agent* clone() const = 0;
};


class Drone: public Agent {
public:
    Drone(Position basePosition);
    ~Drone() override = default;

    Agent* clone() const override {
        return new Drone(*this);
    }
};


class Robot: public Agent {
public:
    Robot(Position basePosition);
    ~Robot() override = default;

    Agent* clone() const override {
        return new Robot(*this);
    }
};


class Scooter: public Agent {
public:
    Scooter(Position basePosition);
    ~Scooter() override = default;

    Agent* clone() const override {
        return new Scooter(*this);
    }
};

//design pattern Factory pt a crea mai usor agenti si pt a elimina duplicarea comenzilor
class AgentFactory {
public:
    static std::unique_ptr<Agent> createAgent(Agents agentType, Position initPosition);
};

#endif //AGENT_H

