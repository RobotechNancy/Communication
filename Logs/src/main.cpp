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
    Logger logger("test");

    logger << "Hello, World!" << mendl;
    logger << "Hello, World! " << 2 << mendl;

    return 0;
}
