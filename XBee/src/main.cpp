/*!
 * @file    xbee.h
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier d'en-tête de la classe XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"
using namespace std;


int main() {
    XBee xbee("/dev/ttyS0", XB_ADR_CAMERA_01);
    int status = xbee.openSerialConnection();

    if (status != XB_SER_E_SUCCESS) {
        cout << "Erreur à l'établissement de la connection série : " << status << endl;
        return status;
    }

    xbee.subscribe(XB_FCT_TEST_ALIVE, [&](const frame_t& frame) {
         xbee.sendFrame(frame.adr_emetteur, frame.code_fct, frame.data, frame.data_len);
    });

    xbee.start_listen();
    XBee::delay(60);

    xbee.closeSerialConnection();
    return XB_E_SUCCESS;
}
