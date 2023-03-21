/*!
 *  @file    xbee.h
 *  @version 1.0
 *  @date    2022-2023
 *  @author  Julien PISTRE
 *  @brief   Fichier d'en-tête de la classe XBee
 *  @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "../include/xbee.h"
using namespace std;


// Constructeurs et destructeurs

XBee::XBee(): logger("xbee") {}
XBee::~XBee() = default;


// Configuration et initialisation

/*!
 *  @brief  <br>Nettoyer le buffer et ouvrir la connexion UART
 *  @param  mode Configuration de port à utiliser
 *  @return <b>500</b> succès
 *  @return <b>-501</b> Port série non trouvé
 *  @return <b>-502</b> Erreur lors de l'ouverture du port série
 *  @return <b>-503</b> Erreur lors de la récupération des informations du port série
 *  @return <b>-504</b> Baudrate non reconnu
 *  @return <b>-505</b> Erreur lors de l'écriture de la configuration du port série
 *  @return <b>-506</b> Erreur lors de l'écriture du timeout
 *  @return <b>-507</b> Databits non reconnus
 *  @return <b>-508</b> Stopbits non reconnus
 *  @return <b>-509</b> Parité non reconnue
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
    \brief <br>Nettoyer le buffer et fermer la connexion UART
 */
void XBee::closeSerialConnection() {
    serial.flushReceiver();
    logger << "(serial) buffer Rx nettoyé avec succès" << mendl;

    serial.closeDevice();
    logger << "(serial) connexion série fermée avec succès" << mendl;
}


// Configuration en mode AT

/*!
 *  @brief  <br>Vérifier et paramétrer la configuration un module XBee
 *  @return <b>400</b> Succès
 *  @return <b>-401</b> Impossible d'entrer dans le mode AT
 *  @return <b>-402</b> Impossible de configurer le mode API
 *  @return <b>-403</b> Impossible de configurer le baudrate
 *  @return <b>-404</b> Impossible de configurer le paramètre de chiffrement AES
 *  @return <b>-405</b> Impossible de configurer la clé de chiffrement AES
 *  @return <b>-406</b> Impossible de configurer le canal de découverte réseau
 *  @return <b>-407</b> Impossible de configurer l'ID du réseau
 *  @return <b>-408</b> Impossible de configurer le mode coordinateur
 *  @return <b>-409</b> Impossible de configurer le nombre de bits de parité
 *  @return <b>-410</b> Impossible de configurer l'adresse source 16bits
 *  @return <b>-411</b> Impossible de configuer l'adresse de destination
 *  @return <b>-412</b> Impossible de sortir du mode AT
 *  @return <b>-413</b> Impossible d'écrire les paramètres dans la mémoire flash
 *  @return <b>-414</b> Impossible d'établir une connexion avec le module XBee distant
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
               << " : impossible d'écrire les paramètres dans la mémoire flash" << mendl;
        return XB_AT_E_WRITE_CONFIG;
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

    if (discoverXbeeNetwork())
        logger << "(config AT) connexion XBee établie avec succès avec le module distant" << mendl;
    else {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_DISCOVER_NETWORK
               << " : impossible d'établir une connexion avec le module XBee distant" << mendl;
        return XB_AT_E_DISCOVER_NETWORK;
    }

    if (!exitATMode()) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_EXIT << " : impossible de sortir du mode AT" << mendl;
        return XB_AT_E_EXIT;
    }

    logger << "(config AT) configuration AT réalisée avec succès" << mendl;
    return XB_AT_E_SUCCESS;
}


/*!
 *  @brief  <br>Envoyer une commmande AT en UART via le port série
 *  @param  command Le paramètre AT à envoyer au module
 *  @param  value La valeur de réponse attendue
 *  @param  mode Le mode de transmission (mode lecture ou écriture)
 *  @return <b>true</b> La réponse du module XBee est celle attendue
 *  @return <b>false</b> La réponse du module XBee n'est pas celle attendue
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
 *  @brief  <br>Lire la réponse à un envoi de commande AT
 *  @param  value La valeur attendue pour la commande envoyée
 *  @param  mode Le mode de lecture à utiliser
 *  @return <b>true</b> La réponse du module XBee est celle attendue
 *  @return <b>false</b> la réponse du module XBee n'est pas celle attendue
 */
