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
    int errorOpening = serial.openDevice(XB_SERIAL_PORT_PRIMARY, XB_BAUDRATE_PRIMARY, XB_DATABITS_PRIMARY,
                                     XB_PARITY_PRIMARY, XB_STOPBITS_PRIMARY);

    if (errorOpening != XB_SER_E_SUCCESS)
        logger << "(serial) /!\\ erreur " << errorOpening << " : impossible d'ouvrir le port "
                << XB_SERIAL_PORT_PRIMARY << " - baudrate : " << XB_BAUDRATE_PRIMARY << " - parités : "
                << XB_PARITY_PRIMARY << mendl;
    else {
        logger << "(serial) connexion ouverte avec succès sur le port " << XB_SERIAL_PORT_PRIMARY
                << " - baudrate : " << XB_BAUDRATE_PRIMARY << " - parité : " << XB_PARITY_PRIMARY << mendl;

        return checkATConfig();
    }

    return errorOpening;
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


/*!
 *  @brief <br>Retarder l'exécution du code
 *  @param time Délai en secondes
 */
void XBee::delay(unsigned int time) {
    this_thread::sleep_for(chrono::milliseconds(time * 1000));
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
    if (mode == XB_AT_M_GET) {
        serial.writeString(command);
        serial.writeString(XB_AT_V_END_LINE);
        logger << "(config AT) envoi de la commande AT : " << command << mendl;
        return readATResponse(value);
    } else {
        serial.writeString(command);
        serial.writeString(value);
        logger << "(config AT) envoi de la commande AT : " << command << "=" << value << mendl;
        return readATResponse(XB_AT_R_SUCCESS);
    }
}


/*!
 *  @brief  <br>Lire la réponse à un envoi de commande AT
 *  @param  value La valeur attendue pour la commande envoyée
 *  @param  mode Le mode de lecture à utiliser
 *  @return <b>true</b> La réponse du module XBee est celle attendue
 *  @return <b>false</b> la réponse du module XBee n'est pas celle attendue
 */
bool XBee::readATResponse(const char *value, int mode) {
    string reponse;

    if (strcmp(value, XB_AT_V_END_LINE) == 0) {
        delay(3);
        reponse = readString();
        serial.flushReceiver();
        logger << "(config AT) réponse du Xbee : " << mendl;
        logger << reponse << mendl;

        return !reponse.empty() && reponse != XB_AT_V_END_LINE;
    }

    reponse = readString();
    serial.flushReceiver();

    logger << "(config AT) Réponse du Xbee : " << reponse << mendl;
    return mode == 0 && reponse == value;
}

/*!
 *  @brief  <br>Entrer dans le mode AT
 *  @return <b>true</b> Le module XBee est entré dans le mode AT
 *  @return <b>false</b> Le module XBee n'est pas entré dans le mode AT
 */
bool XBee::enterATMode() {
    serial.writeString(XB_AT_CMD_ENTER);
    logger << "(config AT) entrée en mode AT en cours..." << mendl;

    delay(2);
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

    delay(2);
    string reponse = readString();
    serial.flushReceiver();

    return reponse[0] == XB_AT_V_DISCOVER_NETWORK[0];
}


// Envoi/Réception/Traitement des trames de messages


/*!
 *  @brief <br>Attendre, vérifier et traiter une trame reçue
 */
[[noreturn]] void XBee::waitForATrame() {
    vector<int> rep;

    if (XB_ADR_CURRENT == XB_ADR_ROBOT_01) {
        char msg[1] = {XB_V_ACK};
        sendTrame(XB_ADR_CAMERA, XB_FCT_TEST_ALIVE, msg, 1);
    }

    while (true) {
        rep.clear();
        this_thread::sleep_for(chrono::milliseconds(10));

        if (serial.available() > 0) {
            rep = readBuffer();
            subTrame(rep);
        }
    }
}


/*!
 *  @brief  <br>Découpe le résultat de la lecture du buffer en différentes trames
 *  \param  msg_recu Le résultat de la lecture du buffer
 *  \return <b>300</b> Succès
 *  \return <b>-204</b> La vérification du CRC a échoué
 */
