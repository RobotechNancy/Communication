/*!
 * @file    main.cpp
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier pour tester la librairie XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"


int main() {
    XBee xbee(XB_ADDR_ROBOT_01);
    int status = xbee.open("/dev/ttyUSB0");

    if (status != XB_E_SUCCESS) {
        return status;
    }

    xbee.startListening();

    //uint8_t data[1] = {0x05};
    //xbee.send(XB_ADDR_CAMERA_01, XB_FCT_TEST_ALIVE, data, 1);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return XB_E_SUCCESS;
}
