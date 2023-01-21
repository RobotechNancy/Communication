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

class Can {
private:
    int sock;
    std::thread *listen_thread;
    std::map<uint8_t, can_message> messages;

    [[noreturn]] void listen();
    int process_frame(can_message &response, can_frame frame) const;
public:
    Can();
    Logger logger;
    can_message get_message(uint8_t id);

    int init(CAN_EMIT_ADDR emit_addr);
    void start_listen();

    void process_resp(can_message &response);
    int send(CAN_ADDR addr, CAN_FCT_CODE fct_code, uint8_t data[], uint8_t data_len, bool is_rep, uint8_t rep_len, uint8_t msg_id);
};

#endif //RASPBERRY_H