bool XBee::readATResponse(const char *value, int mode) {
    string response;

    if (strcmp(value, XB_AT_V_END_LINE) == 0) {
        delay(3);
        readRx<string>(response);
        serial.flushReceiver();

        logger << "(config AT) réponse du Xbee : " << mendl;
        logger << response << mendl;

        return !response.empty() && response != XB_AT_V_END_LINE;
    }

    readRx<string>(response);
    serial.flushReceiver();

    logger << "(config AT) Réponse du Xbee : " << response << mendl;
    return mode == 0 && response == value;
}

/*!
 *  @brief  <br>Entrer dans le mode AT
 *  @return <b>true</b> Le module XBee est entré dans le mode AT
 *  @return <b>false</b> Le module XBee n'est pas entré dans le mode AT
 */
bool XBee::enterATMode() {
    serial.writeString(XB_AT_CMD_ENTER);
    logger << "(config AT) entrée en mode AT en cours..." << mendl;

    delay(1.5);
    serial.writeString(XB_AT_V_END_LINE);
    return readATResponse(XB_AT_R_SUCCESS);
}

/*!
 *  @brief  <br>Sortir du mode AT
 *  @return <b>true</b> Le module XBee est sorti du mode AT
 *  @return <b>false</b> Le module XBee n'est pas sorti du mode AT
 */
bool XBee::exitATMode() {
    serial.writeString(XB_AT_CMD_EXIT);
    serial.writeString(XB_AT_V_END_LINE);
    logger << "(config AT) sortie du mode AT" << mendl;
    return readATResponse(XB_AT_R_SUCCESS);
}


/*!
 *  @brief  <br>Ecrire les paramètres AT définis dans la mémoire flash du module XBee
 *  @return <b>true</b> La configuration a été écrite
 *  @return <b>false</b> La configuration n'a pas été écrite
 */
bool XBee::writeATConfig() {
    serial.writeString(XB_AT_CMD_WRITE_CONFIG);
    serial.writeString(XB_AT_V_END_LINE);
    logger << "(config AT) écriture des paramètres AT dans la mémoire" << mendl;
    return readATResponse(XB_AT_R_SUCCESS);
}


/*!
 *  @brief  <br>Recherche du module XBee distant de l'autre robot
 *  @return <b>true</b> Le bon module XBee est détecté
 *  @return <b>false</b> Aucun module XBee détecté ou module XBee incorrect détecté
 */
bool XBee::discoverXbeeNetwork() {
    serial.writeString(XB_AT_CMD_DISCOVER_NETWORK);
    serial.writeString(XB_AT_V_END_LINE);
    logger << "(config AT) lancement de la découverte réseau XBee" << mendl;

    delay(1.5);
    string response;
    readRx<string>(response);

    serial.flushReceiver();
    return response[0] == XB_AT_V_DISCOVER_NETWORK[0];
}


// Envoi/Réception/Traitement des trames de messages

/*!
 *  @brief <br>Attendre, vérifier et traiter une trame reçue
 */
[[noreturn]] void XBee::waitForATrame() {
    vector<int> response;

    if (XB_ADR_CURRENT == XB_ADR_ROBOT_01) {
        char msg[1] = {XB_V_ACK};
        sendFrame(XB_ADR_CAMERA, XB_FCT_TEST_ALIVE, msg, 1);
    }

    while (true) {
        response.clear();
        this_thread::sleep_for(chrono::milliseconds(10));

        if (serial.available() > 0) {
            readRx<vector<int>>(response);
            subTrame(response);
        }
    }
}


