/*!
 * @file can.cpp
 * @version 1.3
 * @date 2023-2024
 * @author Romain ADAM
 * @brief Header de la classe Can
 * @details Version modifiée de la librairie de Julien PISTRE (v1.2)
 */

#ifndef RASPI_CAN_H
#define RASPI_CAN_H

#include <map>
#include <mutex>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <linux/can.h>
#include <robotech/logs.h>

#include "define_can.h"


// Pré-déclaration pour utiliser la classe CAN dans can_callback_t
class CAN;

// Type des fonctions de callback
typedef std::function<void(CAN &can, const CanBus_FrameFormat &frame)> can_callback_t;

// Différents status possibles
enum can_status_t {
    CAN_OK, CAN_ERROR, CAN_TIMEOUT
};

// Structure pour stocker les réponses
struct can_result_t {
    can_status_t status;
    CanBus_FrameFormat frame;
};


class CAN {
public:
    int init(CanBus_Address address);
    ~CAN();

    int startListening();
    void print(const CanBus_FrameFormat &frame);
    void bind(uint16_t FunctionCode, can_callback_t callback);
    can_result_t send(
            CanBus_Priority priority,CanBus_Address dest, CanBus_Fnct_Mode FunctionMode, CanBus_Fnct_Code FunctionCode, const std::vector<uint8_t> &data,
            uint8_t MessageID, bool IsResp, int timeout = 0
    );
private:
    int socket{};
    CanBus_Address address{};
    Logger logger{"CAN", "can.log"};

    std::mutex mutex;                                     // Mutex pour éviter les problèmes de concurrence
    std::map<uint8_t, CanBus_FrameFormat> responses;

    std::atomic<bool> isListening{false};                 // Atomic pour éviter les problèmes de concurrence
    std::map<uint8_t, can_callback_t> callbacks;
    std::unique_ptr<std::thread> listenerThread{nullptr}; // unique_ptr pour pouvoir que la destruction soit automatique

    void listen();
    int readBuffer(CanBus_FrameFormat& frame, can_frame &buffer);
};


#endif //RASPI_CAN_H
