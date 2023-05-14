/*!
 * @file    main.cpp
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier pour tester la librairie XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"
using namespace std;


int main() {
    XBee xbee;
    int status = xbee.openSerialConnection("/dev/ttyUSB0", XB_ADR_ROBOT_01);

    if (status != XB_SER_E_SUCCESS) {
        cout << "Erreur à l'établissement de la connection série : " << status << endl;
        return status;
    }

    vector<::uint8_t> data = {0x05};
    xbee.sendFrame(XB_ADR_CAMERA_01, XB_FCT_TEST_ALIVE, data, 1);

    xbee.closeSerialConnection();
    return XB_E_SUCCESS;
}
