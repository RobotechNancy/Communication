/*!
 * 	@file      can.h
 *  @brief     Gestion du bus can (écoute et envoie de message)
 *  @details   Version modifiée de la librairie de Théo RUSINOWITCH
 *  @author    Julien Pistre
 *  @version   1.2
 *  @date      2022-2023
 */

#include "can.h"

can_address_t canAddress;


void configure_CAN(CAN_HandleTypeDef *hcan, can_address_t addr) {
    canAddress = addr;
    HAL_CAN_Start(hcan);                                             // Démarrer le périphérique CAN
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING); // Activer le mode interruption
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	uint8_t RxData[8];
	CAN_RxHeaderTypeDef RxHeader;
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);

	can_message_t msg;
    if (format_frame(&msg, RxHeader, RxData) != 0)
        return;

    switch (msg.functionCode) {
        case FCT_ACCUSER_RECPETION:
            send(hcan, msg.senderAddress, FCT_ACCUSER_RECPETION, msg.data, 1, msg.messageID, true);
        default:
            break;
    }
}


int format_frame(can_message_t *rep, CAN_RxHeaderTypeDef frame, const uint8_t data[]) {
    rep->receiverAddress = (frame.ExtId & CAN_MASK_RECEIVER_ADDR) >> CAN_OFFSET_RECEIVER_ADDR;

    if (rep->receiverAddress != canAddress && rep->receiverAddress != CAN_ADDR_BROADCAST)
        return -1;

    rep->senderAddress = (frame.ExtId & CAN_MASK_EMIT_ADDR) >> CAN_OFFSET_EMIT_ADDR;
    rep->functionCode = (frame.ExtId & CAN_MASK_FUNCTION_CODE) >> CAN_OFFSET_FUNCTION_CODE;
    rep->messageID = (frame.ExtId & CAN_MASK_MESSAGE_ID) >> CAN_OFFSET_MESSAGE_ID;
    rep->isResponse = (frame.ExtId & CAN_MASK_IS_RESPONSE);

    for (int i = 0; i < frame.DLC; i++){
        rep->data[i] = data[i];
    }

    rep->length = frame.DLC;
    return 0;
}


int send(CAN_HandleTypeDef *hcan, can_address_t address, function_code_t functionCode , uint8_t data[], uint8_t length, uint8_t messageID, bool isResponse) {
    if (length > 8)
        return -1;

	CAN_TxHeaderTypeDef txHeader;
    txHeader.DLC = length;
    txHeader.IDE = CAN_ID_EXT;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.TransmitGlobalTime = DISABLE;

	txHeader.ExtId = canAddress << CAN_OFFSET_EMIT_ADDR |
                     address << CAN_OFFSET_RECEIVER_ADDR |
                     functionCode << CAN_OFFSET_FUNCTION_CODE |
                     messageID << CAN_OFFSET_MESSAGE_ID |
                     isResponse;

	uint32_t TxMailbox;
	HAL_CAN_AddTxMessage(hcan, &txHeader, data, &TxMailbox);

	return 0;
}