int XBee:: subTrame(vector<int> msg_recu) {
    int data_len = msg_recu[5];
    int crc_low = msg_recu[7 + data_len];
    int crc_high = msg_recu[8 + data_len];

    int msg_decoupe[6 + data_len];
    for (int i = 0; i < 6 + data_len; i++)
        msg_decoupe[i] = msg_recu[i];

    if (!isCRCCorrect(crc_low, crc_high, msg_decoupe, data_len + 6)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_CRC << " : CRC incorrect "
               << mendl;
        return XB_TRAME_E_CRC;
    }

    processFrame(msg_recu);
    logger << "(découpe trame) découpage des trames effectué avec succès" << mendl;
    return XB_SUB_TRAME_E_SUCCESS;
}


/*!
 *  @brief  <br>Traiter une trame reçue par le XBee
 *  @param  trame_recue La trame reçue par le XBee
 *  @return <b>200</b> Succès
 */
int XBee::processFrame(vector<int> trame_recue) {
    frame_t trame = {
            .start_seq = trame_recue[0],
            .adr_emetteur = trame_recue[1],
            .adr_dest = trame_recue[2],
            .id_trame_low = trame_recue[3],
            .id_trame_high = trame_recue[4],
            .nb_octets_msg = trame_recue[5],
            .code_fct = trame_recue[6],
            .param = vector<int>{},
            .crc_low = trame_recue[7 + trame_recue[5]],
            .crc_high = trame_recue[8 + trame_recue[5]],
            .end_seq = trame_recue[9 + trame_recue[5]]
    };

    for (int i = 0; i < trame.nb_octets_msg; i++)
        trame.param.push_back(trame_recue[7 + i]);

    afficherTrameRecue(trame);
    processCodeFct(trame.code_fct, trame.adr_emetteur, trame.crc_low, trame.param);

    logger << "(process trame) trame n°" << trame.id_trame_high + trame.id_trame_low << "a été traitée avec succès "
           << mendl;

    return XB_TRAME_E_SUCCESS;
}

/*!
 *  @brief  <br>Interprète le code fonction issu d'une trame reçue
 *  @param  code_fct Le code fonction issu d'une trame reçue
 *  @param  exp L'adresse de l'expéditeur de la trame
 *  @return <b>100</b> Succès
 *  @return <b>-102</b> Code fonction existant mais non implémenté
 */
int XBee::processCodeFct(int code_fct, int exp, int id_trame, vector<int> param) {
    char msg[1] = {XB_V_ACK};

    switch (code_fct) {
        case XB_FCT_TEST_ALIVE:
            sendTrame(exp, XB_FCT_TEST_ALIVE, msg, 1);
            break;
        case XB_FCT_ARUCO_POS:
            for (int i = 0; i < param.size()/4; i++) {
                aruco_tags[i] = {
                        param[4*i],
                        (double) param[4*i+1],
                        (double) param[4*i+2],
                        (double) param[4*i+3]
                };
            }
            break;
        default :
            logger << "/!\\ (process code fonction) erreur " << XB_FCT_E_NONE_REACHABLE
                   << " : code fonction existant mais ne déclenchant aucune action  " << mendl;
            return XB_FCT_E_NONE_REACHABLE;
    }

    if (queue.find(id_trame) != queue.end()) {
        queue.at(id_trame) = true;
        logger << "(process code fonction) trame n°" << id_trame << " a reçu une réponse" << mendl;
    }

    logger << "(process code fonction) code fonction n°" << code_fct << " traité avec succès" << mendl;

    return XB_FCT_E_SUCCESS;
}


/*!
 *  @brief  <br>Envoyer une trame structurée via UART au XBee
 *  @param  ad_dest L'adresse du destinataire du message
 *  @param  code_fct Le code de la fonction concernée par le message
 *  @param  data Les valeurs des paramètres demandées par le code fonction
 *  @return <b>XB_TRAME_E_SUCCESS</b> Succès
 */
