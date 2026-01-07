//
// Created by HP-15S on 1/7/2026.
//

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

using namespace std;

//exceptie pentru fisierul de configurare
class ConfigFileException : public runtime_error {
public:
    ConfigFileException(const string &msg): runtime_error("ConfigFile " + msg + "\n") {}
};

//exceptie pentru citirea din fisier nereusita
class FileException : public runtime_error {
public:
    FileException(const string &msg): runtime_error("Fisierul " + msg + "\n") {}
};

//exceptie pentru erori in incarcarea hartii
class MapException : public runtime_error {
public:
    MapException(const string &msg): runtime_error("Harta " + msg + "\n") {}
};

//exceptie pentru erori pentru clasa Agent
class AgentException : public runtime_error {
public:
    explicit AgentException(const string &msg): runtime_error("Agent " + msg + "\n") {}
};

//exceptie pentru erori pentru clasa Pachete
class PackageException : public runtime_error {
public:
    PackageException(const string &msg): runtime_error("Package " + msg + "\n") {}
};

//exceptie pentru clasa PathFinder
class PathException : public runtime_error {
public:
    PathException(const string &msg1, const string &msg2): runtime_error("Nu exista drum intre " + msg1 + " si " + msg2 + "\n") {}
};

//exceptie pentru simulare
class SimulationException : public runtime_error {
public:
    SimulationException(const string& msg): runtime_error("Simularea " + msg + "\n") {}
};

//exceptie pentru lipsa clienti
class ClientException : public runtime_error {
public:
    ClientException(): runtime_error("Nu exista clienti pe harta!\n") {}
};

#endif //EXCEPTIONS_H
