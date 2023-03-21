/*!
 * @file    xbee.h
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier d'en-tête de la classe XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#ifndef XBEE_H
#define XBEE_H

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <thread>
#include <iterator>
#include <robotech/logs.h>
#include <map>

#include "serialib.h"
#include "xbee_vars.h"


typedef struct {
    int id;
    double x;
    double y;
    double z;
} aruco_t;


/*!
 * @typedef  frame_t
 * @brief    <br>Format des trames reçues en fonction des paramètres de la trame
 */
typedef struct {
    int start_seq;          /*!< Caractère de début de trame */
    int adr_emetteur;       /*!< Adresse de l'émetteur de la trame */
    int adr_dest;           /*!< Adresse du destinataire de la trame */
    int id_trame_low;       /*!< Bits de poids faible de l'ID de la trame */
    int id_trame_high;      /*!< Bits de poids fort de l'ID de la trame */
    int nb_octets_msg;      /*!< Nombre d'octets du champ data + code fonction */
    int code_fct;           /*!< Code fonction de la trame */
    std::vector<int> param; /*!< Data de la trame */
    int crc_low;            /*!< Bits de poids faible du CRC */
    int crc_high;           /*!< Bits de poids fort du CRC */
    int end_seq;            /*!< Caractère de fin de trame */
} frame_t;


/*!  @class  XBee
 *   @brief  Classe pour communiquer avec les modules XBee en UART
 */
class XBee {
public:
    XBee();
    ~XBee();

    int openSerialConnection();
    void closeSerialConnection();

    int checkATConfig();
    bool readATResponse(const char *value = XB_AT_R_EMPTY, int mode = 0);
    bool sendATCommand(const char *command, const char *value, unsigned int mode = XB_AT_M_SET);
    bool writeATConfig();

    int sendFrame(uint8_t dest, uint8_t fct_code, const char *data = nullptr, int data_len = 1);
    [[noreturn]] void waitForATrame();
private:
    serialib serial;
    Logger logger;

    int nb_trame = 0;
    std::vector<aruco_t> aruco_tags;

    bool enterATMode();
    bool exitATMode();
    bool discoverXbeeNetwork();

    int subTrame(std::vector<int> recv_msg);
    int processFctCode(int fct_code, int exp, std::vector<int> data);
    int processFrame(std::vector<int> recv_frame);
    static void printFrame(const frame_t &trame);

    static int computeCRC(const int frame[], uint8_t frame_len);
    static bool isCRCCorrect(uint8_t crc_low, uint8_t crc_high, int frame[], int frame_len);

    template<typename T> void readRx(T &buffer);
    static void delay(float seconds);
};

#endif // XBEE_H
