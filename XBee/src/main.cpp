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
    XBee xbee("/dev/ttyUSB0", XB_ADR_ROBOT_01);
    int status = xbee.openSerialConnection();

    if (status != XB_SER_E_SUCCESS) {
        cout << "Erreur à l'établissement de la connection série : " << status << endl;
        return status;
    }

    xbee.subscribe(XB_FCT_TEST_ALIVE, [&](const frame_t& frame) {
        cout << "Réception d'un message de test de la part de " << frame.adr_emetteur << endl;
    });

    thread listen(&XBee::listen, &xbee);
    XBee::delay(3600);

    xbee.closeSerialConnection();
    return XB_E_SUCCESS;
}
