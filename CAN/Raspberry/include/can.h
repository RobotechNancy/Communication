/*!
 * @file can.h
 * @version 1.0
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier d'en-tête de la classe Can
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v4.1a)
 */

#ifndef RASPBERRY_H
#define RASPBERRY_H

#include <map>
#include <memory>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <functional>

#include <net/if.h>
#include <iostream>
#include <linux/can.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can/raw.h>

#include "can_vars.h"
#include "robotech/logs.h"


// Type d'une fonction qui gère un code fonction
typedef std::function<void(const can_mess_t&)> can_callback_t;

/*!
 * @class Can
 * @brief Classe permettant de gérer la communication CAN
 */
class Can {
private:
    int sock;
    std::atomic<bool> is_listening;
    std::map<uint32_t, can_mess_t> responses;
    std::unique_ptr<std::thread> listen_thread;
    std::map<uint32_t, can_callback_t> listeners;

    void listen();
    int format_frame(can_mess_t &response, can_frame& frame) const;
public:
    Logger logger;
    uint8_t emit_addr;

    explicit Can(uint32_t emit_addr);
    int init();

    void subscribe(uint32_t fct_code, const can_callback_t& callback);
    int send(CAN_ADDR addr, CAN_FCT_CODE fct_code, uint8_t data[], uint8_t data_len, bool is_rep, uint8_t rep_len, uint8_t msg_id);

    void start_listen();
    can_mess_t wait_for_response(CAN_FCT_CODE fct_code, uint32_t timeout);
    void close();

};

#endif //RASPBERRY_H
