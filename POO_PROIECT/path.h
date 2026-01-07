//
// Created by HP-15S on 12/25/2025.
//

#ifndef PATH_H
#define PATH_H

#include "package.h"
#include "map.h"
#include "agent.h"
#include <vector>

//nod
struct Node {
    int x;
    int y;
    int gCost; //costul real
    int hCost; //aproximare cost
    int parentX;
    int parentY;

    int fcost() const;

    //redefinire operator pt comparare pe heap
    bool operator >(const Node &other) const;
};


// clasa pathfinder
class PathFinder {
    static int distanceManhatan(int x1, int y1, int x2, int y2);
    //verifica sunt dacă coordonatele in limitele hartii si daca celula poate fi traversata
    static bool canMoveTo(int x, int y, const Map& map, bool canFly);
    //helper pentru a converti coordonatele 2D în key unic pentru hash map
    static int coordToKey(int x, int y, int mapWidth);
public:
    //returneaza drumul folosind algoritmul A* cu distanta manhattan ca functie euristica
    //se deplazeaza sus, jus, stanga, drepate, si nu si pe diagonala
    static std::vector<Position> getPath(
        const Map &map,
        const Position &start,
        const Position &end,
        bool canFly = false);

    static Position findNearestStation(const Map &map, Agent *agent);
};

#endif //PATH_H
