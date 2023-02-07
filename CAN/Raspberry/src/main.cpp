//
// Created by mrspaar on 1/20/23.
//

#include "../include/can.h"

int main() {
    Can can;
    int err;

    if ((err = can.init(CAN_ADDR_RASPBERRY_E)) < 0) {
        can.logger << "Erreur lors de l'initialisation du bus CAN (n°" << err << ")" << mendl;
        return err;
    }

    can.start_listen();
    can.send(CAN_ADDR_BROADCAST, FCT_ACCUSER_RECPETION, nullptr, 0, false, 0, 0);

    while (true);
    return 0;
}
