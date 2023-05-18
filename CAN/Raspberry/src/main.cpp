//
// Created by mrspaar on 1/20/23.
//

#include "can.h"

int main() {
    Can can;
    int err;

    if ((err = can.init()) < 0) {
        std::cerr << "Erreur lors de l'initialisation du bus CAN (n°" << err << ")" << std::endl;
        return err;
    }

    can.subscribe(FCT_ACCUSER_RECPETION, [](const can_mess_t& message) {
       std::cout << "Bonjour !" << std::endl;
    });

    can.start_listen();

    do {
        std::cout << "Appuyez sur 'q' pour quitter" << std::endl;
    } while (std::cin.get() != 'q');

    can.close();
    return 0;
}
