cmake_minimum_required(VERSION 3.30)
project(POO_PROIECT)

set(CMAKE_CXX_STANDARD 20)

add_executable(POO_PROIECT main.cpp
        map.h
        map.cpp
        agent.h
        agent.cpp
        package.h
        package.cpp
        config.h
        path.h
        path.cpp
        simulation.cpp
        simulation.h
        packageManager.cpp
        packagemanager.h
        agentmanager.cpp
        agentmanager.h
        hivemind.h
        hivemind.cpp
)
