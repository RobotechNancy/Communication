#include "can.h"


void handleAcknowledge(const can_message_t &frame) {
    if (frame.data[0] == 0x01)
        std::cout << "ACK" << std::endl;
    else
        std::cout << "NACK" << std::endl;
}


int main() {
    Can can;
    if (can.init(CAN_ADDR_RASPBERRY) < 0)
        return 1;

    can.bind(FCT_ACCUSER_RECPETION, handleAcknowledge);
    can.startListening();

    Can can2;
    if (can2.init(CAN_ADDR_ODOMETRIE) < 0)
        return 1;

    uint8_t data[1] = {0x01};
    can2.send(CAN_ADDR_RASPBERRY, FCT_ACCUSER_RECPETION, data, 1, 1, false);

    return 0;
}
