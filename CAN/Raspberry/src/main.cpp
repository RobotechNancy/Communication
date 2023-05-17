//
// Created by mrspaar on 1/20/23.
//

#include "can.h"

int main() {
    Can can(CAN_ADDR_ROBOT_01_E);
    int err;

    if ((err = can.init()) < 0) {
        can.logger << "Erreur lors de l'initialisation du bus CAN (n°" << err << ")" << mendl;
        return err;
    }

    can.subscribe(0x32000, [](const can_mess_t& message) {
       std::cout << "Bonjour !" << std::endl;
    });

    can.start_listen();

    do {
        std::cout << "Appuyez sur 'q' pour quitter" << std::endl;
    } while (std::cin.get() != 'q');

    can.close();
    return 0;
}
