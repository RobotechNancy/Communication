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
#include <memory>
#include <thread>
#include <atomic>
#include <linux/can.h>
#include <robotech/logs.h>

#include "define_can.h"


// Type des fonctions de callback
typedef void (*can_callback)(const can_message_t &frame);

/*!
 * @brief Classe permettant de gérer le bus CAN
 * @details Cette classe permet d'initialiser le bus CAN, de l'écouter et d'envoyer des messages
 */
class Can {
public:
    int init(can_address_t address);
    ~Can();


    int startListening();
    void print(const can_message_t &frame);
    void bind(uint8_t functionCode, can_callback callback);
    int waitFor(can_message_t &frame, uint8_t responseId, uint32_t timeout);
    int send(uint8_t address, uint8_t functionCode, uint8_t *data, uint8_t length, uint8_t messageID, bool isResponse);
private:
    int socket{};
    can_address_t address{};
    Logger logger{"CAN", "can.log"};

    std::mutex mutex;                                     // Mutex pour éviter les problèmes de concurrence
    std::map<uint8_t, can_message_t> responses;

    std::atomic<bool> isListening{false};                 // Atomic pour éviter les problèmes de concurrence
    std::map<uint8_t, can_callback> callbacks;
    std::unique_ptr<std::thread> listenerThread{nullptr}; // unique_ptr pour pouvoir delete le thread au destructeur

    void listen();
    int readBuffer(can_message_t& frame, can_frame &buffer);
};


#endif //RASPI_CAN_H
