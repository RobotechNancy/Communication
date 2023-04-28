/*!
 *  @file    xbee.h
 *  @version 1.0
 *  @date    2022-2023
 *  @author  Julien PISTRE
 *  @brief   Fichier d'en-tête de la classe XBee
 *  @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"
using namespace std;


// Configuration et initialisation

/*!
 *  @brief  Nettoyer le buffer et ouvrir la connexion UART
 *  @param  mode Configuration de port à utiliser
 *  @return 500 succès
 *  @return -501 Port série non trouvé
 *  @return -502 Erreur lors de l'ouverture du port série
 *  @return -503 Erreur lors de la récupération des informations du port série
 *  @return -504 Baudrate non reconnu
 *  @return -505 Erreur lors de l'écriture de la configuration du port série
 *  @return -506 Erreur lors de l'écriture du timeout
 *  @return -507 Databits non reconnus
 *  @return -508 Stopbits non reconnus
 *  @return -509 Parité non reconnue
 */
int XBee::openSerialConnection() {
    int status = serial.openDevice(
            XB_SERIAL_PORT_PRIMARY, XB_BAUDRATE_PRIMARY, XB_DATABITS_PRIMARY, XB_PARITY_PRIMARY, XB_STOPBITS_PRIMARY
    );

    if (status != XB_SER_E_SUCCESS) {
        logger << "(serial) /!\\ erreur " << status << " : impossible d'ouvrir le port "
               << XB_SERIAL_PORT_PRIMARY << " - baudrate : " << XB_BAUDRATE_PRIMARY << " - parités : "
               << XB_PARITY_PRIMARY << mendl;
        return status;
    }

    logger << "(serial) connexion ouverte avec succès sur le port " << XB_SERIAL_PORT_PRIMARY
            << " - baudrate : " << XB_BAUDRATE_PRIMARY << " - parité : " << XB_PARITY_PRIMARY << mendl;

    if ((status = checkATConfig()) < 0)
        return status;

    return XB_SER_E_SUCCESS;
}


/*!
    \brief Nettoyer le buffer et fermer la connexion UART
 */
void XBee::closeSerialConnection() {
    serial.flushReceiver();
    logger << "(serial) buffer Rx nettoyé avec succès" << mendl;

    serial.closeDevice();
    logger << "(serial) connexion série fermée avec succès" << mendl;
}


// Configuration en mode AT

/*!
 *  @brief  Vérifier et paramétrer la configuration un module XBee
 *  @return 400 Succès
 *  @return -401 Impossible d'entrer dans le mode AT
 *  @return -402 Impossible de configurer le mode API
 *  @return -403 Impossible de configurer le baudrate
 *  @return -404 Impossible de configurer le paramètre de chiffrement AES
 *  @return -405 Impossible de configurer la clé de chiffrement AES
 *  @return -406 Impossible de configurer le canal de découverte réseau
 *  @return -407 Impossible de configurer l'ID du réseau
 *  @return -408 Impossible de configurer le mode coordinateur
 *  @return -409 Impossible de configurer le nombre de bits de parité
 *  @return -410 Impossible de configurer l'adresse source 16bits
 *  @return -411 Impossible de configuer l'adresse de destination
 *  @return -412 Impossible de sortir du mode AT
 *  @return -413 Impossible d'écrire les paramètres dans la mémoire flash
 */
