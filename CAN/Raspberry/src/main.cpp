#include "can.h"


void handleAcknowledge(const can_message_t &frame) {
    if (frame.data[0] == 0x01)
        std::cout << "ACK" << std::endl;
    else
        std::cout << "NACK" << std::endl;
}


int main() {
    Can can;

    if (can.init(CAN_ADDR_BASE_ROULANTE) < 0)
        return 1;

    can.bind(FCT_ACCUSER_RECPETION, handleAcknowledge);
    can.startListening();

    uint8_t data[1] = {0x01};
    can.send(CAN_ADDR_RASPBERRY, FCT_ACCUSER_RECPETION, data, 1, 1, false);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
