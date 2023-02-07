# Librairie C++ du CAN

Il s'agit d'une version légèrement modifiée de la librairie de Théo RUSINOWITCH :
```c++
#include <robotech/can.h>

int main() {
    Can can;
    int err;
    
    if((err = can.init(CAN_ADDR_RASPBERRY_E)) < 0){
      can.logC << "Erreur dans l'initialisation du CAN (n°" << dec << err << ")" << mendl;
      return err;
    }
    
    can.start_listen();
    
    uint8_t data[5] = {0x01, 0x02, 0xFF, 0x34, 0x45};
    can.send(CAN_ADDR_BROADCAST, FCT_TEST_COMM, data, 5, false, 0, 0);
    
    while(true);
    return 0;
}
```

## Installation

Il suffit d'exécuter la commande `./lib_manager install CAN Logs` puis de créer un CMakeLists.txt :
```cmake
# Cette section à modifier selon votre projet
project(my_project)
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.24)

# La librairie CAN nécessite aussi la librairie Logs
find_package(PkgConfig REQUIRED)
pkg_check_modules(LOGS REQUIRED Logs)
pkg_check_modules(CAN REQUIRED C)

# Ajoutez tous vos fichiers source dans "add_executable"
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${LOGS_LIBRARIES} ${CAN_LIBRARIES})
```

## Bus can virtuel

Pour tester le CAN sans matériel, il est possible d'utiliser un bus virtuel.<br>

Tout d'abord, il faut installer le package `can-utils` :
```bash
# Debian
sudo apt install can-utils

# Fedora
sudo dnf install can-utils
```

Ensuite, il faut créer le bus virtuel (ici, `vcan0`) :
```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

On peut maintenant écouter tous les messages sur le bus virtuel :
```bash
candump vcan0
```