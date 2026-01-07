//
// Created by HP-15S on 12/23/2025.
//

#ifndef MAP_H
#define MAP_H

#include "package.h"
#include <vector>


//rolul fiecarui element de pe harta
enum CellRole {
    SPACE,
    WALL,
    HUB,
    STATION,
    DESTINATION
};

class Map {
    //clasa contine drept atribute vectori cu pozitiile clientilor si statiilor pentru simplitate
    std::vector<Position> clients;
    std::vector<Position> stations;

    int rows;
    int columns;
    int nrClients;
    int nrStations;
    CellRole grid[100][100] = {SPACE};

public:

    Map(int row = 20, int col = 20, int clients = 10, int stations = 3);
    ~Map() = default;

    //adaugarea clientilor si statiilor in vectori
    void addClient(const int &posX, const int &posY);
    void addStation(const int &posX, const int &posY);

    //gettere
    int getRows() const;
    int getCols() const;
    int getNrClients() const;
    int getNrStations() const;
    CellRole getCellRole(const int &posX, const int &posY) const;

    std::vector<Position> getStations() const;

    void setCellRole(const int &posX, const int &posY, const CellRole &cellRole);

    //pentru accesarea din PathFinder
    //friend class PathFinder;
};

//design pattern Strategy pentru a gestiona usor mai multe strategii daca una nu functioneaza corect
class MapStrategy {
public:
    virtual void loadMap(Map &map) = 0;
    virtual ~MapStrategy() = default;
};

//prima strategie: incarca mapa dintr-un fisier
class FileMapLoader: public MapStrategy {
    const char *fileName;

public:
    FileMapLoader(const char *file): fileName(file){}
    ~FileMapLoader() override = default;

    void loadMap(Map &map)override;
};

//creaza mapa folosind valorile date
class ProceduralMapGenerator: public MapStrategy {
public:
    ~ProceduralMapGenerator() override = default;
    void loadMap(Map &map)override;
};

//clasa valideaza mapa, daca exista drumuri intre elemente
class MapValidator {
public:
    static bool validateMap(Map &map);
};

#endif //MAP_H