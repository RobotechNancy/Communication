#include "../include/xbee.h"

using namespace std;

int main() {
    XBee xbee;
    int status;

    if ((status = xbee.openSerialConnection()) != XB_SER_E_SUCCESS)
        return status;

    thread heartbeat(&XBee::sendHeartbeat, &xbee);
    thread waitingtrame(&XBee::waitForATrame, &xbee);
    thread reponse(&XBee::isXbeeResponding, &xbee);

    while (true);
    xbee.closeSerialConnection();
    return XB_E_SUCCESS;
}