int XBee::sendTrame(uint8_t ad_dest, uint8_t code_fct, const char *data, int data_len) {
    uint8_t length_trame = data_len + 10;
    uint8_t trame[length_trame];
    int trame_int[length_trame];
    int id_trame = ++ID_TRAME;
    uint8_t id_trame_low = id_trame & 0xFF;
    uint8_t id_trame_high = (id_trame >> 8) & 0xFF;

    trame[0] = XB_V_START;
    trame[1] = XB_ADR_CURRENT;
    trame[2] = ad_dest;
    trame[3] = id_trame_low;
    trame[4] = id_trame_high;
    trame[5] = data_len;
    trame[6] = code_fct;

    for (size_t i = 0; i < data_len; i++)
        trame[i + 7] = data[i];

    for (int i = 0; i < length_trame; i++)
        trame_int[i] = int(trame[i]);

    int crc = crc16(trame_int, data_len + 6);
    uint8_t crc_low = crc & 0xFF;
    uint8_t crc_high = (crc >> 8) & 0xFF;

    trame[data_len + 7] = crc_low;
    trame[data_len + 8] = crc_high;
    trame[data_len + 9] = XB_V_END;

    serial.writeBytes(trame, length_trame);
    logger << "(sendTrame) envoi de la trame n°" << dec << id_trame_low + id_trame_high << " effectué avec succès"
            << mendl;

    queue.insert(make_pair(crc_low, false));
    return XB_TRAME_E_SUCCESS;
}


/*!
 *  @brief  <br>Calculer le CRC16 Modbus de la trame XBee envoyée
 *  @param  trame La trame XBee sans le CRC et le caractère de fin de trame
 *  @param  taille La taille de la trame
 *  @return La valeur entière du CRC calculée sur 16 bits
 */
int XBee::crc16(const int trame[], uint8_t taille) {
    int crc = 0xFFFF, count = 0;
    int octet_a_traiter;
    const int POLYNOME = 0xA001;

    octet_a_traiter = trame[0];

    do {
        crc ^= octet_a_traiter;

        for (uint8_t i = 0; i < 8; i++) {

            if ((crc % 2) != 0)
                crc = (crc >> 1) ^ POLYNOME;

            else
                crc = (crc >> 1);

        }

        count++;
        octet_a_traiter = trame[count];
    } while (count < taille);

    return crc;
}


/*!
 *  @brief  <br>Vérifier si le CRC reçu est cohérent avec la trame reçue
 *  @param  crc_low Les bits de poids faible du CRC reçu
 *  @param  crc_high Les bits de poids forts du CRC reçu
 *  @param  trame La trame reçue (en enlevant le CRC et le caratère de fin de trame)
 *  @param  trame_size La taille de le trame telle qu'entrée dans la fonction
 *  @return <b>true</b> La valeur du CRC reçue est bien celle calculée à partir du reste de la trame
 *  @return <b>false</b> La valeur du CRC est incohérente ou non calculable
 */
bool XBee::isCRCCorrect(uint8_t crc_low, uint8_t crc_high, int trame[], int trame_size) {
    int crc = crc16(trame, trame_size);

    uint8_t newcrc_low = crc & 0xFF;
    uint8_t newcrc_high = (crc >> 8) & 0xFF;

    return (newcrc_low == crc_low) && (newcrc_high == crc_high);
}


/*!
 *  @brief  <br>Lire l'intégralité du buffer Rx de la RaspberryPi
 *  @return <b>rep</b> La valeur du buffer sous forme d'un vecteur d'entiers signés sur 32 bits
 */
vector<int> XBee::readBuffer() {
    char *reponse(nullptr);
    reponse = new char;

    vector<int> rep;
    unsigned int timeout = 100;
    delay(1);

    while (serial.available() > 0) {
        serial.readChar(reponse, timeout);
        rep.push_back(*reponse);
    }

    delete reponse;
    reponse = nullptr;

    return rep;
}


/*!
 *   @brief  <br>Renvoyer la valeur du buffer Rx de la RaspberryPi sous forme d'objet string
 *   @return <b>rep</b> La valeur du buffer concaténée sous forme d'objet string
 */
string XBee::readString() {
    char *reponse(nullptr);
    reponse = new char;

    string rep;
    unsigned int timeout = 100;
    delay(1);

    while (serial.available() > 0) {
        serial.readChar(reponse, timeout);
        rep += *reponse;
    }

    delete reponse;
    reponse = nullptr;

    return rep;
}


/*!
 *  @brief <br>Afficher les données découpées d'une structure de type frame_t
 */
void XBee::afficherTrameRecue(const frame_t &trame) {
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
