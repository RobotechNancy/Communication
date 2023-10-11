/*!
 * @file can.cpp
 * @version 1.2
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Header de la classe Can
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v1)
 */

#ifndef RASPI_CAN_H
#define RASPI_CAN_H

#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <linux/can.h>
#include <robotech/logs.h>

#include "define_can.h"


// Pré-déclaration pour utiliser la classe CAN dans can_callback_t
class CAN;

// Type des fonctions de callback
typedef void (*can_callback_t)(CAN &can, const can_frame_t &frame);

// Différents status possibles
enum can_status_t {
    CAN_OK, CAN_ERROR, CAN_TIMEOUT
};

// Structure pour stocker les réponses
struct can_result_t {
    can_status_t status;
    can_frame_t frame;
};


class CAN {
public:
    int init(can_address_t address);
    ~CAN();

    int startListening();
    void print(const can_frame_t &frame);
    void bind(uint8_t functionCode, can_callback_t callback);
    can_result_t send(
            uint8_t address, uint8_t functionCode, const std::vector<uint8_t> &data,
            uint8_t messageID, bool isResponse, int timeout = 0
    );
private:
    int socket{};
    can_address_t address{};
    Logger logger{"CAN", "can.log"};

    std::mutex mutex;                                     // Mutex pour éviter les problèmes de concurrence
    std::map<uint8_t, can_frame_t> responses;

    std::atomic<bool> isListening{false};                 // Atomic pour éviter les problèmes de concurrence
    std::map<uint8_t, can_callback_t> callbacks;
    std::unique_ptr<std::thread> listenerThread{nullptr}; // unique_ptr pour pouvoir que la destruction soit automatique

    void listen();
    int readBuffer(can_frame_t& frame, can_frame &buffer);
};


#endif //RASPI_CAN_H
