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
int XBee::openSerialConnection(int mode) {
    int errorOpening = 0;

    if (mode == 1) {
        errorOpening = serial.openDevice(XB_SERIAL_PORT_DEFAULT, XB_BAUDRATE_DEFAULT, XB_DATABITS_DEFAULT,
                                         XB_PARITY_DEFAULT, XB_STOPBITS_DEFAULT);

        if (errorOpening != XB_SER_E_SUCCESS)
            logger << "(serial) /!\\ erreur " << errorOpening << " : impossible d'ouvrir le port "
                    << XB_SERIAL_PORT_DEFAULT << " - baudrate : " << XB_BAUDRATE_DEFAULT << " - parité : "
                    << XB_PARITY_DEFAULT << mendl;
        else {
            logger << "(serial) connexion ouverte avec succès sur le port " << XB_SERIAL_PORT_DEFAULT
                    << " - baudrate : " << XB_BAUDRATE_DEFAULT << " - parité : " << XB_PARITY_DEFAULT << mendl;
            if (MODE != 2) checkATConfig();
        }
    } else if (mode == 0) {
        errorOpening = serial.openDevice(XB_SERIAL_PORT_PRIMARY, XB_BAUDRATE_PRIMARY, XB_DATABITS_PRIMARY,
                                         XB_PARITY_PRIMARY, XB_STOPBITS_PRIMARY);

        if (errorOpening != XB_SER_E_SUCCESS)
            logger << "(serial) /!\\ erreur " << errorOpening << " : impossible d'ouvrir le port "
                    << XB_SERIAL_PORT_PRIMARY << " - baudrate : " << XB_BAUDRATE_PRIMARY << " - parités : "
                    << XB_PARITY_PRIMARY << mendl;
        else {
            logger << "(serial) connexion ouverte avec succès sur le port " << XB_SERIAL_PORT_PRIMARY
                    << " - baudrate : " << XB_BAUDRATE_PRIMARY << " - parité : " << XB_PARITY_PRIMARY << mendl;
            if (MODE != 2) checkATConfig();
        }
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
    if (!enterATMode()) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_ENTER << " : impossible d'entrer dans le mode AT" << mendl;
        closeSerialConnection();
        if (MODE == 0) {
            MODE = 1;
            openSerialConnection(1);
        } else {
            MODE = 0;
            openSerialConnection();
        }
        return XB_AT_E_ENTER;
    } else logger << "(config AT) entrée dans le mode AT" << mendl;

    if (!sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_BAUDRATE, XB_AT_V_BAUDRATE)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_BAUDRATE << " : impossible de configurer le baudrate"
                   << mendl;
            return XB_AT_E_BAUDRATE;
        }
        logger << "(config AT) baudrate configuré avec succès" << mendl;
    } else logger << "(config AT) baudrate vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_PARITY, XB_AT_V_PARITY)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_PARITY
                    << " : impossible de configurer le nombre de bits de parité" << mendl;
            return XB_AT_E_PARITY;
        }
        logger << "(config AT) nombre de bits de parité configuré avec succès" << mendl;

        if (!writeATConfig()) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_WRITE_CONFIG
                    << " : impossible d'écrire les paramètres dans la mémoire flash" << mendl;
            return XB_AT_E_WRITE_CONFIG;
        }

        closeSerialConnection();
        if (MODE == 0) {
            MODE = 1;
            openSerialConnection(1);
        } else {
            MODE = 0;
            openSerialConnection();
        }
    } else logger << "(config AT) nombre de bits de parité vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_API, XB_AT_V_API, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_API, XB_AT_V_API)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_API << " : impossible de configurer le mode API" << mendl;
            return XB_AT_E_API;
        }
        logger << "(config AT) mode API configuré avec succès" << mendl;
    } else logger << "(config AT) mode API vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_AES, XB_AT_V_AES)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_AES
                   << " : impossible de configurer le paramètre de chiffrement AES" << mendl;
            return XB_AT_E_AES;
        }
        logger << "(config AT) chiffrement AES configuré avec succès" << mendl;
    } else logger << "(config AT) chiffrement AES vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_AES_KEY, XB_AT_V_AES_KEY)) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_AES_KEY
               << " : impossible de configurer la clé de chiffrement AES" << mendl;
        return XB_AT_E_AES_KEY;
    } else logger << "(config AT) clé de chiffrement configurée avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_CHANEL, XB_AT_V_CHANEL)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_CHANEL
                   << " : impossible de configurer le canal de découverte réseau" << mendl;
            return XB_AT_E_CHANEL;
        }
        logger << "(config AT) canal de découverte réseau configuré avec succès" << mendl;
    } else logger << "(config AT) canal de découverte réseau vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_PAN_ID, XB_AT_V_PAN_ID)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_PAN_ID << " : impossible de configurer l'ID du réseau"
                   << mendl;
            return XB_AT_E_PAN_ID;
        }
        logger << "(config AT) ID du réseau configuré avec succès" << mendl;
    } else logger << "(config AT) ID du réseau vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_COORDINATOR, XB_AT_V_COORDINATOR, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_COORDINATOR, XB_AT_V_COORDINATOR)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_COORDINATOR
                   << " : impossible de configurer le mode coordinateur" << mendl;
            return XB_AT_E_COORDINATOR;
        }
        logger << "(config AT) mode coordinateur configuré avec succès" << mendl;
    } else logger << "(config AT) mode coordinateur vérifié avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_16BIT_SOURCE_ADDR, XB_AT_V_16BIT_SOURCE_ADDR, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_16BIT_SOURCE_ADDR, XB_AT_V_16BIT_SOURCE_ADDR)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_16BIT_SOURCE_ADDR
                   << " : impossible de configurer l'adresse source 16bits" << mendl;
            return XB_AT_E_16BIT_SOURCE_ADDR;
        }
        logger << "(config AT) adresse source 16bits configurée avec succès" << mendl;
    } else logger << "(config AT) adresse source 16bits vérifiée avec succès" << mendl;

    if (!sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR, XB_AT_M_GET)) {
        if (!sendATCommand(XB_AT_CMD_LOW_DEST_ADDR, XB_AT_V_LOW_DEST_ADDR)) {
            logger << "/!\\ (config AT) erreur " << XB_AT_E_LOW_DEST_ADDR
                   << " : impossible de configurer l'adresse de destination" << mendl;
            return XB_AT_E_LOW_DEST_ADDR;
        }
        logger << "(config AT) adresse de destination configurée avec succès" << mendl;
    } else logger << "(config AT) adresse de destination vérifiée avec succès" << mendl;

    if (!writeATConfig()) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_WRITE_CONFIG
               << " : impossible d'écrire les paramètres dans la mémoire flash" << mendl;
        return XB_AT_E_WRITE_CONFIG;
    } else logger << "(config AT) configuration AT enregistrée dans la mémoire du module" << mendl;

    if (!discoverXbeeNetwork()) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_DISCOVER_NETWORK
               << " : impossible d'établir une connexion avec le module XBee distant" << mendl;
        return XB_AT_E_DISCOVER_NETWORK;
    } else logger << "(config AT) connexion XBee établie avec succès avec le module distant" << mendl;

    if (!exitATMode()) {
        logger << "/!\\ (config AT) erreur " << XB_AT_E_EXIT << " : impossible de sortir du mode AT" << mendl;
        return XB_AT_E_EXIT;
    }

    logger << "(config AT) configuration AT réalisée avec succès" << mendl;
    MODE = 2;
    return XB_AT_E_SUCCESS;
}

