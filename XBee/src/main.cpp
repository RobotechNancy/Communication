/*!
 * @file    main.cpp
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier pour tester la librairie XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"


void alive(const xbee_frame_t &frame) {
    std::cout << "Requête de " << std::showbase << std::hex << frame.emitAddress << std::endl;
    std::cout << "Données : " << std::endl;

    for (int i = 0; i < frame.length - XB_FRAME_DEFAULT_LENGTH - 2; i++) {
        std::cout << std::showbase << std::hex << (int)frame.data[i] << " ";
    }
}


int main() {
    XBee xbee(XB_ADDR_ROBOT_01);
    int status = xbee.open("/dev/ttyS0");

    if (status != XB_SER_E_SUCCESS) {
        return status;
    }

    uint8_t data[1] = {0x05};
    xbee.send(XB_ADDR_CAMERA_01, XB_FCT_TEST_ALIVE, data, 1);

    xbee.bind(XB_FCT_TEST_ALIVE, alive);
    xbee.startListening();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return XB_E_SUCCESS;
}
