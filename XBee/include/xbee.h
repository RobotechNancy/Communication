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
#include <mutex>
#include <memory>
#include <thread>
#include <atomic>
#include <iterator>
#include <robotech/logs.h>

#include "define_xbee.h"


struct xbee_frame_t {
    uint8_t receiverAddress;
    uint8_t emitterAddress;
    uint8_t functionCode;
    uint8_t frameId;
    std::vector<uint8_t> data;
};

struct xbee_result_t {
    int status = XB_E_SUCCESS;
    xbee_frame_t frame;
};

// Fonction qui retourne rien et qui accepte un xbee_frame_t en paramètre
typedef void(*xbee_callback_t)(const xbee_frame_t &frame);


class XBee {
public:
    explicit XBee(uint8_t addr): logger("xbee"), address(addr) {};
    int open(const char* port);
    ~XBee();

    void startListening();
    void printBuffer(const std::vector<uint8_t> &frame);
    void bind(uint8_t functionCode, xbee_callback_t callback) { callbacks[functionCode] = callback; };
    xbee_result_t send(uint8_t dest, uint8_t functionCode, const std::vector<uint8_t> &data, int timeout = 0);
private:
    serialib serial;                                  // Librairie de Philippe Lucidarme
    Logger logger;                                    // Librairie custom, disponible dans le repo Communication

    uint8_t address;
    int totalFrames = 0;

    std::mutex responseMutex;                         // Mutex pour éviter les accès concurrents à responses
    std::map<uint8_t, xbee_frame_t> responses;

    std::atomic<bool> isListening{false};             // Atomic pour éviter les accès concurrents à isListening
    std::unique_ptr<std::thread> listenerThread;      // Pointeur unique pour que la mémoire soit automatiquement libérée
    std::map<uint8_t, xbee_callback_t> callbacks;

    bool enterATMode();
    bool exitATMode();
    int checkATConfig();
    bool writeATConfig();
    bool readATResponse(const char *value = XB_AT_R_EMPTY, uint16_t timeout = 100);
    bool sendATCommand(const char *command, const char *value, bool mode = XB_AT_M_SET);

    void listen();
    int processBuffer(const std::vector<uint8_t> &response);
    int processFrame(const std::vector<uint8_t> &buffer);
    template<typename T> void readRx(T &buffer, unsigned int timeout = 100);
    static uint16_t computeChecksum(const std::vector<uint8_t> &frame, uint8_t start, uint8_t length);
};

#endif // XBEE_H