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

#include <cstdint>
#include "serialib.h"


#define XB_BAUDRATE_PRIMARY  9600
#define XB_DATABITS_PRIMARY  SERIAL_DATABITS_8
#define XB_PARITY_PRIMARY    SERIAL_PARITY_NONE
#define XB_STOPBITS_PRIMARY  SERIAL_STOPBITS_1


#define XB_ADDR_ROBOT_01   0x01
#define XB_ADDR_ROBOT_02   0x02
#define XB_ADDR_CAMERA_01  0x03
#define XB_ADDR_CAMERA_02  0x04


#define XB_FCT_TEST_ALIVE     0x07
#define XB_FCT_GET_ARUCO_POS  0x08
#define XB_FCT_ARUCO_POS      0x09


#define XB_E_SUCCESS 000

#define XB_TRAME_E_SUCCESS 200
#define XB_TRAME_E_WRONG_ADR (-203)
#define XB_TRAME_E_CRC (-204)
#define XB_TRAME_E_DATALEN (-205)

#define XB_SUBTRAME_E_SUCCESS 300
#define XB_SUBTRAME_E_NONE (-301)

#define XB_AT_E_SUCCESS 400
#define XB_AT_E_ENTER (-401)
#define XB_AT_E_API (-402)
#define XB_AT_E_BAUDRATE (-403)
#define XB_AT_E_AES (-404)
#define XB_AT_E_AES_KEY (-405)
#define XB_AT_E_CHANEL (-406)
#define XB_AT_E_PAN_ID (-407)
#define XB_AT_E_COORDINATOR (-408)
#define XB_AT_E_PARITY (-409)
#define XB_AT_E_16BIT_SOURCE_ADDR (-410)
#define XB_AT_E_LOW_DEST_ADDR (-411)
#define XB_AT_E_EXIT (-412)
#define XB_AT_E_WRITE_CONFIG (-413)

#define XB_SER_E_SUCCESS 500
#define XB_SER_E_NOT_FOUND (-501)
#define XB_SER_E_OPEN (-502)
#define XB_SER_E_PARAM (-503)
#define XB_SER_E_UKN_BAUDRATE (-504)
#define XB_SER_E_CONFIG (-505)
#define XB_SER_E_TIMOUT (-506)
#define XB_SER_E_UKN_DATABITS (-507)
#define XB_SER_E_UKN_STOPBITS (-508)
#define XB_SER_E_UKN_PARITY (-509)


#define XB_AT_CMD_ENTER          "+++"
#define XB_AT_CMD_EXIT           "ATCN"
#define XB_AT_CMD_WRITE_CONFIG   "ATWR"

#define XB_AT_CMD_API            "ATAP"
#define XB_AT_V_API              "0\r"

#define XB_AT_CMD_BAUDRATE       "ATBD"
#define XB_AT_V_BAUDRATE         "3\r"

#define XB_AT_CMD_PARITY         "ATNB"
#define XB_AT_V_PARITY           "0\r"

#define XB_AT_CMD_AES            "ATEE"
#define XB_AT_V_AES              "1\r"

#define XB_AT_CMD_AES_KEY        "ATKY"
#define XB_AT_V_AES_KEY          "32303032\r"

#define XB_AT_CMD_CHANEL         "ATCH"
#define XB_AT_V_CHANEL           "C\r"

#define XB_AT_CMD_PAN_ID         "ATID"
#define XB_AT_V_PAN_ID           "3332\r"

#define XB_AT_CMD_COORDINATOR    "ATCE"
#define XB_AT_V_COORDINATOR      "1\r"
#define XB_AT_V_END_DEVICE       "0\r"

#define XB_AT_CMD_SOURCE_ADDR    "ATMY"

#define XB_AT_CMD_LOW_DEST_ADDR  "ATDL"
#define XB_AT_V_LOW_DEST_ADDR    "FFFF\r"

#define XB_AT_CMD_HIGH_DEST_ADDR  "ATDH"
#define XB_AT_V_HIGH_DEST_ADDR    "0\r"


#define XB_AT_R_EMPTY    ""
#define XB_AT_R_SUCCESS  "OK\r"
#define XB_AT_V_ENDL     "\r"
#define XB_AT_M_GET      false
#define XB_AT_M_SET      true
#define XB_V_START       0x7E


#define XB_FRAME_DEFAULT_LENGTH 8
#define XB_FRAME_DATA_SHIFT     6
#define XB_FRAME_MAX_SIZE       255

typedef struct {
    uint8_t startDelimiter;
    uint8_t length;
    uint8_t emitAddress;
    uint8_t receiverAddress;
    uint8_t frameId;
    uint8_t functionCode;
    uint8_t *data;
    uint16_t checksum;
} xbee_frame_t;


#endif
