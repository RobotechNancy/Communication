/*!
 *  @file    xbee.h
 *  @version 1.0
 *  @date    2022-2023
 *  @author  Julien PISTRE
 *  @brief   Fichier d'en-tête de la classe XBee
 *  @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"


XBee::XBee(::uint8_t addr): logger("xbee"), address(addr) {}

int XBee::open(const char* port) {
    int status = serial.openDevice(
            port, XB_BAUDRATE_PRIMARY, XB_DATABITS_PRIMARY, XB_PARITY_PRIMARY, XB_STOPBITS_PRIMARY
    );

    if (status != XB_SER_E_SUCCESS) {
        logger(CRITICAL) << "Impossible d'ouvrir le port " << port
                                                           << " - baudrate : " << XB_BAUDRATE_PRIMARY
                                                           << " - parités : " << XB_PARITY_PRIMARY
                                                           << std::endl;
        return status;
    }

    logger(INFO) << "Connexion ouverte avec succès sur le port " << port
                                                                 << " - baudrate : " << XB_BAUDRATE_PRIMARY
                                                                 << " - parité : " << XB_PARITY_PRIMARY
                                                                 << std::endl;

    if ((status = checkATConfig()) < 0)
        return status;

    return XB_SER_E_SUCCESS;
}


int XBee::checkATConfig() {
    if (enterATMode())
        logger(INFO) << "Entrée dans le mode AT" << std::endl;
    else {
        logger(CRITICAL) << "Impossible d'entrer dans le mode AT" << std::endl;
        return XB_AT_E_ENTER;
    }

    if (sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE, XB_AT_M_GET))
        logger(INFO) << "Vaudrate vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE))
        logger(INFO) << "Baudrate configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le baudrate" << std::endl;
        return XB_AT_E_BAUDRATE;
    }

    if (sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY, XB_AT_M_GET))
        logger(INFO) << "Nombre de bits de parité vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY))
        logger(INFO) << "Nombre de bits de parité configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer la parité" << std::endl;
        return XB_AT_E_PARITY;
    }

    if (sendATCommand(XB_AT_CMD_API, XB_AT_V_API, XB_AT_M_GET))
        logger(INFO) << "Mode API vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_API, XB_AT_V_API))
        logger(INFO) << "Mode API configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le mode API" << std::endl;
        return XB_AT_E_API;
    }

    if (sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES, XB_AT_M_GET))
        logger(INFO) << "Chiffrement AES vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES))
        logger(INFO) << "Chiffrement AES configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le paramètre de chiffrement AES" << std::endl;
        return XB_AT_E_AES;
    }

    if (sendATCommand(XB_AT_CMD_AES_KEY, XB_AT_V_AES_KEY))
        logger(INFO) << "Clé de chiffrement configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer la clé de chiffrement AES" << std::endl;
        return XB_AT_E_AES_KEY;
    }

    if (sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL, XB_AT_M_GET))
        logger(INFO) << "Canal de découverte réseau vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL))
        logger(INFO) << "Canal de découverte réseau configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le canal de découverte réseau" << std::endl;
        return XB_AT_E_CHANEL;
    }

    if (sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID, XB_AT_M_GET))
        logger(INFO) << "ID du réseau vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID))
        logger(INFO) << "ID du réseau configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'ID du réseau" << std::endl;
        return XB_AT_E_PAN_ID;
    }

    const char* coordinator = (address == 1) ? XB_AT_V_COORDINATOR : XB_AT_V_END_DEVICE;

    if (sendATCommand(XB_AT_CMD_COORDINATOR, coordinator, XB_AT_M_GET))
        logger(INFO) << "Mode coordinateur vérifié avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_COORDINATOR, coordinator))
        logger(INFO) << "Mode coordinateur configuré avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer le mode coordinateur" << std::endl;
        return XB_AT_E_COORDINATOR;
    }

    char addr[3];
    sprintf(addr, "%d\r", address);

    if (sendATCommand(XB_AT_CMD_SOURCE_ADDR, addr, XB_AT_M_GET))
        logger(INFO) << "Adresse source 16bits vérifiée avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_SOURCE_ADDR, addr))
        logger(INFO) << "Adresse source 16bits configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'adresse source 16bits" << std::endl;
        return XB_AT_E_16BIT_SOURCE_ADDR;
    }

    if (sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR, XB_AT_M_GET))
        logger(INFO) << "Adresse de destination vérifiée avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR))
        logger(INFO) << "Adresse de destination configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'adresse de destination" << std::endl;
        return XB_AT_E_LOW_DEST_ADDR;
    }

    if (sendATCommand(XB_AT_CMD_HIGH_DEST_ADDR, XB_AT_V_HIGH_DEST_ADDR, XB_AT_M_GET))
        logger(INFO) << "Adresse de destination vérifiée avec succès" << std::endl;
    else if (sendATCommand(XB_AT_CMD_HIGH_DEST_ADDR, XB_AT_V_HIGH_DEST_ADDR))
        logger(INFO) << "Adresse de destination configurée avec succès" << std::endl;
    else {
        logger(CRITICAL) << "Impossible de configurer l'adresse de destination" << std::endl;
        return XB_AT_E_LOW_DEST_ADDR;
    }

    if (writeATConfig())
        logger(INFO) << "Configuration AT enregistrée dans la mémoire du module" << std::endl;
    else {
        logger(CRITICAL) << "Impossible d'écrire les paramètres dans la mémoire flash" << std::endl;
        return XB_AT_E_WRITE_CONFIG;
    }

    if (!exitATMode()) {
        logger(CRITICAL) << "Impossible de sortir du mode AT" << std::endl;
        return XB_AT_E_EXIT;
    }

    serial.flushReceiver();
    logger(INFO) << "Configuration AT réalisée avec succès" << std::endl;
    return XB_AT_E_SUCCESS;
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
    readRx<std::string>(response, timeout);

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


void XBee::bind(uint8_t functionCode, const xbee_callback_t& callback) {
    listeners.insert(std::make_pair(functionCode, callback));
}

void XBee::printFrame(const uint8_t *frame, uint8_t length) {
    logger(INFO) << std::showbase << std::hex
                 << "\n\t- StartDelimiter : " << (int) frame[0]
                 << "\n\t- Length : " << (int) frame[1]
                 << "\n\t- EmitAddress : " << (int) frame[2]
                 << "\n\t- ReceiverAddress : " << (int) frame[3]
                 << "\n\t- FrameId : " << (int) frame[4]
                 << "\n\t- FunctionCode : " << (int) frame[5]
                 << "\n\t- Data : ";

    for (int i = 0; i < length - XB_FRAME_DATA_SHIFT - 2; i++) {
        logger(INFO) << (int) frame[XB_FRAME_DATA_SHIFT + i] << " ";
    }

    logger(INFO) << "\n\t- Checksum : " << (int) (frame[length - 2] << 8 | frame[length - 1]) << std::endl;
}

void XBee::startListening() {
    isListening = true;
    listenerThread = std::make_unique<std::thread>(&XBee::listen, this);
    logger(INFO) << "Thread d'écoute démarré" << std::endl;
}

void XBee::listen() {
    std::vector<uint8_t> response;

    while (isListening) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (serial.available() > 0) {
            readRx<std::vector<uint8_t>>(response);
            processBuffer(response);
        }
    }
}

int XBee::processBuffer(std::vector<uint8_t> &response) {
    int status = XB_SUBTRAME_E_NONE;
    logger(INFO) << "Données reçues" << std::endl;

    while (true) {
        auto it = std::find(response.begin(), response.end(), XB_V_START);

        if (it == response.end() || response.size() < XB_FRAME_DEFAULT_LENGTH) {
            break;
        }

        auto it2 = std::find(it + 1, response.end(), XB_V_START);

        if (it2 - it < XB_FRAME_DEFAULT_LENGTH) {
            continue;
        }

        uint8_t buffer[it2 - it];
        std::copy(it, it2, buffer);

        response.erase(it, it2);
        status = processFrame(buffer);
    }

    return status;
}

int XBee::processFrame(const uint8_t *buffer) {
    uint8_t length = buffer[1];
    uint8_t dataLength = length - XB_FRAME_DATA_SHIFT - 2;

    if (length > XB_FRAME_MAX_SIZE) {
        logger(ERROR) << "La trame reçue dépasse la limite de " << XB_FRAME_MAX_SIZE << " octets" << std::endl;
        return XB_TRAME_E_DATALEN;
    }

    if (address != buffer[3]) {
        logger(WARNING) << "La trame reçue n'est pas destinée à ce module" << std::endl;
        return XB_TRAME_E_WRONG_ADR;
    }

    logger(INFO) << "Trame reçue :" << std::endl;
    printFrame(buffer, length);

    uint16_t checksum = buffer[length - 1] << 8 | buffer[length - 2];
    if (checksum != computeChecksum(buffer, length - 2)) {
        logger(ERROR) << "La trame reçue est corrompue" << std::endl;
        return XB_TRAME_E_CRC;
    }

    xbee_frame_t frame = {
            .startDelimiter = buffer[0],
            .length = buffer[1],
            .emitAddress = buffer[2],
            .receiverAddress = buffer[3],
            .frameId = buffer[4],
            .functionCode = buffer[5],
            .data = new uint8_t[dataLength],
            .checksum = checksum,
    };

    for (uint8_t i = 0; i < dataLength; i++) {
        frame.data[i] = buffer[XB_FRAME_DATA_SHIFT + i];
    }

    if (listeners.contains(frame.functionCode)) {
        listeners[frame.functionCode](frame);
    } else {
        responses[frame.frameId] = frame;
    }

    delete[] frame.data;
    return XB_SUBTRAME_E_SUCCESS;
}

int XBee::waitFor(xbee_frame_t &frame, uint8_t frameID, uint32_t timeout) {
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < timeout) {
        if (!responses.contains(frameID))
            continue;

        frame = responses[frameID];
        responses.erase(frameID);
        return XB_E_SUCCESS;
    }

    logger(ERROR) << "Timeout atteint lors de l'attente d'une réponse" << std::endl;
    return XB_SER_E_TIMOUT;
}


int XBee::send(uint8_t dest, uint8_t functionCode, const uint8_t *data, uint8_t dataLength) {
    if (dataLength > XB_FRAME_MAX_SIZE - XB_FRAME_DEFAULT_LENGTH) {
        logger(ERROR) << "Trop de données à envoyer (max " << XB_FRAME_MAX_SIZE - XB_FRAME_DEFAULT_LENGTH << " octets)" << std::endl;
        return XB_TRAME_E_DATALEN;
    }

    uint8_t frameLen = XB_FRAME_DEFAULT_LENGTH + dataLength;
    uint8_t frame[frameLen];

    frame[0] = XB_V_START;
    frame[1] = dataLength;
    frame[2] = address;
    frame[3] = dest;
    frame[4] = ++totalFrames;
    frame[5] = functionCode;

    for (int i = 0; i < dataLength; i++) {
        frame[XB_FRAME_DATA_SHIFT + i] = data[i];
    }

    uint16_t crc = computeChecksum(frame, frameLen - 2);
    frame[frameLen - 2] = crc & 0xFF;
    frame[frameLen - 1] = crc >> 8;

    serial.writeBytes(frame, frameLen);

    logger(INFO) << "Trame envoyée avec succès :";
    printFrame(frame, frameLen);

    return XB_TRAME_E_SUCCESS;
}


uint16_t XBee::computeChecksum(const uint8_t *frame, uint8_t length) {
    int checksum = 0xFFFF, count = 0;
    int curByte = frame[0];
    const int POLYNOME = 0xA001;

    do {
        checksum ^= curByte;

        for (uint8_t i = 0; i < 8; i++)
            if ((checksum % 2) != 0)
                checksum = (checksum >> 1) ^ POLYNOME;
            else
                checksum = (checksum >> 1);

        count++;
        curByte = frame[count];
    } while (count < length);

    return checksum;
}


template<typename T>
void XBee::readRx(T &buffer, unsigned int timeout) {
    buffer.clear();

    char reponse;
    auto start = std::chrono::steady_clock::now();

    while (true) {
        auto elapsed = std::chrono::steady_clock::now() - start;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() > timeout)
            break;

        if (serial.available() > 0) {
            while (serial.available() > 0) {
                serial.readChar(&reponse);
                buffer.push_back(reponse);
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }

            break;
        }
    }
}


XBee::~XBee() {
    serial.flushReceiver();
    logger(INFO) << "Buffer Rx nettoyé avec succès" << std::endl;

    serial.closeDevice();
    logger(INFO) << "Connexion série fermée avec succès" << std::endl;

    if (isListening) {
        isListening = false;
        listenerThread->join();
    }
}
