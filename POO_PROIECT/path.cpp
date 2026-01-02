//
// Created by HP-15S on 12/25/2025.
//

#include "path.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <limits.h>

//implementare nod
int Node::fcost() const {
    return gCost + hCost;
}

bool Node::operator >(const Node &other) const {
    return fcost() > other.fcost();
}


//implementare clasa PathFinder
int PathFinder::distanceManhatan(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool PathFinder::canMoveTo(int x, int y, const Map& map, bool canFly) {
    if (x < 0|| x >= map.columns || y < 0 || y >= map.rows) {
        return false;
    }

    //pt dronele care pot trece peste ziduri
    if (canFly) {
        return true;
    }
    //cazul pt scootere si roboti
    return map.grid[x][y] != WALL;
}

int PathFinder::coordToKey(int x, int y, int mapWidth) {
    return x + y * mapWidth;
}

std::vector<Position> PathFinder::getPath(
        const Map &map,
        const Position &start,
        const Position &end,
        bool canFly) {

    std::vector<Position> path;

    int startX = start.getPositionX();
    int startY = start.getPositionY();

    int endX = end.getPositionX();
    int endY = end.getPositionY();

    //verificam daca variabilele pt inceput si sfarsit sunt valide
    if (!canMoveTo(startX, startY, map, canFly) || !canMoveTo(endX, endY, map, canFly)) {
        return path;
    }

    //daca am ajuns la client(destinatie)
    if (startX == endX && startY == endY) {
        path.push_back(start);
        return path;
    }

    //hash map pt noduri pt A*
    std::unordered_map <int, Node> nodes;

    //matricea nodurilor vizitate
    int visited[100][100] = {0};

    //
    //
    //comentarii pt priority queue
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> nodesList;

    //incepem cu nodul de la care plecam
    Node firstNode;

    firstNode.x = startX;
    firstNode.y = startY;
    firstNode.gCost = 0;
    firstNode.hCost = distanceManhatan(startX, startY, endX, endY);
    firstNode.parentX = startX;
    firstNode.parentY = startY;

    int startKey = coordToKey(startX, startY, map.columns);
    nodes[startKey] = firstNode;
    nodesList.push(firstNode);

    //directii: sus jos stanga dreapta
    const int dx[] = {0, 0, -1, 1};
    const int dy[] = {-1, 1, 0, 0};

    bool pathFound = false;

    //incepem algoritmul A* propriu-zis
    while (!nodesList.empty()) {
        Node current = nodesList.top();
        nodesList.pop();

        int x = current.x;
        int y = current.y;

        //verificam daca am visitat punctul
        if (x < 0 || x >= map.rows || y < 0 || y >= map.columns) {
            if (visited[x][y]) {
                continue;
            }
        }
        visited[x][y] = 1;

        //daca am ajuns la client;
        if (x == endX && y == endY) {
            pathFound = true;
            break;
        }

        //verificam si vecinii
        for (int i = 0; i< 4; i++) {
            int vecinX = dx[i] + x;
            int vecinY = dy[i] + y;
            int newKey = coordToKey(vecinX, vecinY, map.columns);

            //verid=ficam daca vacinul e valid si nu a fost visitat
            if (vecinX >= 0 && vecinX < map.rows &&
                vecinY >= 0 && vecinY < map.columns &&
                !visited[vecinX][vecinY]) {

                int vecinCostG = current.gCost + 1;
                int vecinCostH = distanceManhatan(vecinX, vecinY, endX, endY);

                //vedem daca am gasit un drum mai bun
                auto better = nodes.find(newKey);
                if (better == nodes.end() || vecinCostG < better->second.gCost) {
                    Node newNode;
                    newNode.x = vecinX;
                    newNode.y = vecinY;
                    newNode.gCost = vecinCostG;
                    newNode.hCost = vecinCostH;
                    newNode.parentX = x;
                    newNode.parentY = y;

                    nodes[newKey] = newNode;
                    nodesList.push(newNode);
                }
            }
        }
    }

    if (pathFound) {
        int x = endX;
        int y = endY;

        //facem drumul inapoi, de la sfarsit la inceput
        while (!(x == startX &&y == startY)) {
            path.push_back(Position(x, y));

            int key = coordToKey(x, y, map.columns);
            Node &tempNode = nodes[key];
            int tempX = tempNode.parentX;
            int tempY = tempNode.parentY;
            x = tempX;
            y = tempY;
        }
        path.push_back(Position(startX, startY));

        //inversam drumul
        std::reverse(path.begin(), path.end());
    }
    return path;
}

Position PathFinder::findNearestStation(const Map &map, Agent *agent) {
    Position nearestStation = agent->getAgentPosition();
    int minDistance = INT_MAX;
    bool canFly = agent->getMovementType() == AIR;

    for (const auto &station: Map::stations) {
        std::vector<Position> path = PathFinder::getPath(map, agent->getAgentPosition(), station, canFly);

        if (!path.empty() && path.size()  < minDistance) {
            minDistance = path.size() ;
            nearestStation = station;
        }
    }
    return nearestStation;
}