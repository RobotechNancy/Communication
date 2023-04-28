//
// Created by mrspaar on 1/20/23.
//

#include "can.h"

int main() {
    Can can;
    int err;

    if ((err = can.init(CAN_ADDR_RASPBERRY_E)) < 0) {
        can.logger << "Erreur lors de l'initialisation du bus CAN (n°" << err << ")" << mendl;
        return err;
    }

    can.subscribe(0x32000, [](const can_mess_t& message) {
       std::cout << "Bonjour !" << std::endl;
    });

    can.start_listen();

    while (true);
    return 0;
}
