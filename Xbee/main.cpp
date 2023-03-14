/*!
 * @file    xbee.h
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier d'en-tête de la classe XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "include/xbee.h"
using namespace std;

int main() {
    XBee xbee;
    int status;

    if ((status = xbee.openSerialConnection()) != XB_E_AT_SUCCESS) {
        cout << "Error while opening serial connection" << endl;
        return status;
    }

    thread waitingtrame(&XBee::waitForATrame, &xbee);
    this_thread::sleep_for(chrono::seconds(3600));

    xbee.closeSerialConnection();
    return XB_E_SUCCESS;
}
