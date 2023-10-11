/*!
 * @file    main.cpp
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier pour tester la librairie XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"


void onTestAlive(XBee &xbee, const xbee_frame_t &frame) {
    std::cout << "Test Alive reçu de " << frame.emitterAddress << std::endl;
    xbee.send(frame.emitterAddress, XB_FCT_TEST_ALIVE, frame.data);
}

int main() {
    XBee xbee(XB_ADDR_ROBOT_01);
    int status = xbee.open("/dev/ttyUSB0");

    if (status != XB_E_SUCCESS)
        return status;

    xbee.bind(XB_FCT_TEST_ALIVE, onTestAlive);
    xbee.startListening();

    xbee.send(XB_ADDR_CAMERA_01, XB_FCT_TEST_ALIVE, {0x05});
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return XB_E_SUCCESS;
}
