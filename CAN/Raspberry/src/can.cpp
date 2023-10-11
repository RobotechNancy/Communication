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
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bits/ioctls.h>

#include "can.h"


inline void printError(Logger &logger, Log level = CRITICAL, const std::string_view &message = "") {
    // errno = dernier code d'erreur
    logger(level) << message << " (" << strerror(errno) << ")" << std::endl;
}


int CAN::init(can_address_t myAddress) {
    // Création du socket en mode non-bloquant
    address = myAddress;
    socket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    fcntl(socket, F_SETFL, O_NONBLOCK);

    // Vérification de la création du socket
    ifreq ifr{};
    sockaddr_can addr{};
    strcpy(ifr.ifr_name, CAN_INTERFACE);

    if (::ioctl(socket, SIOCGIFFLAGS, &ifr) < 0) {
        printError(logger, CRITICAL, "Impossible de récupérer les flags de l'interface");
        return -1;
    }

    // Vérification de l'état de l'interface
    if ((ifr.ifr_flags & IFF_UP) == 0) {
        printError(logger, ERROR, "L'interface est down");
        return -1;
    }

    // Récupération de l'adresse Hardware de l'interface
    if (::ioctl(socket, SIOCGIFHWADDR, &ifr) < 0) {
        printError(logger, CRITICAL, "Impossible de récupérer l'adresse Hardware de l'interface");
        return -1;
    }

    logger(INFO) << "Adresse Hardware de l'interface " << CAN_INTERFACE << " : ";
    for (int i = 0; i < 6; i++)
        logger << std::hex << std::showbase << (int) ifr.ifr_hwaddr.sa_data[i] << " ";
    logger << std::dec << std::endl;

    // Récupération de l'index de l'interface
    if (::ioctl(socket, SIOCGIFINDEX, &ifr) < 0) {
        printError(logger, CRITICAL, "Impossible de récupérer l'index de l'interface");
        return -1;
    }

    // Bind du socket à l'interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(socket, (sockaddr *) &addr, sizeof(addr)) < 0) {
        printError(logger, CRITICAL, "Impossible de bind le socket");
        return -1;
    }

    logger(INFO) << "Bus CAN initialisé" << std::endl;
    return 0;
}