int XBee::checkATConfig() {
    if (enterATMode())
        logger << "(config AT) entrée dans le mode AT" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_ENTER << " : impossible d'entrer dans le mode AT" << mendl;
        closeSerialConnection();
        return XB_AT_E_ENTER;
    }

    if (sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE, XB_AT_M_GET))
        logger << "(config AT) baudrate vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE))
        logger << "(config AT) baudrate configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_BAUDRATE << " : impossible de configurer le baudrate" << mendl;
        return XB_AT_E_BAUDRATE;
    }

    if (sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY, XB_AT_M_GET))
        logger << "(config AT) nombre de bits de parité vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY))
        logger << "(config AT) nombre de bits de parité configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_PARITY
               << " : impossible de configurer la parité" << mendl;
        return XB_AT_E_PARITY;
    }

    if (sendATCommand(XB_AT_CMD_API, XB_AT_V_API, XB_AT_M_GET))
        logger << "(config AT) mode API vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_API, XB_AT_V_API))
        logger << "(config AT) mode API configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_API << " : impossible de configurer le mode API" << mendl;
        return XB_AT_E_API;
    }

    if (sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES, XB_AT_M_GET))
        logger << "(config AT) chiffrement AES vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES))
        logger << "(config AT) chiffrement AES configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_AES
               << " : impossible de configurer le paramètre de chiffrement AES" << mendl;
        return XB_AT_E_AES;
    }

    if (sendATCommand(XB_AT_CMD_AES_KEY, XB_AT_V_AES_KEY))
        logger << "(config AT) clé de chiffrement configurée avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_AES_KEY
               << " : impossible de configurer la clé de chiffrement AES" << mendl;
        return XB_AT_E_AES_KEY;
    }

    if (sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL, XB_AT_M_GET))
        logger << "(config AT) canal de découverte réseau vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL))
        logger << "(config AT) canal de découverte réseau configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_CHANEL
               << " : impossible de configurer le canal de découverte réseau" << mendl;
        return XB_AT_E_CHANEL;
    }

    if (sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID, XB_AT_M_GET))
        logger << "(config AT) ID du réseau vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID))
        logger << "(config AT) ID du réseau configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_PAN_ID << " : impossible de configurer l'ID du réseau"
               << mendl;
        return XB_AT_E_PAN_ID;
    }

    if (sendATCommand(XB_AT_CMD_COORDINATOR, XB_AT_V_COORDINATOR, XB_AT_M_GET))
        logger << "(config AT) mode coordinateur vérifié avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_COORDINATOR, XB_AT_V_COORDINATOR))
        logger << "(config AT) mode coordinateur configuré avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_COORDINATOR
               << " : impossible de configurer le mode coordinateur" << mendl;
        return XB_AT_E_COORDINATOR;
    }

    if (sendATCommand(XB_AT_CMD_16BIT_SOURCE_ADDR, XB_AT_V_16BIT_SOURCE_ADDR, XB_AT_M_GET))
        logger << "(config AT) adresse source 16bits vérifiée avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_16BIT_SOURCE_ADDR, XB_AT_V_16BIT_SOURCE_ADDR))
        logger << "(config AT) adresse source 16bits configurée avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_16BIT_SOURCE_ADDR
               << " : impossible de configurer l'adresse source 16bits" << mendl;
        return XB_AT_E_16BIT_SOURCE_ADDR;
    }

    if (sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR, XB_AT_M_GET))
        logger << "(config AT) adresse de destination vérifiée avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR))
        logger << "(config AT) adresse de destination configurée avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_LOW_DEST_ADDR
               << " : impossible de configurer l'adresse de destination" << mendl;
        return XB_AT_E_LOW_DEST_ADDR;
    }

    if (sendATCommand(XB_AT_CMD_HIGH_DEST_ADDR, XB_AT_V_HIGH_DEST_ADDR, XB_AT_M_GET))
        logger << "(config AT) adresse de destination vérifiée avec succès" << mendl;
    else if (sendATCommand(XB_AT_CMD_HIGH_DEST_ADDR, XB_AT_V_HIGH_DEST_ADDR))
        logger << "(config AT) adresse de destination configurée avec succès" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_LOW_DEST_ADDR
               << " : impossible de configurer l'adresse de destination" << mendl;
        return XB_AT_E_LOW_DEST_ADDR;
    }

    if (writeATConfig())
        logger << "(config AT) configuration AT enregistrée dans la mémoire du module" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_WRITE_CONFIG
               << " : impossible d'écrire les paramètres dans la mémoire flash" << mendl;
        return XB_AT_E_WRITE_CONFIG;
    }

    if (!exitATMode()) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_EXIT << " : impossible de sortir du mode AT" << mendl;
        return XB_AT_E_EXIT;
    }

    logger << "(config AT) configuration AT réalisée avec succès" << mendl;
    return XB_AT_E_SUCCESS;
}


/*!
 *  @brief  Envoyer une commmande AT en UART via le port série
 *  @param  command Le paramètre AT à envoyer au module
 *  @param  value La valeur de réponse attendue
 *  @param  mode Le mode de transmission (mode lecture ou écriture)
 *  @return true La réponse du module XBee est celle attendue
 *  @return false La réponse du module XBee n'est pas celle attendue
 */
