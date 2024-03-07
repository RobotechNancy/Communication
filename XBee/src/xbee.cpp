/*!
 *  @file    xbee.h
 *  @version 1.0
 *  @date    2022-2023
 *  @author  Julien PISTRE
 *  @brief   Fichier d'en-tête de la classe XBee
 *  @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"


// Ouvrir une connexion série avec le module et configurer les paramètres AT
int XBee::open(const char* port) {
    int status = serial.openDevice(
        port, XB_BAUDRATE_PRIMARY, XB_DATABITS_PRIMARY, XB_PARITY_PRIMARY, XB_STOPBITS_PRIMARY
    );

    if (status != XB_E_SUCCESS) {
        logger(CRITICAL) << "Impossible d'ouvrir la connexion sur le port '" << port
                         << "' (baudrate=" << XB_BAUDRATE_PRIMARY
                         << " et parité=" << XB_PARITY_PRIMARY << ")" << std::endl;

        return status;
    }

    logger(INFO) << "Connexion ouverte avec succès sur le port " << port << std::endl;
    return checkATConfig();
}


int XBee::checkATConfig() {
    // Vérification de la configuration d'un module
    // Pour chaque paramètre, si la valeur n'est pas celle attendue, on la configure

    if (enterATMode())
        logger(INFO) << "Entrée dans le mode AT" << std::endl;
    else {
        logger(CRITICAL) << "Impossible d'entrer dans le mode AT" << std::endl;
        return XB_E_AT_ENTER;
    }

    if (sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE, XB_AT_M_GET))
        logger(INFO) << "Vaudrate vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE))
        logger(INFO) << "Baudrate configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le baudrate" << std::endl;
        return XB_E_AT_BAUDRATE;
    }

    if (sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY, XB_AT_M_GET))
        logger(INFO) << "Nombre de bits de parité vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY))
        logger(INFO) << "Nombre de bits de parité configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer la parité" << std::endl;
        return XB_E_AT_PARITY;
    }

    if (sendATCommand(XB_AT_CMD_API, XB_AT_V_API, XB_AT_M_GET))
        logger(INFO) << "Mode API vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_API, XB_AT_V_API))
        logger(INFO) << "Mode API configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le mode API" << std::endl;
        return XB_E_AT_API;
    }

    if (sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES, XB_AT_M_GET))
        logger(INFO) << "Chiffrement AES vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES))
        logger(INFO) << "Chiffrement AES configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le paramètre de chiffrement AES" << std::endl;
        return XB_E_AT_AES;
    }

    if (sendATCommand(XB_AT_CMD_AES_KEY, XB_AT_V_AES_KEY))
        logger(INFO) << "Clé de chiffrement configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer la clé de chiffrement AES" << std::endl;
        return XB_E_AT_AES_KEY;
    }

    if (sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL, XB_AT_M_GET))
        logger(INFO) << "Canal de découverte réseau vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL))
        logger(INFO) << "Canal de découverte réseau configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le canal de découverte réseau" << std::endl;
        return XB_E_AT_CHANEL;
    }

    if (sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID, XB_AT_M_GET))
        logger(INFO) << "ID du réseau vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID))
        logger(INFO) << "ID du réseau configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'ID du réseau" << std::endl;
        return XB_E_AT_PAN_ID;
    }

    const char* coordinator = (address == 1) ? XB_AT_V_COORDINATOR : XB_AT_V_END_DEVICE;

    if (sendATCommand(XB_AT_CMD_COORDINATOR, coordinator, XB_AT_M_GET))
        logger(INFO) << "Mode coordinateur vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_COORDINATOR, coordinator))
        logger(INFO) << "Mode coordinateur configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le mode coordinateur" << std::endl;
        return XB_E_AT_COORDINATOR;
    }

    char addr[3];
    sprintf(addr, "%d\r", address);

    if (sendATCommand(XB_AT_CMD_SOURCE_ADDR, addr, XB_AT_M_GET))
        logger(INFO) << "Adresse source 16bits vérifiée avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_SOURCE_ADDR, addr))
        logger(INFO) << "Adresse source 16bits configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'adresse source 16bits" << std::endl;
        return XB_E_AT_16BIT_SOURCE_ADDR;
    }

    if (sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR, XB_AT_M_GET))
        logger(INFO) << "Adresse de destination vérifiée avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR))
        logger(INFO) << "Adresse de destination configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'adresse de destination" << std::endl;
        return XB_E_AT_LOW_DEST_ADDR;
    }

    if (sendATCommand(XB_AT_CMD_HIGH_DEST_ADDR, XB_AT_V_HIGH_DEST_ADDR, XB_AT_M_GET))
        logger(INFO) << "Adresse de destination vérifiée avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_HIGH_DEST_ADDR, XB_AT_V_HIGH_DEST_ADDR))
        logger(INFO) << "Adresse de destination configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'adresse de destination" << std::endl;
        return XB_E_AT_HIGH_DEST_ADDR;
    }

    if (writeATConfig())
        logger(INFO) << "Configuration AT enregistrée dans la mémoire du module" << std::endl;
    else {
        logger(CRITICAL) << "Impossible d'écrire les paramètres dans la mémoire flash" << std::endl;
        return XB_E_AT_WRITE_CONFIG;
    }

    if (!exitATMode()) {
        logger(CRITICAL) << "Impossible de sortir du mode AT" << std::endl;
        return XB_E_AT_EXIT;
    }

    serial.flushReceiver();
    logger(INFO) << "Configuration AT réalisée avec succès" << std::endl;
    return XB_E_SUCCESS;
}

bool XBee::sendATCommand(const char *command, const char *value, bool mode) {
    serial.writeString(command);

    if (mode == XB_AT_M_GET) {
        serial.writeString(XB_AT_V_ENDL);
        logger(INFO) << "Envoi de la commande AT : " << command << std::endl;
        return readATResponse(value);
    }

    serial.writeString(value);
    logger(INFO) << "Envoi de la commande AT : " << command << "=" << value << std::endl;
    return readATResponse(XB_AT_R_SUCCESS);
}

bool XBee::readATResponse(const char *value, uint16_t timeout) {
    std::string response;
    readRx(response, timeout);

    serial.flushReceiver();
    logger(INFO) << "Réponse du XBee : " << response << std::endl;

    return (response == value) || (!response.empty() && response != XB_AT_V_ENDL && strcmp(value, XB_AT_V_ENDL) == 0);
}

bool XBee::enterATMode() {
    serial.writeString(XB_AT_CMD_ENTER);

    logger(INFO) << "Entrée en mode AT en cours..." << std::endl;
    return readATResponse(XB_AT_R_SUCCESS, 3000);
}

bool XBee::exitATMode() {
    serial.writeString(XB_AT_CMD_EXIT);
    serial.writeString(XB_AT_V_ENDL);

    logger(INFO) << "Sortie du mode AT" << std::endl;
    return readATResponse(XB_AT_R_SUCCESS);
}

bool XBee::writeATConfig() {
    serial.writeString(XB_AT_CMD_WRITE_CONFIG);
    serial.writeString(XB_AT_V_ENDL);

    logger(INFO) << "Ecriture des paramètres AT dans la mémoire" << std::endl;
    return readATResponse(XB_AT_R_SUCCESS);
}


void XBee::printBuffer(const std::vector<uint8_t> &buffer) {
    for (const uint8_t &byte: buffer)
        logger(INFO) << std::showbase << std::hex << (int) byte << ", ";
    logger(INFO) << std::endl;
}

void XBee::startListening() {
    isListening = true;
    listenerThread = std::make_unique<std::thread>(&XBee::listen, this);
    logger(INFO) << "Thread d'écoute démarré" << std::endl;
}

void XBee::listen() {
    std::vector<uint8_t> response;

    while (isListening) {
        // On attend 10ms pour éviter de surcharger le CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (serial.available() > 0) {
            readRx(response);
            processBuffer(response);
        }
    }
}

// Fonction pour fiabiliser la réception des trames
int XBee::processBuffer(const std::vector<uint8_t> &response) {
    const uint8_t length = response.size();
    const uint8_t dataLength = length - XB_FRAME_MIN_LENGTH;

    logger(INFO) << "Données reçues : ";
    printBuffer(response);

    if (length < XB_FRAME_MIN_LENGTH) {
        logger(WARNING) << "Trame reçue trop petite" << std::endl;
        return XB_E_FRAME_LENGTH;
    }

    // SOH = start of header et EOT = end of transmission
    if (response[0] != XB_FRAME_SOH || response.back() != XB_FRAME_EOT) {
        logger(WARNING) << "Délimitation de la trame invalide" << std::endl;
        return XB_E_FRAME_CORRUPTED;
    }

    // Si tout va bien, response[1] = NOT response[2]
    if (response[1] != (uint8_t) ~response[2]) {
        logger(WARNING) << "Valeur de la longueur corrompue" << std::endl;
        return XB_E_FRAME_CORRUPTED;
    }

    if (response[1] != length) {
        logger(WARNING) << "Longueur de la trame incohérente" << std::endl;
        return XB_E_FRAME_LENGTH;
    }

    // Vérification des CRC16
    uint16_t headerChecksum = (response[7] << 8) | response[8];
    uint16_t dataChecksum = (response[length - 2] << 8) | response[length - 3];

    if (headerChecksum != computeChecksum(response, 0, XB_FRAME_HEADER_LENGTH)) {
        logger(WARNING) << "Checksum de l'en-tête invalide" << std::endl;
        return XB_E_FRAME_CRC_HEADER;
    }

    if (dataChecksum != computeChecksum(response, XB_FRAME_DATA_SHIFT, dataLength)) {
        logger(WARNING) << "Checksum des données invalide" << std::endl;
        return XB_E_FRAME_CRC_DATA;
    }

    return processFrame(response);
}

int XBee::processFrame(const std::vector<uint8_t> &buffer) {
    // Vérification de l'adresse de destination
    if (buffer[3] != address)
        return XB_E_FRAME_ADDR;

    xbee_frame_t frame = {
            .receiverAddress = buffer[3],
            .emitterAddress = buffer[4],
            .functionCode = buffer[5],
            .frameId = buffer[6],
            .data = std::vector<uint8_t>(buffer.begin() + XB_FRAME_DATA_SHIFT, buffer.end() - 3),
    };

    for (uint8_t byte: frame.data) {
        std::cout << std::hex << std::showbase << (int) byte << " ";
    }
    std::cout << std::endl;

    // callback->second contient la fonction à appeler
    auto callback = callbacks.find(frame.functionCode);

    if (callback != callbacks.end()) {
        callback->second(*this, frame);
        return XB_E_SUCCESS;
    }

    {
        // lock_gard vérouille le mutex et le dévérouille à la fin du bloc
        std::lock_guard<std::mutex> lock(responseMutex);
        responses[frame.frameId] = frame;
    }
    
    return XB_E_SUCCESS;
}


xbee_result_t XBee::send(uint8_t dest, uint8_t functionCode, const std::vector<uint8_t> &data, int timeout) {
    if (totalFrames >= XB_FRAME_MAX_ID)
        totalFrames = 0;

    if (data.size() > XB_FRAME_MAX_SIZE - XB_FRAME_MIN_LENGTH) {
        logger(ERROR) << "Trop de données à envoyer (max " << XB_FRAME_MAX_SIZE - XB_FRAME_MIN_LENGTH << " octets)" << std::endl;
        return { XB_E_FRAME_DATA_LENGTH };
    }

    uint8_t frameLen = XB_FRAME_MIN_LENGTH + data.size();
    std::vector<uint8_t> frame(frameLen);

    frame[0] = XB_FRAME_SOH;
    frame[1] = frameLen;
    frame[2] = ~frameLen;
    frame[3] = dest;
    frame[4] = address;
    frame[5] = functionCode;
    frame[6] = totalFrames++;

    uint16_t headerChecksum = computeChecksum(frame, 0, XB_FRAME_HEADER_LENGTH);
    frame[7] = (headerChecksum >> 8) & 0xFF;
    frame[8] = headerChecksum & 0xFF;

    for (int i = 0; i < data.size(); i++)
        frame[XB_FRAME_DATA_SHIFT + i] = data[i];

    uint16_t dataChecksum = computeChecksum(data, 0, data.size());
    frame[frameLen - 3] = dataChecksum & 0xFF;
    frame[frameLen - 2] = dataChecksum >> 8;
    frame[frameLen - 1] = XB_FRAME_EOT;

    serial.writeBytes(frame.data(), frameLen);
    logger(INFO) << "Trame envoyée avec succès : ";
    printBuffer(frame);

    if (timeout == 0)
        return { XB_E_SUCCESS };

    auto start = std::chrono::steady_clock::now();
    std::chrono::milliseconds timeoutMs(timeout*1000);

    // On attend la réponse pendant le timeout (de manière thread-safe)
    while ((std::chrono::steady_clock::now() - start) < timeoutMs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::lock_guard<std::mutex> lock(responseMutex);
        auto response = responses.find(totalFrames);

        if (response != responses.end()) {
            responses.erase(response);
            return { XB_E_SUCCESS, response->second };
        }
    }

    // Aucune réponse reçue à temps
    return { XB_E_FRAME_TIMEOUT };
}


uint16_t XBee::computeChecksum(const std::vector<uint8_t> &frame, uint8_t start, uint8_t length) {
    uint8_t checksum = 0x0000;

    for (uint8_t i = start; i < length; i++)
        checksum ^= frame[i];

    // On ne peut transmettre que des octets
    // 0x50 permet de faciliter l'échantillonnage du signal
    uint8_t checksumLSB = (checksum & 0x0F) | 0x50;
    uint8_t checksumMSB = (checksum & 0xF0) >> 4 | 0x50;

    return checksumMSB << 8 | checksumLSB;
}


template<typename T>
void XBee::readRx(T &buffer, unsigned int timeout) {
    buffer.clear();

    char reponse;
    std::chrono::milliseconds timeoutMs(timeout);
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start < timeoutMs) {
        while (serial.available() > 0) {
            serial.readChar(&reponse);
            buffer.push_back(reponse);
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}


// Destructeur appelé automatiquement pour fermer la connexion série
// et arrêter le thread d'écoute
XBee::~XBee() {
    if (serial.isDeviceOpen()) {
        serial.flushReceiver();
        logger(INFO) << "Buffer Rx nettoyé avec succès" << std::endl;

        serial.closeDevice();
        logger(INFO) << "Connexion série fermée avec succès" << std::endl;
    }

    if (isListening) {
        isListening = false;
        listenerThread->join();
    }
}