/*!
 *  @brief  <br>Découpe le résultat de la lecture du buffer en différentes trames
 *  @param  recv_msg Le résultat de la lecture du buffer
 *  @return <b>300</b> Succès
 *  @return <b>-204</b> La vérification du CRC a échoué
 *  @return <b>-205</b> La longueur des données est incorrecte
 *  @return <b>-206</b> La séquence de début est incorrecte
 *  @return <b>-207</b> La longueur de données est incorrecte
 */
int XBee::subTrame(vector<int> recv_msg) {
    if (recv_msg[0] != XB_V_START) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_START << " : Séquence de début incorrect " << mendl;
        return XB_TRAME_E_START;
    }

    // TODO : plusieurs trames dans le buffer
    recv_msg[5] -= XB_V_SEQ_SHIFT;
    recv_msg[6] -= XB_V_SEQ_SHIFT;
    recv_msg[7] -= XB_V_SEQ_SHIFT;

    int data_len = recv_msg[5];

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

    if (!isCRCCorrect(recv_msg[7 + data_len], recv_msg[8 + data_len], msg_slice, data_len + 6)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_CRC << " : CRC incorrect "
               << mendl;
        return XB_TRAME_E_CRC;
    }

    logger << "(découpe trame) découpage des trames effectué avec succès" << mendl;

    processFrame(recv_msg);
    return XB_SUBTRAME_E_SUCCESS;
}


/*!
 *  @brief  <br>Traiter une trame reçue par le XBee
 *  @param  recv_frame La trame reçue par le XBee
 *  @return <b>200</b> Succès
 */
int XBee::processFrame(vector<int> recv_frame) {
    frame_t frame = {
            .start_seq = recv_frame[0],
            .adr_emetteur = recv_frame[1],
            .adr_dest = recv_frame[2],
            .id_trame_low = recv_frame[3],
            .id_trame_high = recv_frame[4],
            .nb_octets_msg = recv_frame[5],
            .code_fct = recv_frame[6],
            .param = vector<int>{},
            .crc_low = recv_frame[7 + recv_frame[5]],
            .crc_high = recv_frame[8 + recv_frame[5]],
            .end_seq = recv_frame[9 + recv_frame[5]]
    };

    for (int i = 0; i < frame.nb_octets_msg; i++)
        frame.param.push_back(recv_frame[7 + i]);

    printFrame(frame);
    processFctCode(frame.code_fct, frame.adr_emetteur, frame.param);

    logger << "(process frame) frame n°" << frame.id_trame_high + frame.id_trame_low
                                         << "a été traitée avec succès " << mendl;

    return XB_TRAME_E_SUCCESS;
}


/*!
 *  @brief  <br>Interprète le code fonction issu d'une trame reçue
 *  @param  fct_code Le code fonction issu d'une trame reçue
 *  @param  exp L'adresse de l'expéditeur de la trame
 *  @return <b>100</b> Succès
 *  @return <b>-102</b> Code fonction inconnu
 *  @return <b>-103</b> Adresse inconnue
 */
int XBee::processFctCode(int fct_code, int exp, vector<int> data) {
    char msg[1] = {XB_V_ACK};

    switch (exp) {
        case XB_ADR_CAMERA:
        case XB_ADR_ROBOT_01:
        case XB_ADR_ROBOT_02:
        break;
        default:
            logger << "/!\\ (process code fonction) erreur " << XB_ADR_E_UNKNOWN << " : adresse inconnue" << mendl;
            return XB_ADR_E_UNKNOWN;
    }

    switch (fct_code) {
        case XB_FCT_TEST_ALIVE:
            sendFrame(exp, XB_FCT_TEST_ALIVE, msg, 1);
        break;
        case XB_FCT_ARUCO_POS:
            for (int i = 0; i < data.size() / 4; i++) {
                aruco_tags[i] = {
                        data[4 * i],
                        (double) data[4 * i + 1],
                        (double) data[4 * i + 2],
                        (double) data[4 * i + 3]
                };
            }
        break;
        default :
            logger << "/!\\ (process code fonction) erreur " << XB_FCT_E_UNKNOWN << " : code fonction inconnu" << mendl;
            return XB_FCT_E_UNKNOWN;
    }

    logger << "(process code fonction) code fonction n°" << fct_code << " traité avec succès" << mendl;
    return XB_FCT_E_SUCCESS;
}


