/*!
 * @file    xbee.h
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier d'en-tête des variables globales
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#ifndef DEFINE_XBEE_H
#define DEFINE_XBEE_H

// Paramètres du réseau XBee
#define XB_SERIAL_PORT_PRIMARY "/dev/ttyUSB0"
#define XB_BAUDRATE_PRIMARY 9600
#define XB_DATABITS_PRIMARY SERIAL_DATABITS_8
#define XB_PARITY_PRIMARY SERIAL_PARITY_NONE
#define XB_STOPBITS_PRIMARY SERIAL_STOPBITS_1

#define XB_ADR_ROBOT_01 0x12
#define XB_ADR_ROBOT_02 0x13
#define XB_ADR_CAMERA   0x14
#define XB_ADR_CURRENT XB_ADR_ROBOT_01

#define XB_NODE_1 "1\r"
#define XB_NODE_2 "2\r"


// Paramètres de la trame message
#define XB_V_START 0x02
#define XB_V_END 0x03
#define XB_V_ACK 0x06

#define XB_FCT_TEST_ALIVE 0x07
#define XB_FCT_ARUCO_POS  0x08


// Code d'erreurs
#define XB_E_SUCCESS 000

#define XB_E_FCT_SUCCESS 100
#define XB_E_FCT_NONE_REACHABLE (-102)

#define XB_E_TRAME_SUCCESS 200
#define XB_E_TRAME_CRC (-204)

#define XB_E_SUB_TRAME_SUCCESS 300

#define XB_E_AT_SUCCESS 400
#define XB_E_AT_ENTER (-401)
#define XB_E_AT_API (-402)
#define XB_E_AT_BAUDRATE (-403)
#define XB_E_AT_AES (-404)
#define XB_E_AT_AES_KEY (-405)
#define XB_E_AT_CHANEL (-406)
#define XB_E_AT_PAN_ID (-407)
#define XB_E_AT_COORDINATOR (-408)
#define XB_E_AT_PARITY (-409)
#define XB_E_AT_16BIT_SOURCE_ADDR (-410)
#define XB_E_AT_LOW_DEST_ADDR (-411)
#define XB_E_AT_EXIT (-412)
#define XB_E_AT_WRITE_CONFIG (-413)
#define XB_E_AT_DISCOVER_NETWORK (-414)

#define XB_E_SER_SUCCESS 500
#define XB_E_SER_NOT_FOUND (-501)
#define XB_E_SER_OPEN (-502)
#define XB_E_SER_PARAM (-503)
#define XB_E_SER_UKN_BAUDRATE (-504)
#define XB_E_SER_CONFIG (-505)
#define XB_E_SER_TIMOUT (-506)
#define XB_E_SER_UKN_DATABITS (-507)
#define XB_E_SER_UKN_STOPBITS (-508)
#define XB_E_SER_UKN_PARITY (-509)


// Commandes et valeurs AT
#define XB_AT_CMD_ENTER "+++"
#define XB_AT_CMD_EXIT "ATCN"
#define XB_AT_CMD_WRITE_CONFIG "ATWR"
#define XB_AT_V_END_LINE "\r"

#define XB_AT_CMD_API "ATAP"
#define XB_AT_V_API "0\r"

#define XB_AT_CMD_BAUDRATE "ATBD"
#define XB_AT_V_BAUDRATE "3\r"

#define XB_AT_CMD_PARITY "ATNB"
#define XB_AT_V_PARITY "0\r"

#define XB_AT_CMD_AES "ATEE"
#define XB_AT_V_AES "1\r"

#define XB_AT_CMD_AES_KEY "ATKY"
#define XB_AT_V_AES_KEY "32303032\r"

#define XB_AT_CMD_CHANEL "ATCH"
#define XB_AT_V_CHANEL "C\r"

#define XB_AT_CMD_PAN_ID "ATID"
#define XB_AT_V_PAN_ID "3332\r"

#define XB_AT_CMD_COORDINATOR "ATCE"
#if XB_ADR_CURRENT == XB_ADR_ROBOT_01
    #define XB_AT_V_COORDINATOR "1\r"
#else
    #define XB_AT_V_COORDINATOR "0\r"
#endif

#define XB_AT_CMD_HIGH_DEST_ADDR "ATDH"
#define XB_AT_V_HIGH_DEST_ADDR "0\r"

#define XB_AT_CMD_16BIT_SOURCE_ADDR "ATMY"
#define XB_AT_CMD_LOW_DEST_ADDR "ATDL"
#if XB_ADR_CURRENT == XB_ADR_CAMERA
    #define XB_AT_V_16BIT_SOURCE_ADDR XB_NODE_2
    #define XB_AT_V_LOW_DEST_ADDR XB_NODE_1
#else
    #define XB_AT_V_16BIT_SOURCE_ADDR XB_NODE_1
    #define XB_AT_V_LOW_DEST_ADDR XB_NODE_2
#endif

#define XB_AT_CMD_DISCOVER_NETWORK "ATND"
#define XB_AT_V_DISCOVER_NETWORK XB_AT_V_LOW_DEST_ADDR

// Réponses AT
#define XB_AT_R_EMPTY ""
#define XB_AT_R_SUCCESS "OK\r"

// Mode AT
#define XB_AT_M_GET 1
#define XB_AT_M_SET 2

#endif
