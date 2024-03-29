/*!
 * @file can.cpp
 * @version 1.3
 * @date 2023-2024
 * @author Romain ADAM
 * @brief Fichier source de la classe Can
 * @details Version modifiée de la librairie de Julien PISTRE (v1.2)
 */

#include <fcntl.h>
#include <cstring>
#include <linux/can.h>
#include <net/if.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bits/ioctls.h>
#include <unistd.h>

#include "../include/can.h"


inline void printError(Logger &logger, Log level = CRITICAL, const std::string_view &message = "") {
    // errno = dernier code d'erreur
    logger(level) << message << " (" << strerror(errno) << ")" << std::endl;
}


int CAN::init(CanBus_Address myAddress) {
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
        logger(INFO) << CAN_INTERFACE << std::endl;
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


void CAN::print(const CanBus_FrameFormat &frame) {
    logger(INFO) << "Message reçu :\n" << std::hex << std::showbase
    	   << "  - Priorité : " << (int) frame.Priority << "\n"
           << "  - Adresse émetteur : " << (int) frame.SenderAddress << "\n"
           << "  - Adresse récepteur : " << (int) frame.ReceiverAddress << "\n"
           << "  - Mode Fonction : " << (int) frame.FunctionMode << "\n"
           << "  - Code fonction : " << (int) frame.FunctionCode << "\n"
           << "  - ID message : " << (int) frame.MessageID << "\n"
           << "  - Données : ";

    for (int i = 0; i < frame.Length; i++)
        logger << std::hex << (int) frame.Data[i] << " ";
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
    // "socket" est un entier qui indique comment accéder à une ressource et à quoi elle correspond (file descriptor)
    // On utilise un pollfd pour vérifier de manière non-bloquante si des données sont disponibles
    pollfd fd{ socket, POLLIN, 0 };

    int status;
    can_frame buffer{};
    CanBus_FrameFormat frame{};

    while (isListening.load()) {
        // On regarde si des données sont disponibles, 0 => pas de timeout
        status = ::poll(&fd, 1, 0);

        // "status == 0" => timeout, "status < 0" => erreur
        if (status == 0)
            continue;
        else if (status < 0) {
            printError(logger, ERROR, "Erreur lors de l'écoute du bus CAN");
            continue;
        }
	
	//Affichage de la trame avant traitement
	logger(INFO) << "ID et Data de la trame : " << (buffer.can_id ^ CAN_EFF_FLAG) << buffer.data << std::endl;
	
        // Traitement du buffer
        if (readBuffer(frame, buffer) < 0)
            continue;

        // On affiche le message et on le traite
        print(frame);

        // Si c'est une réponse, on bloque l'accès à responses dans d'autres threads
        if (frame.IsResp) {
            std::lock_guard<std::mutex> lock(mutex);
            responses[frame.MessageID] = frame;
            continue;
        }

        auto callback = callbacks.find(frame.FunctionCode);

        if (callback != callbacks.end()) {
            callback->second(*this, frame);
            continue;
        }

        logger(WARNING) << "Aucun callback pour le code fonction " << (int) frame.FunctionCode << std::endl;
    }
}


int CAN::readBuffer(CanBus_FrameFormat &frame, can_frame &buffer) {
    // Lecture du buffer
    if (::read(socket, &buffer, sizeof(can_frame)) < 0) {
        printError(logger, ERROR, "Impossible de lire le buffer");
        return -1;
    }

    // dlc = Data Length code (taille des données)
    if (buffer.can_dlc > 8) {
        logger(WARNING) << "Taille du message trop grande : " << buffer.can_dlc << std::endl;
        return -1;
    }

    // On filtre pour n'avoir que la partie qui correspond à l'adresse du récepteur
    // et on la décale pour avoir la vraie valeur
    frame.ReceiverAddress = (buffer.can_id & CAN_MASK_RECEIVER_ADDR) >> CAN_OFFSET_RECEIVER_ADDR;

    if (address != frame.ReceiverAddress && frame.ReceiverAddress != CANBUS_BROADCAST) {
        return -1;
    }

    // Même démarche pour les autres champs
    frame.Priority        = (buffer.can_id & CAN_MASK_PRIORITY) >> CAN_OFFSET_PRIORITY;
    frame.SenderAddress   = (buffer.can_id & CAN_MASK_EMIT_ADDR) >> CAN_OFFSET_EMIT_ADDR;
    frame.FunctionMode    = (buffer.can_id & CAN_MASK_FUNCTION_MODE) >> CAN_OFFSET_FUNCTION_MODE;
    frame.FunctionCode    = (buffer.can_id & CAN_MASK_FUNCTION_CODE) >> CAN_OFFSET_FUNCTION_CODE;
    frame.MessageID       = (buffer.can_id & CAN_MASK_MESSAGE_ID) >> CAN_OFFSET_MESSAGE_ID;
    frame.IsResp      = buffer.can_id & CAN_MASK_IS_RESPONSE;

    // Copie des données
    frame.Length = buffer.can_dlc;
    memcpy(frame.Data, buffer.data, buffer.can_dlc);

    return 0;
}


can_result_t CAN::send(
        CanBus_Priority Priority, CanBus_Address dest, CanBus_Fnct_Mode FunctionMode, CanBus_Fnct_Code FunctionCode, const std::vector<uint8_t> &Data,
        uint8_t MessageID, bool IsResp, int timeout // timeout si on attend une réponse
) {
    if (Data.size() > 8) {
        logger(WARNING) << "Taille du message trop grande : " << Data.size() << std::endl;
        return {CAN_ERROR};
    }

    can_frame buffer{};
    buffer.len = Data.size();
    memcpy(buffer.data, Data.data(), Data.size());

    buffer.can_id = Priority     << CAN_OFFSET_PRIORITY      |
                    address      << CAN_OFFSET_EMIT_ADDR     |
                    dest         << CAN_OFFSET_RECEIVER_ADDR |
                    FunctionMode << CAN_OFFSET_FUNCTION_MODE |
                    FunctionCode << CAN_OFFSET_FUNCTION_CODE |
                    MessageID    << CAN_OFFSET_MESSAGE_ID    |
                    IsResp   |  CAN_EFF_FLAG;

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
        auto response = responses.find(MessageID);

        // Si on a reçu une réponse, on la supprime de la liste et on la retourne
        if (response != responses.end()) {
            responses.erase(response);
            return {CAN_OK, response->second};
        }
    }

    // Aucune réponse reçue à temps
    return {CAN_TIMEOUT};
}


void CAN::bind(uint16_t FunctionCode, can_callback_t callback) {
    callbacks[FunctionCode] = callback;
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
