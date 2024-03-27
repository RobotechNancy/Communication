project(mon_projet_avec_can)                # Nom du projet
set(CMAKE_CXX_STANDARD 20)                  # Version de C++
cmake_minimum_required(VERSION 3.22.1)        # Version de cmake

find_package(PkgConfig REQUIRED)            # Utilitaire pour trouver des librairies
pkg_check_modules(CAN REQUIRED CAN)         # Deuxième CAN = nom de la librairie,
                                            # Premier CAN = permet de créer CAN_LIBRARIES et CAN_INCLUDE_DIRS

add_executable(${PROJECT_NAME} test.cpp)             # Créer un exécutable du même nom que le projet
target_link_libraries(${PROJECT_NAME} ${CAN_LIBRARIES})   # Lier les fichier cpp de la lib CAN à l'exécutable
