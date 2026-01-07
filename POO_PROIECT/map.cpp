//
// Created by HP-15S on 12/23/2025.
//

#include <iostream>
#include <sstream>
#include <random>
#include "map.h"
#include "package.h"
#include "config.h"
#include "exceptions.h"

using namespace std;


Map::Map(int row, int col, int clients, int stations): rows(row),
    columns(col),
    nrClients(clients),
    nrStations(stations) {
    ConfigFile &config = ConfigFile::getInstance();

    this->rows = config.getRows();
    this->columns = config.getColumns();
    this->nrClients = config.getNrClients();
    this->nrStations = config.getMaxStations();
}


//metoda pentru adaugarea clientilor in lista cu clienti
void Map::addClient(const int &posX, const int &posY) {
    clients.push_back(Position(posX, posY));
}

//metoda pentru adaugarea statiilor in lista cu statii
void Map::addStation(const int &posX, const int &posY) {
    stations.push_back(Position(posX, posY));
}

//sterge vectorii pentru a evita memory leak-ul

//gettere
int Map::getRows() const {
    return this->rows;
}

int Map::getCols() const {
    return this->columns;
}

int Map::getNrClients() const {
    return this->nrClients;
}

int Map::getNrStations() const {
    return this->nrStations;
}

std::vector<Position> Map::getStations() const {
    return this->stations;
}

CellRole Map::getCellRole(const int &posX, const int &posY) const {
    return this->grid[posX][posY];
}

void Map::setCellRole(const int &posX, const int &posY, const CellRole &cellRole) {
    this->grid[posX][posY] = cellRole;
}


//metoda care incarca mapa din fisier
void FileMapLoader::loadMap(Map &map) {
    FILE *file = fopen(fileName, "r");

    //exceptie daca nu s-a putut deschide fisierul
    if (!file) {
        throw FileException(" nu a putut fi deschis!");
    }

    //citeste nr de linii si coloane ale matricei
    fscanf(file, "%d %d", map.getRows(), map.getCols());

    for (int i = 0; i < map.getRows(); i++) {
        for (int j = 0; j < map.getCols(); j++) {
            char c = fgetc(file);

            //decide pt fiecare caracter ce rol are
            switch (c) {
                case '.': map.setCellRole(i, j, SPACE); break;
                case '#': map.setCellRole(i, j, WALL); break;
                case 'B': map.setCellRole(i, j, HUB); break;
                case 'S': map.setCellRole(i, j,STATION); map.addStation(i, j); break;
                case 'D': map.setCellRole(i, j, DESTINATION); map.addClient(i, j); break;
            }
        }
    }

}

//metoda care genereaza random mapa, respectand anumite valori
void ProceduralMapGenerator::loadMap(Map &map) {
    //initializeaza matricea
    for (int i = 0; i< map.getRows(); i++) {
        for (int j = 0; j< map.getCols(); j++) {
            map.setCellRole(i, j, SPACE);
        }
    }

    //genereaza coordonatele bazei
    int hubX = rand() % map.getRows();
    int hubY = rand() % map.getCols();
    basePosition.setPosition(hubX, hubY);
    map.setCellRole(hubX, hubY, HUB);

    //genereaza un nr de totalStations statii
    int totalStations = map.getNrStations();
    while (totalStations) {
        int randomRow = rand() % map.getRows();
        int randomColumn = rand() % map.getCols();

        //pune statia daca nu a fost atribuit alt rol pentru spatiul respectiv
        if (map.getCellRole(randomRow, randomColumn) == SPACE) {
            map.setCellRole(randomRow, randomColumn, STATION);
            map.addStation(randomRow, randomColumn);
            totalStations--;
        }
    }

    //genereaza un nr de totalClients clienti
    int totalClients = map.getNrClients();
    while (totalClients) {
        int randomRow = rand() % map.getRows();
        int randomColumn = rand() % map.getCols();

        //daca nu are alt rol atribuit pune clientul
        if (map.getCellRole(randomRow, randomColumn) == SPACE) {
            map.setCellRole(randomRow, randomColumn, DESTINATION);
            map.addClient(randomRow, randomColumn);
            totalClients--;
        }
    }

    //generam un anumit numar de ziduri
    int nrWalls = map.getRows() * map.getCols() *0.2;

    while (nrWalls) {
        int randomRow = rand() % map.getRows();
        int randomColumn = rand() % map.getCols();

        if (map.getCellRole(randomRow, randomColumn) == SPACE) {
            map.setCellRole(randomRow, randomColumn, WALL);
            nrWalls--;
        }
    }
}

//de vazut pe urma
bool MapValidator::validateMap(Map &map) {
    bool visited[100][100] = {false};

    // coadă simpla pentru BFS
    int queueX[10000], queueY[10000];
    int head = 0, tail = 0;

    //gaseste hub
    for (int i = 0; i < map.getRows(); i++) {
        for (int j = 0; j < map.getCols(); j++) {
            if (map.getCellRole(i, j) == HUB) {
                queueX[tail] = i;
                queueY[tail] = j;
                tail++;
                visited[i][j] = true;
            }
        }
    }

    //daca nu exista hub harta e invalida
    if (tail == 0) {
        return false;
    }

    //BFS pentru a marca toate celulele accesibile pentru toate directiile, mai putin pe diagonala
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    while (head < tail) {
        int x = queueX[head];
        int y = queueY[head];
        head++;

        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            //daca poate accesa pozitia respectiva
            if (nx >= 0 && nx < map.getRows() &&
                ny >= 0 && ny < map.getCols() &&
                map.getCellRole(nx, ny) != WALL && !visited[nx][ny]) {
                    visited[nx][ny] = true;
                    queueX[tail] = nx;
                    queueY[tail] = ny;
                    tail++;
                }
        }
    }

    //verifica daca toate destinatiile si statiile sunt accesibile
    for (int i = 0; i < map.getRows(); i++) {
        for (int j = 0; j < map.getCols(); j++) {
            if ((map.getCellRole(i, j) == DESTINATION || map.getCellRole(i, j) == STATION) &&
                !visited[i][j]) {
                return false;
                }
        }
    }
    //returneaza adevarat daca toate punctele importante sunt accesibile
    return true;
}