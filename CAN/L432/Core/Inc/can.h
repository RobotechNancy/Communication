/*!
 * 	@file      can.h
 *  @brief     Gestion du bus can (écoute et envoie de message)
 *  @details   Version modifiée de la librairie de Théo RUSINOWITCH
 *  @author    Julien Pistre
 *  @version   1.1
 *  @date      2022-2023
 */

#ifndef CAN_H
#define CAN_H

#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "define_can.h"


void configure_CAN(CAN_HandleTypeDef *hcan, can_address_t adresse);
int format_frame(can_message_t *msg, CAN_RxHeaderTypeDef frame, const uint8_t data[]);
int send(CAN_HandleTypeDef *hcan, can_address_t address, function_code_t functionCode , uint8_t data[], uint8_t length, uint8_t messageID, bool isResponse);

#endif /* CAN_H */
