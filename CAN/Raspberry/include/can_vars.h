/*!
 * @file can_vars.h
 * @version 1.0
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier contenant les variables globales du bus CAN
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v4.1a)
 */

#ifndef CAN_CAN_VARS_H
#define CAN_CAN_VARS_H

#include <stdint.h>


#define CAN_BUS_NAME "vcan0"

//                                0b00000000000000000000000000000
//                                Ob|1  |5   |10  |15  |20  |25 |29
//                                Ob|29 |25  |20  |15  |10  |5  |1
#define CAN_FILTER_ADDR_EMETTEUR  0b01111000000000000000000000000
#define CAN_FILTER_ADDR_RECEPTEUR 0b00000111100000000000000000000
#define CAN_FILTER_CODE_FCT       0b00000000011111111000000000000
#define CAN_FILTER_IDE_MSG        0b00000000000000000111111110000
#define CAN_FILTER_IS_REP         0b00000000000000000000000001000
#define CAN_FILTER_REP_NBR        0b00000000000000000000000000111

#define CAN_DECALAGE_ID_MSG       4
#define CAN_DECALAGE_IS_REP       3

#define CAN_MAX_VALUE_ADDR        16
#define CAN_MAX_VALUE_CODE_FCT    128
#define CAN_MAX_VALUE_REP_NBR     8


/*!
 * @enum    CAN_ADDR
 * @brief   \n Adresses d'émission sur le bus CAN
 * @example CAN_ADDR_BROADCAST = 0xF000000
 * @details 0x?000000 ou bits 23 à 26
 */
typedef enum {
    CAN_ADDR_ROBOT_01 =         0x1000000,
    CAN_ADDR_ROBOT_02 =         0x2000000,
    CAN_ADDR_BASE_ROULANTE =    0x3000000,
    CAN_ADDR_ODOMETRIE =        0x4000000,
    CAN_ADDR_ACTIONNEUR =       0x5000000,
	CAN_ADDR_ODOMETRIE_TOF =    0x6000000,
    CAN_ADDR_ACTIONNEUR_PETIT = 0x7000000,
    CAN_ADDR_ACTIONNEUR_GROS  = 0x8000000,
    CAN_ADDR_BROADCAST =        0xF000000,
} CAN_ADDR;


/*!
 * @enum    CAN_EMIT_ADDR
 * @brief   Adresses de réception sur le bus CAN
 * @example CAN_ADDR_BROADCAST_E = 0xF00000
 * @details 0x?00000 ou bits 20 à 23
 */
typedef enum {
    CAN_ADDR_ROBOT_01_E =         0x100000,
    CAN_ADDR_ROBOT_02_E =         0x200000,
    CAN_ADDR_BASE_ROULANTE_E =    0x300000,
    CAN_ADDR_ODOMETRIE_E =        0x400000,
    CAN_ADDR_ACTIONNEUR_E =       0x500000,
	CAN_ADDR_ODOMETRIE_TOF_E =    0x600000,
    CAN_ADDR_ACTIONNEUR_PETIT_E = 0x700000,
    CAN_ADDR_ACTIONNEUR_GROS_E  = 0x800000,
    CAN_ADDR_BROADCAST_E =        0xF00000,
} CAN_EMIT_ADDR;


/*!
 * @enum    CAN_FCT_CODE
 * @brief   Codes fonction
 * @example FCT_AVANCE = 0x010000
 * @details 0x??00 ou bits 12 à 19
 */
typedef enum {
    FCT_AVANCE =                        0x01000,
    FCT_REP_AVANCE =                    0x02000,

    FCT_ASPIRER_BALLE =                 0x11000,
    FCT_PLACER_BALLE =                  0x12000,
    FCT_OUVRIR_PANIER =                 0x13000,
    FCT_FERMER_PANIER =                 0x14000,

    FCT_GET_VARIATION_XY =              0x32000,
    FCT_GET_OPTIQUE =                   0x33000,

    FCT_TOF_WCOLLISION = 				0x0f000,
    FCT_ACCUSER_RECPETION =				0xff000,
} CAN_FCT_CODE;


/*!
 * @typedef can_mess_t
 * @brief   Trame customisée pour le bus CAN
 */
typedef struct {
    uint32_t recv_addr;  /*!< Adresse de réception */
    uint32_t emit_addr;  /*!< Adresse d'émission */
    uint32_t fct_code;   /*!< Code fonction */
    uint8_t message_id;  /*!< Identifiant du message */

    bool is_rep;         /*!< Indique si le message est une réponse */
    uint8_t rep_id;      /*!< Identifiant de la réponse ou nombre de réponses attendues */

    uint8_t data_len;    /*!< Taille des données (max 8) */
    uint8_t data[8];     /*!< Données (max 8 octets) */
} can_mess_t;


#define CAN_E_WRITE_ERROR (-501)
#define CAN_E_READ_ERROR (-502)
#define CAN_E_SOCKET_ERROR (-503)
#define CAN_E_BIND_ERROR (-504)

#define CAN_E_DATA_SIZE_TOO_LONG (-510)
#define CAN_E_OOB_ADDR (-511)
#define CAN_E_OOB_CODE_FCT (-512)
#define CAN_E_OOB_REP_NBR (-513)
#define CAN_E_OOB_DATA (-514)

#define CAN_E_UNKNOW_ADDR (-551)
#define CAN_E_UNKNOW_CODE_FCT (-552)

#endif //CAN_CAN_VARS_H
