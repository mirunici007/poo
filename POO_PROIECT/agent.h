//
// Created by HP-15S on 12/23/2025.
//

#ifndef AGENT_H
#define AGENT_H

#include "map.h"
#include "package.h"
#include <memory>
#include <vector>

enum StateMachine {
    IDLE,
    MOVING,
    CHARGING,
    DEAD
};

enum MovementType {
    AIR,
    GROUND
};

enum class Agents {
    DRONE,
    ROBOT,
    SCOOTER
};

class Agent {
protected:
    int speed;
    int maxBattery;
    int currentBattery;
    int consumptionPerTick;
    int cost;
    int capacity;

    char simbol;

    Position agentPosition;
    StateMachine currentState;
    MovementType movementType;
    std::vector<Package*> assignedPackages;
    std::vector<Position> currentPath;
    std::vector<Position> destinationPath;
    CellRole targetCellRole;

public:
    Agent(): speed(0),
    maxBattery(100),
    currentBattery(100),
    consumptionPerTick(2),
    cost(1),
    capacity(1),
    agentPosition(basePosition),
    movementType(GROUND),
    currentState(IDLE),

    targetCellRole(HUB){}
    virtual ~Agent() = default;

    virtual void actionPerTick(const Map &map);

    int getCost()const;
    StateMachine getCurrentState()const;
    int getPackageId() const;
    //Package* getAssignedPackage() const;
    Position getAgentPosition()const;
    MovementType getMovementType()const;
    int getSpeed()  const;
    int getCurrentBattery() const;
    int getMaxBattery() const;
    int getConsumptionPerTick()const;

    //as sterge-o
    virtual std::string getName() const = 0;

    bool isDead()const;

    void addAssignedPackage(Package *package);
    void removeAssignedPackage(int packageId);
    void setPath(const std::vector<Position> &path);
    void setCurrentState(StateMachine state);
    void setAgentDestination(const Position &position);
    void setDestinationPath(const std::vector<Position> &path);
    void setCurrentBattery(const int &battery);
    std::vector<Package*>& getAssignedPackages();
    int countAssignedPackages() const;


    void move(const Map &map);
    bool hasReachedClient()const;

    virtual Agent* clone() const = 0;
};


class Drone: public Agent {
public:
    Drone(Position basePosition);
    ~Drone() override = default;

    std::string getName() const override;

    Agent* clone() const override {
        return new Drone(*this);
    }
};


class Robot: public Agent {
public:
    Robot(Position basePosition);
    ~Robot() override = default;

    std::string getName() const override;

    Agent* clone() const override {
        return new Robot(*this);
    }
};


class Scooter: public Agent {
public:
    Scooter(Position basePosition);
    ~Scooter() override = default;

    std::string getName() const override;

    Agent* clone() const override {
        return new Scooter(*this);
    }
};

class AgentFactory {
public:
    static std::unique_ptr<Agent> createAgent(Agents agentType, Position initPosition);
};

#endif //AGENT_H

