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

#include "can.h"


/*!
 * @brief Affiche le dernier message d'erreur
 * @param logger Logger à utiliser pour l'affichage
 */
inline void printError(Logger &logger) {
    logger << " (" << strerror(errno) << ")" << std::endl;
}


/*!
 * @brief Initialise le bus CAN
 * @param myAddress Adresse CAN pour le filtrage
 * @return 0 si tout s'est bien passé, -1 sinon
 */
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
        logger << "Impossible de récupérer les flags de l'interface " << CAN_INTERFACE;
        printError(logger);
        return -1;
    }

    // Vérification de l'état de l'interface
    if ((ifr.ifr_flags & IFF_UP) == 0) {
        logger << "Interface " << CAN_INTERFACE << " down" << std::endl;
        return -1;
    }

    // Récupération de l'index de l'interface
    if (::ioctl(socket, SIOCGIFINDEX, &ifr) < 0) {
        logger << "Impossible de récupérer l'index de l'interface " << CAN_INTERFACE;
        printError(logger);
        return -1;
    }

    // Bind du socket à l'interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
        logger << "Impossible de bind le socket";
        printError(logger);
        return -1;
    }

    logger << "Bus CAN initialisé" << std::endl;
    return 0;
}


/*!
 * @brief Affiche un message CAN
 * @param frame Message à afficher
 */
void Can::print(const can_message_t &frame) {
    logger << "Message reçu :\n" << std::hex << std::showbase
           << "  - Adresse émetteur : " << (int) frame.senderAddress << "\n"
           << "  - Adresse récepteur : " << (int) frame.receiverAddress << "\n"
           << "  - Code fonction : " << (int) frame.functionCode << "\n"
           << "  - ID message : " << (int) frame.messageID << "\n"
           << "  - Données : ";

    for (int i = 0; i < frame.length; i++)
        logger << std::hex << (int) frame.data[i] << " ";
    logger << std::dec << std::endl;
}


/*!
 * @brief Démarrer l'écoute du bus CAN sur un thread séparé
 * @return 0 si tout s'est bien passé, -1 sinon
 */
int Can::startListening() {
    if (isListening) {
        logger << "Le socket est déjà en écoute" << std::endl;
        return -1;
    }

    isListening = true;
    listenerThread = std::make_unique<std::thread>(&Can::listen, this);

    logger << "Le bus CAN est sous écoute" << std::endl;
    return 0;
}


/*!
 * @brief Boucle d'écoute du bus CAN
 * @details Lecture non bloquante du socket pour pouvoir arrêter l'écoute à tout moment
 */
void Can::listen() {
    fd_set rds{};            // Set de lecture contenant les sockets à écouter
    timeval timeout = {      // Timeout nul pour faire du polling
            .tv_sec = 0,
            .tv_usec = 0
    };

    int status = 0;
    can_frame buffer{};
    can_message_t frame{};

    while (isListening) {
        FD_ZERO(&rds);          // On vide le set de lecture
        FD_SET(socket, &rds);   // On ajoute le socket au set de lecture
        status = ::select(socket + 1, &rds, nullptr, nullptr, &timeout); // On attend qu'un socket soit prêt

        // status < 0 signifie qu'une erreur est survenue
        if (status < 0) {
            logger << "Erreur lors de la lecture du socket";
            printError(logger);
            continue;
        }

        // status == 0 signifie que le timeout est arrivé à expiration
        if (status == 0) {
            continue;
        }

        // Lecture du buffer et formatage du message
        if (readBuffer(frame, buffer) < 0)
            continue;

        print(frame);
        auto callback = callbacks.find(frame.functionCode);

        if (frame.isResponse) {
            mutex.lock();
            responses[frame.messageID] = frame;
            mutex.unlock();
        } else if (callback != callbacks.end()) {
            callback->second(frame);
        } else {
            logger << "Code fonction non traité : " << frame.functionCode << std::endl;
        }
    }
}


int Can::readBuffer(can_message_t &frame, can_frame &buffer) {
    if (::read(socket, &buffer, sizeof(struct can_frame)) < 0) {
        logger << "Impossible de lire le buffer";
        printError(logger);
        return -1;
    }

    if (buffer.can_dlc > 8) {
        logger << "Taille du message trop grande : " << buffer.can_dlc << std::endl;
        return -1;
    }

    frame.receiverAddress = (buffer.can_id & CAN_MASK_RECEIVER_ADDR) >> CAN_OFFSET_RECEIVER_ADDR;

    if (address != frame.receiverAddress && frame.receiverAddress != CAN_ADDR_BROADCAST) {
        return -1;
    }

    frame.senderAddress   = (buffer.can_id & CAN_MASK_EMIT_ADDR) >> CAN_OFFSET_EMIT_ADDR;
    frame.functionCode    = (buffer.can_id & CAN_MASK_FUNCTION_CODE) >> CAN_OFFSET_FUNCTION_CODE;
    frame.messageID       = (buffer.can_id & CAN_MASK_MESSAGE_ID) >> CAN_OFFSET_MESSAGE_ID;
    frame.isResponse      = buffer.can_id & CAN_MASK_IS_RESPONSE;

    for (int i = 0; i < buffer.can_dlc; i++){
        if(buffer.data[i] < 0 || buffer.data[i] > 255) {
            logger << "Valeur du message non valide : " << buffer.data[i] << std::endl;
            return -1;
        }

        frame.data[i] = buffer.data[i];
    }

    frame.length = buffer.can_dlc;
    return 0;
}


int Can::waitFor(can_message_t &frame, uint8_t messageID, uint32_t duration) {
    auto start = std::chrono::steady_clock::now();

    while (true) {
        mutex.lock();
        auto response = responses.find(messageID);

        if (response != responses.end()) {
            frame = response->second;
            responses.erase(response);
            mutex.unlock();
            return 0;
        }

        mutex.unlock();
        uint32_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start
        ).count();

        if (elapsed > duration) {
            logger << "Timeout lors de l'attente d'une réponse" << std::endl;
            return -1;
        }
    }
}


int Can::send(uint8_t dest, uint8_t functionCode, uint8_t *data, uint8_t length, uint8_t messageID, bool isResponse) {
    if (length > 8) {
        logger << "Taille du message trop grande : " << length << std::endl;
        return -1;
    }

    can_frame buffer{};

    buffer.can_dlc = length;
    memcpy(buffer.data, data, length);

    buffer.can_id = CAN_ADDR_RASPBERRY << CAN_OFFSET_EMIT_ADDR |
                    dest << CAN_OFFSET_RECEIVER_ADDR |
                    functionCode << CAN_OFFSET_FUNCTION_CODE |
                    messageID << CAN_OFFSET_MESSAGE_ID |
                    isResponse;

    if (::write(socket, &buffer, sizeof(struct can_frame)) < 0) {
        logger << "Impossible d'écrire dans le buffer";
        printError(logger);
        return -1;
    }

    return 0;
}


void Can::bind(uint8_t functionCode, can_callback callback) {
    callbacks[functionCode] = callback;
}


Can::~Can() {
    if (!isListening) return;

    isListening = false;
    ::close(socket);
    listenerThread->join();
}
