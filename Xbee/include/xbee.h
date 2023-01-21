/*!
 \file    xbeelib.h
 \brief   Fichier d'en-tête de la classe XBee. Cette classe est utilisée afin de programmer les modules XBee en UART et de mettre en place des communications entre différents modules XBee.
 \author  Samuel-Charles DITTE-DESTREE (samueldittedestree@protonmail.com)
 \version 3.0
 \date    10/03/2022
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

#include "../include/xbee_vars.h"
#include "../include/serialib.h"

/*!
 * \typedef  Trame_t
 * \brief    Format des trames reçues en fonction des paramètres de la trame
 */
typedef struct {
    int start_seq; /*!< Caractère de début de trame */
    int adr_emetteur; /*!< Adresse de l'émetteur de la trame */
    int adr_dest; /*!< Adresse du destinataire de la trame */
    int id_trame_low; /*!< Bits de poids faible de l'ID de la trame */
    int id_trame_high; /*!< Bits de poids fort de l'ID de la trame */
    int nb_octets_msg; /*!< Nombre d'octets du champ data + code fonction */
    int code_fct; /*!< Code fonction de la trame */
    std::vector<int> param; /*!< Data de la trame */
    int crc_low; /*!< Bits de poids faible du CRC */
    int crc_high; /*!< Bits de poids fort du CRC */
    int end_seq; /*!< Caractère de fin de trame */
} Trame_t;

/*!  \class  XBee
 *   \brief  Cette classe est utilisée pour la communication entre un module XBee et une RaspberryPi et entre plusieurs modules XBee.
 */
class XBee {
public:
    XBee();
    ~XBee();

    int openSerialConnection(int mode = 0);
     void closeSerialConnection();

    int checkATConfig();
     bool readATResponse(const char *value = XB_AT_R_EMPTY, int mode = 0);
     bool sendATCommand(const char *command, const char *value, unsigned int mode = XB_AT_M_SET);
     bool writeATConfig();

    int sendTrame(uint8_t ad_dest, uint8_t code_fct, char *data = nullptr);
     void sendMsg(const std::string &msg);

    void waitForATrame();
    [[noreturn]] void sendHeartbeat();
    [[noreturn]] int isXbeeResponding();

     std::string charToString(char *message);
     char *stringToChar(const std::string &chaine);
     void print(const std::vector<int> &v);
private:
    serialib serial;
    Logger logger;

    int MODE = 0;
    int ID_TRAME = 0;
    int trames_envoyees[100]{};

    bool enterATMode();
    bool exitATMode();

    bool discoverXbeeNetwork();

    bool isExpCorrect(int exp);
    bool isDestCorrect(int dest);
    bool isCodeFctCorrect(int code_fct);
    bool isTrameSizeCorrect(std::vector<int> trame);


    int subTrame(std::vector<int> msg_recu);
    int processCodeFct(int code_fct, int exp);
    void afficherTrameRecue(const Trame_t &trame);
    int processTrame(std::vector<int> trame);


     std::vector<int> readBuffer();
     std::string readString();

     int crc16(const int trame[], uint8_t taille);
     bool isCRCCorrect(uint8_t crc_low, uint8_t crc_high, int trame[], int trame_size);

     bool isStartSeqCorrect(int value);
     bool isEndSeqCorrect(int value);

     void delay(unsigned int time);
     std::vector<int> slice(const std::vector<int> &v, int a, int b);
};

#endif // XBEE_H