bool XBee::sendATCommand(const char *command, const char *value, unsigned int mode) {
    serial.writeString(command);

    if (mode == XB_AT_M_GET) {
        serial.writeString(XB_AT_V_END_LINE);
        logger << "(config AT) envoi de la commande AT : " << command << mendl;
        return readATResponse(value);
    }

    serial.writeString(value);
    logger << "(config AT) envoi de la commande AT : " << command << "=" << value << mendl;
    return readATResponse(XB_AT_R_SUCCESS);
}


/*!
 *  @brief  Lire la réponse à un envoi de commande AT
 *  @param  value La valeur attendue pour la commande envoyée
 *  @param  mode Le mode de lecture à utiliser
 *  @return true La réponse du module XBee est celle attendue
 *  @return false la réponse du module XBee n'est pas celle attendue
 */
bool XBee::readATResponse(const char *value, int mode) {
    string response;

    if (strcmp(value, XB_AT_V_END_LINE) == 0) {
        delay(3);
        readRx<string>(response);
        serial.flushReceiver();

        logger << "(config AT) réponse du XBee : " << mendl;
        logger << response << mendl;

        return !response.empty() && response != XB_AT_V_END_LINE;
    }

    readRx<string>(response);
    serial.flushReceiver();

    logger << "(config AT) Réponse du XBee : " << response << mendl;
    return mode == 0 && response == value;
}

/*!
 *  @brief  Entrer dans le mode AT
 *  @return true Le module XBee est entré dans le mode AT
 *  @return false Le module XBee n'est pas entré dans le mode AT
 */
bool XBee::enterATMode() {
    serial.writeString(XB_AT_CMD_ENTER);
    logger << "(config AT) entrée en mode AT en cours..." << mendl;

    delay(2);
    serial.writeString(XB_AT_V_END_LINE);
    return readATResponse(XB_AT_R_SUCCESS);
}

/*!
 *  @brief  Sortir du mode AT
 *  @return true Le module XBee est sorti du mode AT
 *  @return false Le module XBee n'est pas sorti du mode AT
 */
bool XBee::exitATMode() {
    serial.writeString(XB_AT_CMD_EXIT);
    serial.writeString(XB_AT_V_END_LINE);
    logger << "(config AT) sortie du mode AT" << mendl;
    return readATResponse(XB_AT_R_SUCCESS);
}


/*!
 *  @brief  Ecrire les paramètres AT définis dans la mémoire flash du module XBee
 *  @return true La configuration a été écrite
 *  @return false La configuration n'a pas été écrite
 */
bool XBee::writeATConfig() {
    serial.writeString(XB_AT_CMD_WRITE_CONFIG);
    serial.writeString(XB_AT_V_END_LINE);
    logger << "(config AT) écriture des paramètres AT dans la mémoire" << mendl;
    return readATResponse(XB_AT_R_SUCCESS);
}


// Envoi/Réception/Traitement des trames de messages


/*!
 * @brief Lier un code fonction à une fonction
 * @param fct_code Le code fonction à écouter
 * @param callback La fonction à exécuter
 */
void XBee::subscribe(uint32_t fct_code, const message_callback& callback) {
    listeners.insert(std::make_pair(fct_code, callback));
}


/*!
 *  @brief Attendre, vérifier et traiter une trame reçue
 */
[[noreturn]] void XBee::listen() {
    vector<int> response;

    while (true) {
        response.clear();
        this_thread::sleep_for(chrono::milliseconds(10));

        if (serial.available() > 0) {
            readRx<vector<int>>(response);
            processResponse(response);
        }
    }
}


int XBee::processResponse(const vector<int> &response) {
    vector<int> buffer{};
    int status = -1;
    logger << "(process trame) trame reçue" << mendl;

    for (int i : response) {
        buffer.push_back(i);

        if (i == XB_V_END) {
            status = processSubFrame(buffer);
            buffer.clear();
        }
    }

    if (status == -1) {
        logger << "/!\\ (process trame) erreur " << XB_SUBTRAME_E_NONE << " : Aucune sous-trame valide " << mendl;
        return XB_TRAME_E_START;
    }

    return status;
}


/*!
 *  @brief  Découpe le résultat de la lecture du buffer en différentes trames
 *  @param  recv_msg Le résultat de la lecture du buffer
 *  @return 300 Succès
 *  @return -203 La trame n'est pas adressé au module
 *  @return -204 La vérification du CRC a échoué
 *  @return -205 La longueur des données est incorrecte
 *  @return -206 La séquence de début est incorrecte
 *  @return -207 La longueur de données est incorrecte
 */
