/*!
 * @file define_can.h
 * @version 1.2
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Header avec les constantes du bus CAN
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v1)
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
#define CAN_INTERFACE "vcan0"

// Masques et décalages pour extraire les informations d'un message CAN
#define CAN_MASK_EMIT_ADDR      0b11111111000000000000000000000
#define CAN_MASK_RECEIVER_ADDR  0b00000000111111110000000000000
#define CAN_MASK_FUNCTION_CODE  0b00000000000000001111111100000
#define CAN_MASK_MESSAGE_ID     0b00000000000000000000000011110
#define CAN_MASK_IS_RESPONSE    0b00000000000000000000000000001

#define CAN_OFFSET_EMIT_ADDR      21
#define CAN_OFFSET_RECEIVER_ADDR  13
#define CAN_OFFSET_FUNCTION_CODE  5
#define CAN_OFFSET_MESSAGE_ID     1


typedef enum {
    CAN_ADDR_RASPBERRY =     0x01,
    CAN_ADDR_BASE_ROULANTE = 0x02,
    CAN_ADDR_ODOMETRIE =     0x03,
    CAN_ADDR_TOF =           0x04,
    CAN_ADDR_ACTIONNEURS =   0x05,

    CAN_ADDR_BROADCAST =     0xFF
} can_address_t;


typedef enum {
    FCT_AVANCE =              0x21,

    FCT_GET_VARIATION_XY =    0x32,
    FCT_GET_OPTIQUE =         0x33,

    FCT_TOF_WARNING =         0x41,

    FCT_ASPIRER_BALLE =       0x51,
    FCT_PLACER_BALLE =        0x52,
    FCT_OUVRIR_PANIER =       0x53,
    FCT_FERMER_PANIER =       0x54,
    FCT_BAISSER_CREMAILLERE = 0x55,
    FCT_MONTER_CREMAILLERE =  0x56,
    FCT_ATTRAPER_GATEAU =     0x57,
    FCT_LACHER_GATEAU =       0x58,

    FCT_ACCUSER_RECPETION =	  0xFF,
} can_code_t;


typedef struct {
    uint8_t receiverAddress;
    uint8_t senderAddress;

    uint8_t data[8];
    uint8_t length;
    uint8_t functionCode;

    uint8_t messageID;
    bool isResponse;
} can_frame_t;


#endif //RASPI_DEFINE_CAN_H