void CAN::print(const can_frame_t &frame) {
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


int CAN::startListening() {
    if (isListening) {
        logger(WARNING) << "Le socket est déjà en écoute" << std::endl;
        return -1;
    }

    isListening = true;
    listenerThread = std::make_unique<std::thread>(&CAN::listen, this);

    logger(INFO) << "Le bus CAN est sous écoute" << std::endl;
    return 0;
}


void CAN::listen() {
    // "socket" correspond à un descripteur de fichier (fd), c'est-à-dire,
    // un entier qui indique comment accéder à une ressource et à quoi elle correspond.
    fd_set fds{};
    timeval timeout{};

    int status;
    can_frame buffer{};
    can_frame_t frame{};

    while (isListening.load()) {
        // Réinitialisation du set de descripteurs
        FD_ZERO(&fds);
        FD_SET(socket, &fds);

        // On regarde si des données sont disponibles
        status = ::select(socket + 1, &fds, nullptr, nullptr, &timeout);

        if (status < 0) {
            printError(logger, ERROR, "Erreur lors de l'écoute du bus CAN");
            continue;
        }

        // Lecture du buffer et formatage du message
        if (status == 0 || readBuffer(frame, buffer) < 0)
            continue;

        // On affiche le message et on le traite
        print(frame);

        // Si c'est une réponse, on bloque l'accès à responses dans d'autres threads
        if (frame.isResponse) {
            std::lock_guard<std::mutex> lock(mutex);
            responses[frame.messageID] = frame;
            continue;
        }

        auto callback = callbacks.find(frame.functionCode);

        if (callback != callbacks.end()) {
            callback->second(*this, frame);
            continue;
        }

        logger(WARNING) << "Aucun callback pour le code fonction " << (int) frame.functionCode << std::endl;
    }
}


int CAN::readBuffer(can_frame_t &frame, can_frame &buffer) {
    // Lecture du buffer
    if (::read(socket, &buffer, sizeof(struct can_frame)) < 0) {
        printError(logger, ERROR, "Impossible de lire le buffer");
        return -1;
    }

    // dlc = data length code (taille des données)
    if (buffer.can_dlc > 8) {
        logger(WARNING) << "Taille du message trop grande : " << buffer.can_dlc << std::endl;
        return -1;
    }

    // On filtre pour n'avoir que la partie qui correspond à l'adresse du récepteur
    // et on la décale pour avoir la vraie valeur
    frame.receiverAddress = (buffer.can_id & CAN_MASK_RECEIVER_ADDR) >> CAN_OFFSET_RECEIVER_ADDR;

    if (address != frame.receiverAddress && frame.receiverAddress != CAN_ADDR_BROADCAST) {
        return -1;
    }

    // Même démarche pour les autres champs
    frame.senderAddress   = (buffer.can_id & CAN_MASK_EMIT_ADDR) >> CAN_OFFSET_EMIT_ADDR;
    frame.functionCode    = (buffer.can_id & CAN_MASK_FUNCTION_CODE) >> CAN_OFFSET_FUNCTION_CODE;
    frame.messageID       = (buffer.can_id & CAN_MASK_MESSAGE_ID) >> CAN_OFFSET_MESSAGE_ID;
    frame.isResponse      = buffer.can_id & CAN_MASK_IS_RESPONSE;

    // Copie des données
    frame.length = buffer.can_dlc;
    memcpy(frame.data, buffer.data, buffer.can_dlc);

    return 0;
}


can_result_t CAN::send(
        uint8_t dest, uint8_t functionCode, const std::vector<uint8_t> &data,
        uint8_t messageID, bool isResponse, int timeout // timeout si on attend une réponse
) {
    if (data.size() > 8) {
        logger(WARNING) << "Taille du message trop grande : " << data.size() << std::endl;
        return {CAN_ERROR};
    }

    can_frame buffer{};
    buffer.len = data.size();
    memcpy(buffer.data, data.data(), data.size());

    buffer.can_id = address << CAN_OFFSET_EMIT_ADDR |
                    dest << CAN_OFFSET_RECEIVER_ADDR |
                    functionCode << CAN_OFFSET_FUNCTION_CODE |
                    messageID << CAN_OFFSET_MESSAGE_ID |
                    isResponse;

    if (::write(socket, &buffer, sizeof(struct can_frame)) < 0) {
        logger(ERROR) << "Impossible d'écrire dans le buffer";
        printError(logger);
        return {CAN_ERROR};
    }

    logger(INFO) << "Message envoyé : " << std::showbase << std::hex << buffer.can_id << std::endl;
    if (timeout == 0)
        return {CAN_OK};

    auto start = std::chrono::steady_clock::now();
    std::chrono::milliseconds timeoutMs(timeout*1000);

    while (std::chrono::steady_clock::now() - start < timeoutMs) {
        std::lock_guard<std::mutex> lock(mutex);
        auto response = responses.find(messageID);

        // Si on a reçu une réponse, on la supprime de la liste et on la retourne
        if (response != responses.end()) {
            responses.erase(response);
            return {CAN_OK, response->second};
        }
    }

    // Aucune réponse reçue à temps
    return {CAN_TIMEOUT};
}


void CAN::bind(uint8_t functionCode, can_callback_t callback) {
    callbacks[functionCode] = callback;
}


CAN::~CAN() {
    // On arrête l'écoute du bus CAN que si elle a été démarrée
    if (listenerThread == nullptr)
        return;

    isListening.store(false);
    listenerThread->join();
    ::close(socket);
    logger(INFO) << "Arrêt de l'écoute CAN" << std::endl;
}