/*!
 *  @brief  <br>Envoyer une trame structurée via UART au XBee
 *  @param  dest L'adresse du destinataire du message
 *  @param  fct_code Le code de la fonction concernée par le message
 *  @param  data Les valeurs des paramètres demandées par le code fonction
 *  @return <b>XB_TRAME_E_SUCCESS</b> Succès
 *  @return <b>XB_TRAME_E_DATALEN</b> La taille des données est trop grande
 */
int XBee::sendFrame(uint8_t dest, uint8_t fct_code, const char *data, int data_len) {
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

    serial.writeBytes(frame, frame_len);
    logger << "(sendFrame) envoi de la frame n°" << dec << frame_id_low + frame_id_high
                                                 << " effectué avec succès" << mendl;

    return XB_TRAME_E_SUCCESS;
}


/*!
 *  @brief <br>Afficher les données découpées d'une structure de type frame_t
 */
void XBee::printFrame(const frame_t &trame) {
    cout << hex << showbase;
    cout << "\t-> Start seq : " << trame.start_seq << endl;
    cout << "\t-> Emetteur : " << trame.adr_emetteur << endl;
    cout << "\t-> Destinataire : " << trame.adr_dest << endl;
    cout << "\t-> Id trame  : " << trame.id_trame_low << " " << trame.crc_high << endl;
    cout << "\t-> Taille msg : " << trame.nb_octets_msg << endl;
    cout << "\t-> Code fct : " << trame.code_fct << endl;
    cout << "\t-> Data : ";

    copy(trame.param.begin(), trame.param.end(), ostream_iterator<int>(cout << hex, " "));
    cout << endl;

    cout << "\t-> CRC : " << trame.crc_low << " " << trame.crc_high << endl;
    cout << "\t-> End seq : " << trame.end_seq << endl;
}


/*!
 *  @brief  <br>Calculer le CRC16 Modbus de la frame XBee envoyée
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
 *  @brief  <br>Vérifier si le CRC reçu est cohérent avec la trame reçue
 *  @param  crc_low Les bits de poids faible du CRC reçu
 *  @param  crc_high Les bits de poids forts du CRC reçu
 *  @param  frame La trame reçue (en enlevant le CRC et le caratère de fin de trame)
 *  @param  frame_len La taille de le trame telle qu'entrée dans la fonction
 *  @return <b>true</b> La valeur du CRC reçue est bien celle calculée à partir du reste de la trame
 *  @return <b>false</b> La valeur du CRC est incohérente ou non calculable
 */
bool XBee::isCRCCorrect(uint8_t crc_low, uint8_t crc_high, int frame[], int frame_len) {
    int crc = computeCRC(frame, frame_len);

    uint8_t newcrc_low = crc & 0xFF;
    uint8_t newcrc_high = (crc >> 8) & 0xFF;

    return (newcrc_low == crc_low) && (newcrc_high == crc_high);
}


/*!
 * @brief  <br>Lire le buffer Rx de la liaison série
 * @tparam T Le type du buffer dans lequel on veut stocker les données lues
 * @param  buffer Le buffer dans lequel on veut stocker les données lues
 */
template<typename T>
void XBee::readRx(T &buffer) {
    char *reponse(nullptr);
    reponse = new char;

    unsigned int timeout = 100;
    delay(1);

    while (serial.available() > 0) {
        serial.readChar(reponse, timeout);
        buffer.push_back(*reponse);
    }

    delete reponse;
    reponse = nullptr;
}


/*!
 *  @brief <br>Retarder l'exécution du code
 *  @param seconds Délai en secondes
 */
void XBee::delay(float seconds) {
    this_thread::sleep_for(chrono::milliseconds((int) seconds * 1000));
}
