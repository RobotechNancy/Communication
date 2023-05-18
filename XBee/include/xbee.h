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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <iostream>
#include <iterator>
#include <functional>

#include "serialib.h"
#include "xbee_vars.h"
#include "robotech/logs.h"


/*!
 * @typedef  xbee_frame_t
 * @brief    <br>Format des trames reçues en fonction des paramètres de la trame
 */
typedef struct {
    uint8_t start_seq;          /*!< Caractère de début de trame */
    uint8_t adr_emetteur;       /*!< Adresse de l'émetteur de la trame */
    uint8_t adr_dest;           /*!< Adresse du destinataire de la trame */
    uint8_t id_trame_low;       /*!< Bits de poids faible de l'ID de la trame */
    uint8_t id_trame_high;      /*!< Bits de poids fort de l'ID de la trame */
    uint8_t data_len;           /*!< Nombre d'octets du champ data + code fonction */
    uint8_t code_fct;           /*!< Code fonction de la trame */
    std::vector<uint8_t> data; /*!< Data de la trame */
    uint8_t crc_low;            /*!< Bits de poids faible du CRC */
    uint8_t crc_high;           /*!< Bits de poids fort du CRC */
    uint8_t end_seq;            /*!< Caractère de fin de trame */
} xbee_frame_t;

// Type d'une fonction qui gère un code fonction
typedef std::function<void(const xbee_frame_t&)> xbee_callback_t;


/*!  @class  XBee
 *   @brief  Classe pour communiquer avec les modules XBee en UART
 */
class XBee {
public:
    explicit XBee(uint8_t addr);
    ~XBee() = default;

    int openSerialConnection(const char* port);
    void closeSerialConnection();
    static void delay(float seconds);

    void start_listen();
    xbee_frame_t wait_for_response(uint8_t fct_code, uint32_t timeout);
    void subscribe(uint8_t fct_code, const xbee_callback_t& callback);
    int sendFrame(uint8_t dest, uint8_t fct_code, const std::vector<uint8_t>& data, uint8_t data_len = 1);
private:
    serialib serial;
    Logger logger;

    int nb_trame = 0;
    uint8_t module_addr;
    std::map<uint8_t, xbee_frame_t> responses;

    std::atomic<bool> is_listening;
    std::unique_ptr<std::thread> listen_thread;
    std::map<uint32_t, xbee_callback_t> listeners;

    bool enterATMode();
    bool exitATMode();
    int checkATConfig();
    bool writeATConfig();
    bool readATResponse(const char *value = XB_AT_R_EMPTY, int mode = 0);
    bool sendATCommand(const char *command, const char *value, unsigned int mode = XB_AT_M_SET);

    void listen();
    int processResponse(const std::vector<uint8_t> &response);
    int processSubFrame(std::vector<uint8_t> &recv_msg);
    int processFrame(std::vector<uint8_t> recv_frame);

    template<typename T> static void printFrame(const T &frame, int data_len);
    static int computeCRC(const int frame[], uint8_t frame_len);
    static bool validateCRC(uint8_t crc_low, uint8_t crc_high, int frame[], int frame_len);

    template<typename T> void readRx(T &buffer);
};

#endif // XBEE_H