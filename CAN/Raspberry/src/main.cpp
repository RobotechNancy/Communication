#include "can.h"


void handleAcknowledge(CAN &can, const can_frame_t &frame) {
    if (frame.data[0] == 0x01)
        std::cout << "ACK" << std::endl;
    else
        std::cout << "NACK" << std::endl;

    can.send(frame.senderAddress, FCT_ACCUSER_RECPETION, {0x01}, frame.messageID, true);
}


int main() {
    CAN can;
    if (can.init(CAN_ADDR_RASPBERRY) < 0)
        return 1;

    can.bind(FCT_ACCUSER_RECPETION, handleAcknowledge);
    can.startListening();

    CAN can2;
    if (can2.init(CAN_ADDR_ODOMETRIE) < 0)
        return 1;

    can2.startListening();

    // Exemple d'envoi d'un message, ici on aura toujours CAN_TIMEOUT (aucun send dans handleAcknowledge)
    can_result_t res = can2.send(CAN_ADDR_RASPBERRY, FCT_ACCUSER_RECPETION, {0x01}, 1, false, 2);

    switch (res.status) {
        case CAN_OK: std::cout << "CAN_OK" << std::endl; break;
        case CAN_ERROR: std::cout << "CAN_ERROR" << std::endl; break;
        case CAN_TIMEOUT: std::cout << "CAN_TIMEOUT" << std::endl; break;
    }

    return 0;
}
