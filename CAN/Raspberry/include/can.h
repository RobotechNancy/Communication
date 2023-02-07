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
#include <thread>
#include <cstring>
#include <net/if.h>
#include <iostream>
#include <linux/can.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can/raw.h>

#include "can_vars.h"
#include "robotech/logs.h"


/*!
 * @class Can
 * @brief Classe permettant de gérer la communication CAN
 */
class Can {
private:
    int sock;
    std::thread *listen_thread;
    std::map<uint8_t, can_mess_t> messages;

    [[noreturn]] void listen();
    int process_frame(can_mess_t &response, can_frame frame) const;
    void process_resp(can_mess_t &response);
public:
    Can();
    Logger logger;
    can_mess_t get_message(uint8_t id);

    int init(CAN_EMIT_ADDR emit_addr);
    void start_listen();
    int send(CAN_ADDR addr, CAN_FCT_CODE fct_code, uint8_t data[], uint8_t data_len, bool is_rep, uint8_t rep_len, uint8_t msg_id);
};

#endif //RASPBERRY_H
