#include "can.h"


void handleAcknowledge(CAN &can, const CanBus_FrameFormat &frame) {
    if (frame.Data[0] == 0x01)
        std::cout << "ACK" << std::endl;
    else
        std::cout << "NACK" << std::endl;

    can.send((CanBus_Priority) frame.Priority,(CanBus_Address) frame.SenderAddress,(CanBus_Fnct_Mode) 0x00,(CanBus_Fnct_Code) frame.FunctionCode, {0x01}, frame.MessageID, true);
}


int main() {
    CAN can;
    if (can.init(CANBUS_RASPBERRY) < 0)
        return 1;

    can.bind(FCT_ACCUSER_RECEPTION, handleAcknowledge);
    can.startListening();

    // Exemple d'envoi d'un message, ici on aura toujours CAN_TIMEOUT (aucun send dans handleAcknowledge)
    can_result_t res = can.send(CANBUS_PRIO_STD, CANBUS_BASE_ROULANTE, MODE_DEBUG, FCT_DPL_AVANCE, {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, 1, 0, 2);

    switch (res.status) {
        case CAN_OK: std::cout << "CAN_OK" << std::endl; break;
        case CAN_ERROR: std::cout << "CAN_ERROR" << std::endl; break;
        case CAN_TIMEOUT: std::cout << "CAN_TIMEOUT" << std::endl; break;
    }

    return 0;
}