/*!
 *  @brief <br>Retarder l'exécution du code
 *  @param time Délai en secondes
 */
void XBee::delay(unsigned int time) {
    this_thread::sleep_for(chrono::milliseconds(time * 1000));
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

    if (value == XB_AT_V_DISCOVER_NETWORK) {
        delay(3);
        reponse = readString();
        serial.flushReceiver();
        logger << "(config AT) réponse du Xbee : " << mendl;
        logger << reponse << mendl;

        if (!reponse.empty() && reponse != XB_AT_V_END_LINE) return true;
        return false;
    }

    reponse = readString();

    if (!reponse.empty() && reponse != XB_AT_V_END_LINE) {
        logger << "(config AT) réponse du Xbee : " << reponse << mendl;
    }

    if (mode == 0)
        if (reponse == value) return true;

    return false;
}

/*!
 *  @brief  <br>Entrer dans le mode AT
 *  @return <b>true</b> Le module XBee est entré dans le mode AT
 *  @return <b>false</b> Le module XBee n'est pas entré dans le mode AT
 */
bool XBee::enterATMode() {
    serial.writeString(XB_AT_CMD_ENTER);
    logger << "(config AT) entrée en mode AT en cours..." << mendl;

    delay(3);
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
 *  @brief  <br>Recherche du module XBee distant de l'autre robot
 *  @return <b>true</b> Le bon module XBee est détecté
 *  @return <b>false</b> Aucun module XBee détecté ou module XBee incorrect détecté
 */
bool XBee::discoverXbeeNetwork() {
    serial.writeString(XB_AT_CMD_DISCOVER_NETWORK);
    serial.writeString(XB_AT_V_END_LINE);
    logger << "(config AT) lancement de la découverte réseau XBee" << mendl;
    return readATResponse(XB_AT_V_DISCOVER_NETWORK, 1);
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


// Envoi/Réception/Traitement des trames de messages

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
 *  @brief  <br>Envoyer une trame structurée via UART au XBee
 *  @param  ad_dest L'adresse du destinataire du message
 *  @param  code_fct Le code de la fonction concernée par le message
 *  @param  data Les valeurs des paramètres demandées par le code fonction
 *  @return <b>XB_TRAME_E_SUCCESS</b> Succès
 */
int XBee::sendTrame(uint8_t ad_dest, uint8_t code_fct, char *data) {
    uint8_t length_trame = strlen(data) + 10;
    uint8_t trame[length_trame];
    int trame_int[length_trame];
    int id_trame = ++ID_TRAME;
    uint8_t id_trame_low = id_trame & 0xFF;
    uint8_t id_trame_high = (id_trame >> 8) & 0xFF;

    trame[0] = XB_V_START;
    trame[1] = XB_ADR_CURRENT_ROBOT;
    trame[2] = ad_dest;
    trame[3] = id_trame_low + 4;
    trame[4] = id_trame_high + 4;
    trame[5] = strlen(data) + 4;
    trame[6] = code_fct;

    for (size_t i = 0; i < strlen(data); i++)
        trame[i + 7] = data[i];

    for (int i = 0; i < length_trame; i++)
        trame_int[i] = int(trame[i]);

    int crc = crc16(trame_int, strlen(data) + 6);
    uint8_t crc_low = crc & 0xFF;
    uint8_t crc_high = (crc >> 8) & 0xFF;

    trame[strlen(data) + 7] = crc_low;
    trame[strlen(data) + 8] = crc_high;
    trame[strlen(data) + 9] = XB_V_END;

    serial.writeBytes(trame, length_trame);
    logger << "(sendTrame) envoi de la trame n°" << dec << id_trame_low + id_trame_high << " effectué avec succès"
            << mendl;

    trames_envoyees[code_fct] = trames_envoyees[code_fct] + 1;

    return XB_TRAME_E_SUCCESS;
}

/*!
 *  @brief  <br>Traiter une trame reçue par le XBee
 *  @param  trame_recue La trame reçue par le XBee
 *  @return <b>200</b> Succès
 *  @return <b>-201</b> Taille de la trame incorrecte ou non concordante
 *  @return <b>-202</b> Premier caractère de la trame incorrect
 *  @return <b>-203</b> Pernier caractère de la trame incorrect
 *  @return <b>-204</b> Valeur du CRC incorrecte
 *  @return <b>-205</b> Adresse de l'expéditeur incorrecte ou inconnue
 *  @return <b>-206</b> Adresse du destinataire incorrecte ou inconnue
 */
int XBee::processFrame(vector<int> trame_recue) {

    if (!isTrameSizeCorrect(trame_recue)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_SIZE
                << " : taille de la trame incorrecte ou non concordante " << mendl;
        return XB_TRAME_E_SIZE;
    }

    frame_t trame = {
            .start_seq = trame_recue[0],
            .adr_emetteur = trame_recue[1],
            .adr_dest = trame_recue[2],
            .id_trame_low = trame_recue[3] - 4,
            .id_trame_high = trame_recue[4] - 4,
            .nb_octets_msg = trame_recue[5] - 4,
            .code_fct = trame_recue[6],
            .crc_low = trame_recue[3 + trame_recue[4]],
            .crc_high = trame_recue[4 + trame_recue[4]],
            .end_seq = trame_recue[5 + trame_recue[4]]
    };

    vector<int> data{};

    for (uint8_t i = 0; i < trame.nb_octets_msg; i++) {
        data.push_back(trame_recue[7 + i]);
    }

    trame.param = data;
    afficherTrameRecue(trame);
    int decoupe_trame[trame_recue[4] + 6];

    for (uint8_t i = 0; i < trame_recue[4] + 3; i++) {
        decoupe_trame[i] = trame_recue[i];
    }

    if (!isStartSeqCorrect(trame.start_seq)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_START << " : premier caractère de la trame incorrect "
                << mendl;
        return XB_TRAME_E_START;
    }

    if (!isEndSeqCorrect(trame.end_seq)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_END << " : dernier caractère de la trame incorrect "
                << mendl;
        return XB_TRAME_E_END;
    }

    if (!isCRCCorrect(trame.crc_low, trame.crc_high, decoupe_trame, trame_recue[4] + 2)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_CRC << " : valeur du CRC incorrecte " << mendl;
        return XB_TRAME_E_CRC;
    }

    if (!isExpCorrect(trame.adr_emetteur)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_EXP
                << " : adresse de l'expéditeur incorrecte ou inconnue " << mendl;
        return XB_TRAME_E_EXP;
    }

    if (!isDestCorrect(trame.adr_dest)) {
        logger << "/!\\ (process trame) erreur " << XB_TRAME_E_DEST
                << " : adresse du destinataire incorrecte ou inconnue " << mendl;
        return XB_TRAME_E_DEST;
    }

    processCodeFct(trame.code_fct, trame.adr_emetteur, trame.param);

    logger << "(process trame) trame n°" << trame.id_trame_high + trame.id_trame_low << "a été traitée avec succès "
            << mendl;

    return XB_TRAME_E_SUCCESS;
}

/*!
 *  @brief  <br>Interprète le code fonction issu d'une trame reçue
 *  @param  code_fct Le code fonction issu d'une trame reçue
 *  @param  exp L'adresse de l'expéditeur de la trame
 *  @return <b>100</b> Succès
 *  @return <b>-101</b> Code fonction incorrect
 *  @return <b>-102</b> Code fonction existant mais non implémenté
 */
int XBee::processCodeFct(int code_fct, int exp, vector<int> param) {
    if (!isCodeFctCorrect(code_fct)) {
        logger << "/!\\ (process code fonction) erreur " << XB_FCT_E_NOT_FOUND << " : code fonction incorrect "
                << mendl;
        return XB_FCT_E_NOT_FOUND;
    }

    char msg[1];

    switch (code_fct) {
    case XB_FCT_TEST_ALIVE:
        msg[0] = {XB_V_ACK};
        sendTrame(exp, XB_FCT_TEST_ALIVE, msg);
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

    trames_envoyees[code_fct] = trames_envoyees[code_fct] - 1;
    logger << "(process code fonction) code fonction n°" << code_fct << " traité avec succès" << mendl;

    return XB_FCT_E_SUCCESS;
}

/*!
 *  @brief  <br>Vérifier un code fonction est défini
 *  @param  code_fct Le code fonction à vérifier
 *  @return <b>true</b> Le code fonction est correct
 *  @return <b>false</b> Le code fonction est incorrect/n'existe pas
 */
bool XBee::isCodeFctCorrect(int code_fct) {
    int i = 0;
    int size_list_code_fct = sizeof(XB_LIST_CODE_FCT) / sizeof (XB_LIST_CODE_FCT[0]);

    do {
        if (XB_LIST_CODE_FCT[i] == code_fct)
            return true;
    } while (i++ < size_list_code_fct);

    return false;
}

/*!
 *  @brief  <br>Vérifier si la taille d'une trame est correcte
 *  @param  trame La trame à vérifier
 *  @return <b>true</b> La taille de la trame semble cohérente
 *  @return <b>false</b> La taille de la trame est incorrecte, trop petite ou non cohérente
 */
bool XBee::isTrameSizeCorrect(vector<int> trame) {
    return (trame.size() > 10) && (trame.size() == trame[4]+5);
}

/*!
 *  @brief  <br>Vérifier si une adresse d'expéditeur est correcte
 *  @param  exp L'adresse de l'expéditeur à vérifier
 *  @return <b>true</b> L'adresse est correcte
 *  @return <b>false</b> L'adresse est incorrecte
 */
bool XBee::isExpCorrect(int exp) {
    int i = 0;
    int size_list_addr = sizeof(XB_LIST_ADR) / sizeof (XB_LIST_ADR[0]);

    do {
        if (XB_LIST_ADR[i] == XB_ADR_BROADCAST)
            return true;
    } while (i++ < size_list_addr);

    return false;
}

/*!
 *  @brief  <br>Vérifier si une adresse de destination est correcte
 *  @param  exp L'adresse de destination à vérifier
 *  @return <b>true</b> L'adresse est correcte
 *  @return <b>false</b> L'adresse est incorrecte
 */
bool XBee::isDestCorrect(int dest) {
    int i = 0;
    int size_list_addr = sizeof(XB_LIST_ADR) / sizeof (XB_LIST_ADR[0]);

    do {
        if (XB_LIST_ADR[i] == dest)
            return true;
    } while (i++ < size_list_addr);

    return false;
}

/*!
 *  @brief  <br>Vérifier si le caractère de début de la trame correpond à celui attendu
 *  @param  value Le caractère à vérifier
 *  @return <b>true</b> Le caratère est bien celui attendu
 *  @return <b>false</b> Le caractère est incorrect
 */
bool XBee::isStartSeqCorrect(int value) {
    return value == XB_V_START;
}

/*!
 *  @brief  <br>Vérifier si le caractère de fin de la trame correpond à celui attendu
 *  @param  value Le caractère à vérifier
 *  @return <b>true</b> Le caratère est bien celui attendu
 *  @return <b>false</b> Le caractère est incorrect
 */
bool XBee::isEndSeqCorrect(int value) {
    return value == XB_V_END;
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
 *  @brief <br>Attendre, vérifier et traiter une trame reçue
 */
[[noreturn]] void XBee::waitForATrame() {
    vector<int> rep;

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
 *  @brief  <br>Découpre le résultat de la lecture du buffer en différentes trames
 *  \param  msg_recu Le résultat de la lecture du buffer
 *  \return <b>300</b> Succès
 *  \return <b>-301</b> La position des trames dans le message reçu est incorrecte : les caractères de début et de fin de trame ne sont pas au même nombre
 *  \return <b>-302</b> La position des trames dans le message reçu est incorrecte : certains caractères de début de trame sont placés après des caractères de fin de trame
 *  \return <b>-303</b> La position des trames dans le message reçu est incorrecte : des caractères inconnus sont placés entre deux trames
 *  \return <b>-304</b> Le premier caractère lu dans le buffer n'est pas celui d'un début de trame
 *  \return <b>-305</b> Le dernier caractère lu dans le buffer n'est pas celui d'une fin de trame
 *  \return <b>-306</b> Aucun caractère de début et/ou de fin n'est présent dans le message reçu
 */
int XBee::subTrame(vector<int> msg_recu) {
    vector<int> list_start_seq{};
    vector<int> list_end_seq{};
    vector<int> decoupe{};
    int decoupe_retour;

    for (uint8_t i = 0; i < msg_recu.size(); i++) {
        if (msg_recu[i] == XB_V_START)
            list_start_seq.push_back(i);

        if (msg_recu[i] == XB_V_END)
            list_end_seq.push_back(i);
    }

    if (list_start_seq.empty() || list_end_seq.empty()) {
        logger << "/!\\ (découpe trame) erreur " << XB_SUB_TRAME_E_NULL
                << " : aucun caractère de début et/ou de fin n'est présent dans le message reçu " << mendl;
        return XB_SUB_TRAME_E_NULL;
    }

    if (list_start_seq.size() != list_end_seq.size()) {
        logger << "/!\\ (découpe trame) erreur " << XB_SUB_TRAME_E_SIZE
                << " : les caractères de début et de fin de trame ne sont pas au même nombre " << mendl;
        return XB_SUB_TRAME_E_SIZE;
    }

    for (uint8_t i = 0; i < list_start_seq.size(); i++) {
        if (list_start_seq[i] > list_end_seq[i]) {
            logger << "/!\\ (découpe trame) erreur " << XB_SUB_TRAME_E_REPARTITION
                    << " : certains caractères de début de trame sont placés après des caractères de fin de trame "
                    << mendl;
            return XB_SUB_TRAME_E_REPARTITION;
        }

        if (i != 0) {
            if (list_start_seq[i] != list_end_seq[i - 1] - 1) {
                logger << "/!\\ (découpe trame) erreur " << XB_SUB_TRAME_E_DECOUPAGE
                        << " : des caractères inconnus sont placés entre deux trames " << mendl;
                return XB_SUB_TRAME_E_DECOUPAGE;
            }
        }
    }


    if (list_start_seq[0] != 0) {
        logger << "/!\\ (découpe trame) erreur " << XB_SUB_TRAME_E_START
                << " : le premier caractère lu dans le buffer n'est pas celui d'un début de trame " << mendl;
        return XB_SUB_TRAME_E_START;
    }

    if (list_end_seq[list_end_seq.size() - 1] != msg_recu.size() - 1) {
        logger << "/!\\ (découpe trame) erreur " << XB_SUB_TRAME_E_END
                << " : le dernier caractère lu dans le buffer n'est pas celui d'une fin de trame " << mendl;
        return XB_SUB_TRAME_E_END;
    }

    for (uint8_t i = 0; i < list_start_seq.size(); i++) {
        decoupe.clear();
        decoupe = slice(msg_recu, list_start_seq[i], list_end_seq[i]);
        decoupe_retour = processFrame(decoupe);
    }

    logger << "(découpe trame) découpage des trames effectué avec succès" << mendl;
    return XB_SUB_TRAME_E_SUCCESS;
}

/*!
 *  @brief <br>Envoyer des battements de coeur au second robot pour savoir s'il est toujours opérationnel
 */
[[noreturn]] void XBee::sendHeartbeat() {
    char *msg = new char[1];
    msg[0] = XB_V_ACK;

    while (true) {
        delay(3);
        sendTrame(XB_ADR_ROBOT_02, XB_FCT_TEST_ALIVE, msg);
    }
}

/*!
 *  @brief <br>Vérifier si un message envoyé a reçu une réponse
 */
[[noreturn]] int XBee::isXbeeResponding() {
    int size_list_code_fct = sizeof(XB_LIST_CODE_FCT) / sizeof (XB_LIST_CODE_FCT[0]);

    while (true) {
        delay(3);
        for (int i = 0; i < size_list_code_fct; i++) {
            if (trames_envoyees[XB_LIST_CODE_FCT[i]] == 0){
                logger << "(verif reponse) les trames envoyées portant le code fonction " << XB_LIST_CODE_FCT[i] <<
                " ont toutes reçues une réponse" << mendl;
            } else {
                logger << "(verif reponse) /!\\ les trames envoyées portant le code fonction "
                        << XB_LIST_CODE_FCT[i] << " n'ont pas toutes reçues une réponse" << mendl;
            }
        }
    }
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
    cout << "\t-> Taille msg : " << trame.nb_octets_msg - 3 << endl;
    cout << "\t-> Code fct : " << trame.code_fct << endl;
    cout << "\t-> Data : ";
    print(trame.param);
    cout << "\t-> CRC : " << trame.crc_low << " " << trame.crc_high << endl;
    cout << "\t-> End seq : " << trame.end_seq << endl;
}

/*!
 *  @brief <br>Afficher les valeurs contenues dans un vecteur d'entiers
 *  @param v Le vecteur dont on souhaite afficher le contenu
 */
void XBee::print(const vector<int> &v) {
    copy(v.begin(), v.end(),
         ostream_iterator<int>(cout << hex, " "));
    cout << endl;
}

/*!
 *  \brief  <br>Extraire un sous-vecteur d'entiers d'un vecteur d'entiers
 *  \param  v Le vecteur à découper
 *  \param  a l'indice de la première valeur
 *  \param  b L'indice de la dernière valeur
 *  \return <b>vec</b> Le sous-vecteur d'entiers découpé
 */
vector<int> XBee::slice(const vector<int> &v, int a, int b) {
    auto first = v.cbegin() + a;
    auto last = v.cbegin() + b + 1;

    vector<int> vec(first, last);
    return vec;
}
