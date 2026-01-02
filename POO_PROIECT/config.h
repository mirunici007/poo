//
// Created by HP-15S on 12/25/2025.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <iostream>
#include <sstream>

class ConfigFile {
    static ConfigFile* instance;

    ConfigFile() {
        this->mapRows = 10;
        this->mapColumns = 10;
        this->maxTicks = 1000;
        this->maxStations = 3;
        this->nrClients = 10;
        this->nrDrones = 3;
        this->nrRobots = 2;
        this->nrScooters = 1;
        this->nrPackages = 50;
        this->spawnFrequency = 10;
    }
    ~ConfigFile() = default;

    ConfigFile(const ConfigFile&) = delete;

public:
    int mapRows;
    int mapColumns;
    int maxTicks;
    int maxStations;
    int nrClients;
    int nrDrones;
    int nrRobots;
    int nrScooters;
    int nrPackages;
    int spawnFrequency;

    //de ferificat memory eak
    static ConfigFile& getInstance() {
        if (instance == nullptr) {
            instance = new ConfigFile();
        }

        return *instance;
    }

    void loadFromFile(const char *fileName = "simulation_setup.txt") {
        FILE *file = fopen(fileName, "r");
        if (!file) {
            //exceptieeeeeeee
            return;
        }

        char buffer[256];

        while (fgets(buffer, sizeof(buffer), file)) {
            if (buffer[0] == '\n' || buffer[0] == '/') {
                continue;
            }

            if (sscanf(buffer, "MAP_SIZE: %d %d", &this->mapRows, &this->mapColumns) == 2) {
                continue;
            }
            if (sscanf(buffer, "MAX_TICKS: %d", &this->maxTicks) == 1) {
                continue;
            }
            if (sscanf(buffer, "MAX_STATIONS: %d", &this->maxStations) == 1) {
                continue;
            }
            if (sscanf(buffer, "CLIENTS_COUNT: %d", &this->nrClients) == 1) {
                continue;
            }
            if (sscanf(buffer, "DRONES: %d", &this->nrDrones) == 1) {
                continue;
            }
            if (sscanf(buffer, "ROBOTS: %d", &this->nrRobots) == 1) {
                continue;
            }
            if (sscanf(buffer, "SCOOTERS: %d", &this->nrScooters) == 1) {
                continue;
            }
            if (sscanf(buffer, "TOTAL_PACKAGES: %d", &this->nrPackages) == 1) {
                continue;
            }
            if (sscanf(buffer, "SPAWN_FREQUENCY: %d", &this->spawnFrequency) == 1) {
                continue;
            }
        }
        fclose(file);
        //verificare ca toate datele s-au citit cum trebuie+ERORIIIIIIIIIIIIIII
    }

    void printConfig() const{
        std::cout<<"CONFIGURARI"<<std::endl;
        std::cout<<"MAP: "<<this->mapRows<<"    "<<this->mapColumns<<std::endl;
        std::cout<<"MAX_TICKS: "<<this->maxTicks<<std::endl;
        std::cout<<"MAX_STATIONS: "<<this->maxStations<<std::endl;
        std::cout<<"CLIENTS_COUNT: "<<this->nrClients<<std::endl;
        std::cout<<"DRONES: "<<this->nrDrones<<std::endl;
        std::cout<<"ROBOTS: "<<this->nrRobots<<std::endl;
        std::cout<<"SCOOTERS: "<<this->nrScooters<<std::endl;
        std::cout<<"TOTAL_PACKAGES: "<<this->nrPackages<<std::endl;
        std::cout<<"SPAN_FREQUENCY: "<<this->spawnFrequency<<std::endl<<std::endl;
    }


};

inline ConfigFile* ConfigFile::instance = nullptr;

#endif //CONFIG_H
