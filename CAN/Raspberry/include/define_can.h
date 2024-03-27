/*!
 * @file define_can.h
 * @version 1.3
 * @date 2023-2024
 * @author Romain ADAM
 * @brief Header avec les constantes du bus CAN
 * @details Version modifiée de la librairie de Julien PISTRE (v1.2)
 */

#ifndef RASPI_DEFINE_CAN_H
#define RASPI_DEFINE_CAN_H


// Le header est aussi utilisable en C donc
// en fonction du langage, on inclut les headers C++ ou C
#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

// Interface à utiliser (vcan0 ou can0)
#define CAN_INTERFACE "can0"

// Masques et décalages pour extraire les informations d'un message CAN
#define CAN_MASK_PRIORITY       0b11000000000000000000000000000
#define CAN_MASK_EMIT_ADDR      0b00111100000000000000000000000
#define CAN_MASK_RECEIVER_ADDR  0b00000011110000000000000000000
#define CAN_MASK_FUNCTION_MODE  0b00000000001111000000000000000
#define CAN_MASK_FUNCTION_CODE  0b00000000000000111111111100000
#define CAN_MASK_MESSAGE_ID     0b00000000000000000000000011110
#define CAN_MASK_IS_RESPONSE    0b00000000000000000000000000001

#define CAN_OFFSET_PRIORITY       27
#define CAN_OFFSET_EMIT_ADDR      23
#define CAN_OFFSET_RECEIVER_ADDR  19
#define CAN_OFFSET_FUNCTION_MODE  15
#define CAN_OFFSET_FUNCTION_CODE  5
#define CAN_OFFSET_MESSAGE_ID     1

typedef enum {
	/* Adresses Codées sur 2 bits : 0x0 à 0x3 */
	CANBUS_PRIO_HIGH  = 0x0,
	CANBUS_PRIO_STD   = 0x1,
	CANBUS_PRIO_LOW   = 0x2,
	CANBUS_PRIO_INFO  = 0x3

} CanBus_Priority;


typedef enum {
	/* Adresses Codées sur 4 bits : 0x00 à 0x0F */

    CANBUS_RASPBERRY     = 0x01,
    CANBUS_BASE_ROULANTE = 0x02,
    CANBUS_ODOMETRIE     = 0x03,
    CANBUS_TOF           = 0x04,
    CANBUS_ACTIONNEURS   = 0x05,

	CANBUS_BROADCAST     = 0x0F
} CanBus_Address;

typedef enum {
	/* Modes de fonctionnement du Robot codés sur 4 bits : 0x00 à 0x0F */

	MODE_DEBUG       = 0x00,
	MODE_COMPETITION = 0x01
} CanBus_Fnct_Mode;


typedef enum {
	/* Codes fonctions codés sur 12 bits : 0x0000 à 0x0FFF */

    FCT_ACCUSER_RECEPTION = 0x0000,

	FCT_DPL_TRIANGLE      = 0x0021,
	FCT_DPL_AVANCE        = 0x0029,

	FCT_ERROR             = 0x0FFE,
	FCT_COMPLETE          = 0x0FFF,

} CanBus_Fnct_Code;


typedef struct {
	uint8_t Priority;

    uint8_t ReceiverAddress;
    uint8_t SenderAddress;

    uint8_t FunctionMode;

    uint8_t Data[8];
    uint8_t Length;
    uint16_t FunctionCode;

    uint8_t MessageID;
    bool IsResp;
} CanBus_FrameFormat;

#endif /* INC_CANBUS_DEFINE_H_ */