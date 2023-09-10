/*!
 * @file can.cpp
 * @version 1.2
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier source de la classe Can
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v1)
 */

#include <fcntl.h>
#include <cstring>
#include <csignal>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bits/ioctls.h>
#include <sys/poll.h>
#include <vector>

#include "can.h"


inline void printError(Logger &logger) {
    // errno = dernier code d'erreur
    logger << " (" << strerror(errno) << ")" << std::endl;
}


int Can::init(can_address_t myAddress) {
    // Création du socket en mode non-bloquant
    address = myAddress;
    socket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    fcntl(socket, F_SETFL, O_NONBLOCK);

    // Vérification de la création du socket
    ifreq ifr{};
    sockaddr_can addr{};
    strcpy(ifr.ifr_name, CAN_INTERFACE);

    if (::ioctl(socket, SIOCGIFFLAGS, &ifr) < 0) {
        logger(CRITICAL) << "Impossible de récupérer les flags de l'interface " << CAN_INTERFACE;
        printError(logger);
        return -1;
    }

    // Vérification de l'état de l'interface
    if ((ifr.ifr_flags & IFF_UP) == 0) {
        logger(ERROR) << "Interface " << CAN_INTERFACE << " down" << std::endl;
        return -1;
    }

    // Récupération de l'adresse Hardware de l'interface
    if (::ioctl(socket, SIOCGIFHWADDR, &ifr) < 0) {
        logger(CRITICAL) << "Impossible de récupérer l'adresse Hardware de l'interface " << CAN_INTERFACE;
        printError(logger);
        return -1;
    }

    logger(INFO) << "Adresse Hardware de l'interface " << CAN_INTERFACE << " : ";
    for (int i = 0; i < 6; i++) {
        logger << std::hex << std::showbase << (int) ifr.ifr_hwaddr.sa_data[i] << " ";
    }
    logger << std::dec << std::endl;

    // Récupération de l'index de l'interface
    if (::ioctl(socket, SIOCGIFINDEX, &ifr) < 0) {
        logger(CRITICAL) << "Impossible de récupérer l'index de l'interface " << CAN_INTERFACE;
        printError(logger);
        return -1;
    }

    // Bind du socket à l'interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
        logger(CRITICAL) << "Impossible de bind le socket";
        printError(logger);
        return -1;
    }

    logger(INFO) << "Bus CAN initialisé" << std::endl;
    return 0;
}


void Can::print(const can_message_t &frame) {
    logger(INFO) << "Message reçu :\n" << std::hex << std::showbase
           << "  - Adresse émetteur : " << (int) frame.senderAddress << "\n"
           << "  - Adresse récepteur : " << (int) frame.receiverAddress << "\n"
           << "  - Code fonction : " << (int) frame.functionCode << "\n"
           << "  - ID message : " << (int) frame.messageID << "\n"
           << "  - Données : ";

    for (int i = 0; i < frame.length; i++)
        logger << std::hex << (int) frame.data[i] << " ";
    logger << std::dec << std::endl;
}


int Can::startListening() {
    if (isListening) {
        logger(WARNING) << "Le socket est déjà en écoute" << std::endl;
        return -1;
    }

    isListening = true;
    listenerThread = std::make_unique<std::thread>(&Can::listen, this);

    logger(INFO) << "Le bus CAN est sous écoute" << std::endl;
    return 0;
}


void Can::listen() {
    std::vector<struct pollfd> fds{
            { socket, POLLIN, 0 }
    };

    int status;
    can_frame buffer{};
    can_message_t frame{};

    while (!isListening.load()) {
        status = poll(&fds[0], 1, -1);

        if (status < 0 && errno == EINTR) {
            logger(ERROR) << "Thread d'écoute interrompu";
            printError(logger);
            continue;
        } else if (status < 0) {
            logger(ERROR) << "Erreur lors de l'écoute du bus CAN";
            printError(logger);
            continue;
        }

        // Lecture du buffer et formatage du message
        if (fds[0].revents&POLLIN && readBuffer(frame, buffer) < 0)
            continue;

        print(frame);
        auto callback = callbacks.find(frame.functionCode);

        if (callback != callbacks.end()) {
            callback->second(frame);
            continue;
        }

        if (frame.isResponse) {
            std::lock_guard<std::mutex> lock(mutex);
            responses[frame.messageID] = frame;
        }

        logger(WARNING) << "Code fonction non traité : " << frame.functionCode << std::endl;
    }
}


int Can::readBuffer(can_message_t &frame, can_frame &buffer) {
    // Lecture du buffer
    if (::read(socket, &buffer, sizeof(struct can_frame)) < 0) {
        logger(ERROR) << "Impossible de lire le buffer";
        printError(logger);
        return -1;
    }

    if (buffer.can_dlc > 8) {
        logger(WARNING) << "Taille du message trop grande : " << buffer.can_dlc << std::endl;
        return -1;
    }

    // Filtrage du message
    frame.receiverAddress = (buffer.can_id & CAN_MASK_RECEIVER_ADDR) >> CAN_OFFSET_RECEIVER_ADDR;

    if (address != frame.receiverAddress && frame.receiverAddress != CAN_ADDR_BROADCAST) {
        return -1;
    }

    frame.senderAddress   = (buffer.can_id & CAN_MASK_EMIT_ADDR) >> CAN_OFFSET_EMIT_ADDR;
    frame.functionCode    = (buffer.can_id & CAN_MASK_FUNCTION_CODE) >> CAN_OFFSET_FUNCTION_CODE;
    frame.messageID       = (buffer.can_id & CAN_MASK_MESSAGE_ID) >> CAN_OFFSET_MESSAGE_ID;
    frame.isResponse      = buffer.can_id & CAN_MASK_IS_RESPONSE;

    // Copie des données
    frame.length = buffer.can_dlc;
    memcpy(frame.data, buffer.data, buffer.can_dlc);

    return 0;
}


int Can::send(uint8_t dest, uint8_t functionCode, uint8_t *data, uint8_t length, uint8_t messageID, bool isResponse) {
    if (length > 8) {
        logger(WARNING) << "Taille du message trop grande : " << length << std::endl;
        return -1;
    }

    can_frame buffer{};
    buffer.len = length;
    memcpy(buffer.data, data, length);

    buffer.can_id = CAN_ADDR_RASPBERRY << CAN_OFFSET_EMIT_ADDR |
                    dest << CAN_OFFSET_RECEIVER_ADDR |
                    functionCode << CAN_OFFSET_FUNCTION_CODE |
                    messageID << CAN_OFFSET_MESSAGE_ID |
                    isResponse;

    if (::write(socket, &buffer, sizeof(struct can_frame)) < 0) {
        logger(ERROR) << "Impossible d'écrire dans le buffer";
        printError(logger);
        return -1;
    }

    logger(INFO) << "Message envoyé : " << std::showbase << std::hex << buffer.can_id << std::endl;
    return 0;
}


void Can::bind(uint8_t functionCode, can_callback callback) {
    callbacks[functionCode] = callback;
}


Can::~Can() {
    isListening.store(false);
    listenerThread->join();
    ::close(socket);
    logger(INFO) << "Arrêt de l'écoute CAN" << std::endl;
}
