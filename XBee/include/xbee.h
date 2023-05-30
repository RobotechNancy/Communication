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
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>
#include <iterator>
#include <functional>
#include <robotech/logs.h>

#include "define_xbee.h"


class XBee {
public:
    explicit XBee(uint8_t addr);
    int open(const char* port);
    ~XBee();

    void startListening();
    void printFrame(const uint8_t *frame, uint8_t length);
    int send(uint8_t dest, uint8_t functionCode, const uint8_t *data, uint8_t dataLength = 1);
private:
    serialib serial;
    Logger logger;

    uint8_t address;
    int totalFrames = 0;

    std::atomic<bool> isListening{false};
    std::unique_ptr<std::thread> listenerThread;

    std::mutex queueMutex;
    std::vector<xbee_frame_t> queue;
    std::unique_ptr<std::thread> queueThread;

    bool enterATMode();
    bool exitATMode();
    int checkATConfig();
    bool writeATConfig();
    bool readATResponse(const char *value = XB_AT_R_EMPTY, uint16_t timeout = 100);
    bool sendATCommand(const char *command, const char *value, bool mode = XB_AT_M_SET);

    void listen();
    void processQueue();
    int processBuffer(std::vector<uint8_t> &response);
    int processFrame(const uint8_t *buffer, const uint8_t &dataLength);
    static uint16_t computeChecksum(const uint8_t *frame, uint8_t length);
    template<typename T> void readRx(T &buffer, unsigned int timeout = 100);
};

#endif // XBEE_H