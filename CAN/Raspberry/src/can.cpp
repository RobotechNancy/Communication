/*!
 * @file can.h
 * @version 1.0
 * @date 2022-2023
 * @author Julien PISTRE
 * @brief Fichier source de la classe Can
 * @details Version modifiée de la librairie de Théo RUSINOWITCH (v4.1a)
 */

#include "can.h"
using namespace std;


Can::Can(CAN_EMIT_ADDR emit_addr) :
    sock(0),
    logger("can"),
    listen_thread(),
    emit_addr(emit_addr)
{}


/*!
 * @brief  <br>Initialiser la carte liée au bus CAN
 * @param  emit_addr L'adresse de réception de la carte
 * @return 0 ou un code d'erreur
 */
int Can::init() {
    ifreq ifr{};
    sockaddr_can addr{};

    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return CAN_E_SOCKET_ERROR;
    }

    strcpy(ifr.ifr_name, CAN_BUS_NAME);
    ioctl(sock, SIOCGIFINDEX, &ifr);

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return CAN_E_BIND_ERROR;
    }

    struct can_filter rfilter[1];
    rfilter[0].can_id = emit_addr;
    rfilter[0].can_mask = CAN_FILTER_ADDR_RECEPTEUR;

    logger << "Bus CAN initialisé" << mendl;
    return 0;
}


/*!
 * @brief Lier un code fonction à une fonction
 * @param fct_code Le code fonction à écouter
 * @param callback La fonction à exécuter
 */
void Can::subscribe(uint32_t fct_code, const can_callback_t& callback) {
    listeners.insert(std::make_pair(fct_code, callback));
}


/*!
 * @brief <br>Démarrer un thread d'écoute du bus CAN
 */
void Can::start_listen() {
    listen_thread = std::make_unique<thread>(&Can::listen, this);
    logger << "Thread d'écoute du CAN démarré" << mendl;
}


[[noreturn]] void Can::listen() {
    int err;
    can_frame frame{};
    can_mess_t response;

    while(true) {
        if((err = format_frame(response, frame)) < 0){
            logger << "Erreur dans le décodage d'une trame (err n°" << dec << err << ")" << mendl;
            continue;
        }

        logger << "GET : ";
        logger << "   recv_addr : " << hex << response.recv_addr;
        logger << "   emit_addr : " << response.emit_addr;
        logger << "   fct_code : " << response.fct_code;
        logger << "   rep_id : " << response.rep_id;
        logger << "   is_rep : " << response.is_rep;
        logger << "   data : [" << response.data_len << "] ";

        for (int i = 0; i < response.data_len ; i++)
            logger << hex << showbase << (int) response.data[i] << " ";

        logger << mendl;

        if (listeners.contains(response.fct_code)) {
            listeners[response.fct_code](response);
            continue;
        }

        logger << "Code fonction non traité : " << hex << showbase << response.fct_code << mendl;
    }
}


/*!
 * @brief Remplir un object "can_mess_t" à partir d'un objet "can_frame"
 * @param response L'objet à remplir
 * @param frame Le message reçu
 * @return Un code d'erreur
 */
int Can::format_frame(can_mess_t &response, can_frame& frame) const {
    if (read(sock, &frame, sizeof(struct can_frame)) < 0) {
        perror("Read");
        return CAN_E_READ_ERROR;
    }

    if (frame.can_dlc > 8)
        return CAN_E_DATA_SIZE_TOO_LONG;

    response.recv_addr = (frame.can_id & CAN_FILTER_ADDR_EMETTEUR);
    response.emit_addr = (frame.can_id &  CAN_FILTER_ADDR_RECEPTEUR);
    response.fct_code = (frame.can_id & CAN_FILTER_CODE_FCT);
    response.is_rep = (frame.can_id & CAN_FILTER_IS_REP) >> CAN_DECALAGE_IS_REP;
    response.rep_id = (frame.can_id & CAN_FILTER_REP_NBR);
    response.data_len = frame.can_dlc;

    for (int i = 0; i < frame.can_dlc; i++){
        if(frame.data[i] < 0 || frame.data[i] > 255)
            return CAN_E_OOB_DATA;

        response.data[i] = frame.data[i];
    }

    return 0;
}


/*!
 * @brief  <br>Envoyer un message sur le bus CAN
 * @param  addr L'adresse du récepteur
 * @param  fct_code Le code fonction
 * @param  data Les données à envoyer
 * @param  data_len La taille des données
 * @param  is_rep Si le message est une réponse
 * @param  rep_len Le nombre de réponses attendues
 * @param  msg_id L'identifiant du message
 * @return 0 ou un code d'erreur
 */
int Can::send(CAN_ADDR addr, CAN_FCT_CODE fct_code, uint8_t *data, uint8_t data_len, bool is_rep, uint8_t rep_len, uint8_t msg_id) {
    if (data_len > 8) {
        logger << "Vous ne pouvez envoyer que 8 octets de data" << mendl;
        return CAN_E_DATA_SIZE_TOO_LONG;
    }

    can_frame frame{};
    frame.can_dlc = data_len;
    frame.can_id = (uint8_t) addr | emit_addr | fct_code | rep_len | msg_id << CAN_DECALAGE_ID_MSG | is_rep << CAN_DECALAGE_IS_REP | CAN_EFF_FLAG;

    logger << "SEND: ";
    logger << "   recv_addr : " << hex << showbase << addr;
    logger << "   emit_addr : " << emit_addr;
    logger << "   fct_code : " << fct_code;
    logger << "   rep_id : " << msg_id;
    logger << "   is_rep : " << is_rep;
    logger << "   data : ["<< data_len <<"] ";

    for (int i = 0; i < data_len; i++) {
        if(data[i] <0 || data[i] > 255)
            return CAN_E_OOB_DATA;

        frame.data[i] = data[i];
        logger << hex << showbase << (int) data[i] << " ";
    }

    logger << mendl;

    if (write(sock, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write");
        return CAN_E_WRITE_ERROR;
    }

    return 0;
}


void Can::close() const {
    ::close(sock);
}