int XBee::processSubFrame(vector<int> &recv_msg) {
    int data_len = recv_msg[5] - XB_V_SEQ_SHIFT;
    printFrame(recv_msg, data_len);

    if (recv_msg[0] != XB_V_START) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_START << " : Séquence de début incorrect " << mendl;
        return XB_TRAME_E_START;
    }

    if (data_len > 255) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_DATALEN << " : Longueur de données incorrecte " << mendl;
        return XB_TRAME_E_DATALEN;
    }

    if (recv_msg[9 + data_len] != XB_V_END) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_END << " : Séquence de fin incorrecte " << mendl;
        return XB_TRAME_E_END;
    }

    int msg_slice[6 + data_len];
    for (int i = 0; i < 6 + data_len; i++)
        msg_slice[i] = recv_msg[i];

    if (!validateCRC(recv_msg[7 + data_len], recv_msg[8 + data_len], msg_slice, data_len + 6)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_CRC << " : CRC incorrect "
               << mendl;
        return XB_TRAME_E_CRC;
    }

    recv_msg[3] -= XB_V_SEQ_SHIFT;
    recv_msg[4] -= XB_V_SEQ_SHIFT;
    recv_msg[5] -= XB_V_SEQ_SHIFT;
    logger << "(découpe trame) découpage des trames effectué avec succès" << mendl;

    processFrame(recv_msg);
    return XB_SUBTRAME_E_SUCCESS;
}


/*!
 *  @brief  Traiter une trame reçue par le XBee
 *  @param  recv_frame La trame reçue par le XBee
 *  @return 200 Succès
 *  @return -203 La trame n'est pas adressé au module
 */
int XBee::processFrame(vector<int> recv_frame) {
    if (XB_ADR_CURRENT != recv_frame[2])
        return XB_TRAME_E_WRONG_ADR;

    frame_t frame = {
            .start_seq = recv_frame[0],
            .adr_emetteur = recv_frame[1],
            .adr_dest = recv_frame[2],
            .id_trame_low = recv_frame[3],
            .id_trame_high = recv_frame[4],
            .data_len = recv_frame[5],
            .code_fct = recv_frame[6],
            .data = vector<int>{},
            .crc_low = recv_frame[7 + recv_frame[5]],
            .crc_high = recv_frame[8 + recv_frame[5]],
            .end_seq = recv_frame[9 + recv_frame[5]]
    };

    for (int i = 0; i < frame.data_len; i++)
        frame.data.push_back(recv_frame[7 + i]);
    
    if (listeners.contains(frame.code_fct))
        listeners[frame.code_fct](frame);
    else
        logger << "(processFrame) Code fonction non traité : " << frame.code_fct << mendl;

    return XB_TRAME_E_SUCCESS;
}


/*!
 *  @brief  Envoyer une trame structurée via UART au XBee
 *  @param  dest L'adresse du destinataire du message
 *  @param  fct_code Le code de la fonction concernée par le message
 *  @param  data Les valeurs des paramètres demandées par le code fonction
 *  @return 200 Succès
 *  @return -205 La taille des données est trop grande
 */
int XBee::sendFrame(uint8_t dest, uint8_t fct_code, const vector<int>& data, int data_len) {
    if (data_len > 255) {
        logger << "/!\\ (send frame) erreur " << XB_TRAME_E_DATALEN << " : taille des données trop grande" << mendl;
        return XB_TRAME_E_DATALEN;
    }

    int frame_id = ++nb_trame;
    uint8_t frame_len = data_len + 10;

    uint8_t frame[frame_len];
    int frame_int[frame_len];

    uint8_t frame_id_low = frame_id & 0xFF;
    uint8_t frame_id_high = (frame_id >> 8) & 0xFF;

    frame[0] = XB_V_START;
    frame[1] = XB_ADR_CURRENT;
    frame[2] = dest;
    frame[3] = frame_id_low + XB_V_SEQ_SHIFT;
    frame[4] = frame_id_high + XB_V_SEQ_SHIFT;
    frame[5] = data_len + XB_V_SEQ_SHIFT;
    frame[6] = fct_code;

    for (size_t i = 0; i < data_len; i++)
        frame[i + 7] = data[i];

    for (int i = 0; i < frame_len; i++)
        frame_int[i] = int(frame[i]);

    int crc = computeCRC(frame_int, data_len + 6);
    uint8_t crc_low = crc & 0xFF;
    uint8_t crc_high = (crc >> 8) & 0xFF;

    frame[data_len + 7] = crc_low;
    frame[data_len + 8] = crc_high;
    frame[data_len + 9] = XB_V_END;

    printFrame<uint8_t*>(frame, data_len);
    serial.writeBytes(frame, frame_len);
    logger << "(sendFrame) envoi de la frame n°" << dec << frame_id_low + frame_id_high
                                                 << " effectué avec succès" << mendl;

    return XB_TRAME_E_SUCCESS;
}


