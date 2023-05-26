/*!
 * @file main.cpp
 * @version 1.0
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier de test de la classe Logger
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v4.1a)
 */

#include "logs.h"

int main() {
    Logger logger("test", "../logs.log");

    logger << "Hello, World!" << std::endl;
    logger(Log::WARNING) << "Hello, World! " << 2 << std::endl;
    logger(Log::NONE) << "Hello, World! " << 3 << std::endl;

    return 0;
}
