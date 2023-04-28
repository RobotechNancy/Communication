# Librairies de communication

Ce repo contient les librairies réutilisables dans les projets de l'équipe :
- `CAN` : librairie C++ pour communiquer avec le bus CAN
- `Logs` : librairie C++ pour gérer les logs
- `Xbee` : librairie C++ pour communiquer avec le module Xbee

## Dépendances

Ces librairies utilisent `CMake` pour être compilées et déployées :
```bash
# Distributions Debian (Ubuntu, Mint, ...)
sudo apt install cmake make g++

# Distributions RedHat (Fedora, CentOS, ...)
sudo yum install cmake make gcc-c++
sudo dnf install cmake make gcc-c++
```

## Installation

Le script `lib_manager` permet d'installer les librairies sur votre système :
```bash
# Installer toutes les librairies
./lib_manager install

# Installer des librairies spécifiques
./lib_manager install CAN/Raspberry Logs

# Supprimer les dossiers de build
./lib_manager clean
```
Des problèmes d'installation peuvent survenir quand un dossier `cmake-install` est déjà présent.<br>
Dans ce cas, il suffit de lancer `./lib_manager clean` pour supprimer les dossiers de build.

## Utilisation

Une fois les librairies installées, il est possible de les utiliser dans n'importe quel projet.<br>
Pour cela, il suffit d'ajouter un fichier `CMakeLists.txt` à la racine du projet :
```cmake
project(my_project)                                       # Nom du projet
set(CMAKE_CXX_STANDARD 20)                                # Version de C++
cmake_minimum_required(VERSION 3.24)                      # Version de CMake

find_package(PkgConfig REQUIRED)                          # Commun à tous les projets
pkg_check_modules(UNE_LIB REQUIRED UNE_LIB)               # Chercher une librairie

add_executable(${PROJECT_NAME} main.cpp)                  # Ajouter des fichiers sources
target_link_libraries(${PROJECT_NAME} ${Logs_LIBRARIES})  # Lier "UNE_LIB" au projet
```

Les librairies s'installent dans un dossier `robotech` pour faciliter les includes :
```cpp
#include <robotech/can.h>
#include <robotech/logs.h>
#include <robotech/can_vars.h>
```

## Exécution

Pour compiler et exécuter votre projet :
```bash
mkdir build && cd build
cmake .. && make

# L'éxécutable porte le même nom que dans project()
./my_project
```