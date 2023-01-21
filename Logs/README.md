# Librairie de logs

Il s'agit d'une version légèrement modifiée de la librairie de Théo RUSINOWITCH :
```c++
#include <iomanip>
#include <robotech/logs.h>

int main() {
    // Par défaut, le fichier de log a le chemin "../logs.log"
    Logger logger("système");
    
    logger << "Une donnée très importante : " << 42 << mendl;
    
    logger << "Une ";
    logger << "autre ";
    logger << "donnée ";
    logger << "importante : ";
    logger << hex << showbase << 0xFF;
    logger << mendl;
    
    return 0;
}
```

## Installation

Il suffit d'exécuter la commande `./lib_manager install Logs` puis de créer un CMakeLists.txt :
```cmake
# Cette section à modifier selon votre projet
project(my_project)
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.24)

# Cette section permet de trouver la librairie de logs
find_package(PkgConfig REQUIRED)
pkg_check_modules(LOGS REQUIRED Logs)

# Ajoutez tous vos fichiers source dans "add_executable"
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${LOGS_LIBRARIES})
```
