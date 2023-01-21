//
// Created by mrspaar on 1/20/23.
//

#include "../include/logs.h"

int main() {
    Logger logger("test");

    logger << "Hello, World!" << mendl;
    logger << "Hello, World! " << 2 << mendl;

    return 0;
}