/*!
 *  @brief Afficher les données découpées d'une structure de type frame_t
 */
template<typename T>
void XBee::printFrame(const T &frame, int data_len) {
    cout << hex << showbase;
    cout << "\t-> Start seq : " << (int) frame[0] << endl;
    cout << "\t-> Emetteur : " << (int) frame[1] << endl;
    cout << "\t-> Destinataire : " << (int) frame[2] << endl;
    cout << "\t-> Id trame  : " << (int) frame[3] << " " << (int) frame[4] << endl;
    cout << "\t-> Taille msg : " << (int) frame[5] << endl;
    cout << "\t-> Code fct : " << (int) frame[6] << endl;
    cout << "\t-> Data : ";

    for (int i = 0; i < data_len; i++)
        cout << (int) frame[7+i] << " ";
    cout << endl;

    cout << "\t-> CRC : " << (int) frame[data_len+7] << " " << (int) frame[data_len+8] << endl;
    cout << "\t-> End seq : " << (int) frame[data_len+9] << endl;
}


/*!
 *  @brief  Calculer le CRC16 Modbus de la frame XBee envoyée
 *  @param  frame La trame XBee sans le CRC et le caractère de fin de trame
 *  @param  frame_len La taille de la trame
 *  @return La valeur entière du CRC calculée sur 16 bits
 */
int XBee::computeCRC(const int frame[], uint8_t frame_len) {
    int crc = 0xFFFF, count = 0;
    int cur_byte = frame[0];
    const int POLYNOME = 0xA001;

    do {
        crc ^= cur_byte;

        for (uint8_t i = 0; i < 8; i++)
            if ((crc % 2) != 0)
                crc = (crc >> 1) ^ POLYNOME;
            else
                crc = (crc >> 1);

        count++;
        cur_byte = frame[count];
    } while (count < frame_len);

    return crc;
}


/*!
 *  @brief  Vérifier si le CRC reçu est cohérent avec la trame reçue
 *  @param  crc_low Les bits de poids faible du CRC reçu
 *  @param  crc_high Les bits de poids forts du CRC reçu
 *  @param  frame La trame reçue (en enlevant le CRC et le caratère de fin de trame)
 *  @param  frame_len La taille de le trame telle qu'entrée dans la fonction
 *  @return true La valeur du CRC reçue est bien celle calculée à partir du reste de la trame
 *  @return false La valeur du CRC est incohérente ou non calculable
 */
bool XBee::validateCRC(uint8_t crc_low, uint8_t crc_high, int frame[], int frame_len) {
    int crc = computeCRC(frame, frame_len);

    uint8_t new_crc_low = crc & 0xFF;
    uint8_t new_crc_high = (crc >> 8) & 0xFF;

    return (new_crc_low == crc_low) && (new_crc_high == crc_high);
}


/*!
 * @brief  Lire le buffer Rx de la liaison série
 * @tparam T Le type du buffer dans lequel on veut stocker les données lues
 * @param  buffer Le buffer dans lequel on veut stocker les données lues
 */
template<typename T>
void XBee::readRx(T &buffer) {
    char *reponse;
    reponse = new char;

    unsigned int timeout = 100;
    delay(1);

    while (serial.available() > 0) {
        serial.readChar(reponse, timeout);
        buffer.push_back(*reponse);
    }

    delete reponse;
}


/*!
 *  @brief Retarder l'exécution du code
 *  @param seconds Délai en secondes
 */
void XBee::delay(float seconds) {
    this_thread::sleep_for(chrono::milliseconds((int) seconds * 1000));
}
