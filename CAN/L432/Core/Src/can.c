/**
 ******************************************************************************
 * @file           : can.c
 * @brief          : Gestion du bus can
 * @author 		   : Theo RUSINOWITCH <teo.rusi@hotmail.fr>
 ******************************************************************************
 */
#include <stdlib.h>
#include <string.h>

#include "can.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan_p;
extern graph_state_t state;


/*!
 * \struct CanResponse_t
 * \brief contient un message décoder venant du bus can
 * \param addr addresse du destinataire du message
 * \param emetteur adresse de l'émeteur du message
 * \param codeFct code fonction du message
 * \param isRep vrai si c'est une reponse a une requete, faux sinon
 * \param RepId indique le nombre de reponse atendu pour une requete et le num de la reponse pour une reponse
 * \param dataLen frame payload length in byte (0 .. 8) aka data length code
 * \param data CAN frame payload (up to 8 byte)
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	uint8_t RxData[8];
	CAN_RxHeaderTypeDef RxHeader;
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);

	uint8_t data[1];
	can_mess_t msg = process_frame(RxHeader, RxData);

	switch(msg.fct_code) {
	case FCT_CHANGEMENT_ETAT:
		state.len = msg.data_len;
		memcpy(state.data, msg.data, msg.data_len);
		send(CAN_ADDR_RASPBERRY, FCT_CHANGEMENT_ETAT, msg.data, msg.data_len, false, 0, msg.message_id);
	case FCT_ACCUSER_RECPETION:
		data[0] = 0;
		send(CAN_ADDR_RASPBERRY, FCT_ACCUSER_RECPETION, data, 1, true, 1, msg.message_id);
	break;
	default:
	break;
	}
}


void configure_CAN(CAN_HandleTypeDef hcan, CAN_EMIT_ADDR addr) {
	CAN_FilterTypeDef sFilterConfig;

	sFilterConfig.FilterMode =           CAN_FILTERMODE_IDMASK; // Filtrage par liste ou par masque
	sFilterConfig.FilterScale =          CAN_FILTERSCALE_16BIT; // Filtre de 32 bits ou 1 de 16 bits
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;          // 3 files avec 3 filtres par file
	sFilterConfig.SlaveStartFilterBank = 14;                    // Choix du filtre dans la banque
	sFilterConfig.FilterActivation =     ENABLE;
	sFilterConfig.FilterMaskIdLow =      0b111100000000000;     // Masque LSBs
	sFilterConfig.FilterMaskIdHigh =     0b111100000000000;     // Masque MSBs

	sFilterConfig.FilterBank =           0;
	sFilterConfig.FilterIdHigh =         addr >> 9;             // Adresse de l'émetteur
	sFilterConfig.FilterIdLow =          0b111100000000000;     // Adresse de broadcast

	HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

	HAL_CAN_Start(&hcan); // Démarrer le périphérique CAN
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING); // Activer le mode interruption

	hcan_p = hcan;
}


int send(CAN_ADDR addr, CAN_FCT_CODE fct_code, uint8_t data[], uint8_t data_len, bool is_rep, uint8_t rep_len, uint8_t msg_id){
	if (data_len > 8)
		return CAN_E_DATA_SIZE_TOO_LONG;

	if (addr > CAN_MAX_VALUE_ADDR) return CAN_E_OOB_ADDR;
	if (fct_code > CAN_MAX_VALUE_CODE_FCT) return CAN_E_OOB_CODE_FCT;
	if(rep_len < 0 || rep_len > CAN_MAX_VALUE_REP_NBR) return CAN_E_OOB_REP_NBR;

	CAN_TxHeaderTypeDef txHeader;
	txHeader.DLC = data_len;
	txHeader.ExtId = addr | CAN_ADDR_BASE_ROULANTE_E | fct_code | rep_len | msg_id << CAN_DECALAGE_ID_MSG | is_rep << CAN_DECALAGE_IS_REP | rep_len;
	txHeader.IDE = CAN_ID_EXT;
	txHeader.RTR = CAN_RTR_DATA;
	txHeader.TransmitGlobalTime = DISABLE;

	uint32_t TxMailbox;
	HAL_CAN_AddTxMessage(&hcan_p, &txHeader, data, &TxMailbox);

	return 0;
}


can_mess_t process_frame(CAN_RxHeaderTypeDef frame, const uint8_t data[]){
	can_mess_t rep;

	rep.recv_addr = (frame.ExtId & CAN_FILTER_ADDR_EMETTEUR);
	rep.emit_addr = (frame.ExtId &  CAN_FILTER_ADDR_RECEPTEUR);
	rep.fct_code = (frame.ExtId & CAN_FILTER_CODE_FCT);
	rep.is_rep = (frame.ExtId & CAN_FILTER_IS_REP) >> CAN_DECALAGE_IS_REP;
	rep.rep_id = (frame.ExtId & CAN_FILTER_REP_NBR) ;
	rep.message_id = (frame.ExtId & CAN_FILTER_IDE_MSG) >> CAN_DECALAGE_ID_MSG;

	if(rep.recv_addr < 0 || rep.recv_addr > CAN_MAX_VALUE_ADDR) exit(CAN_E_OOB_ADDR);
	if(rep.fct_code < 0 || rep.fct_code > CAN_MAX_VALUE_CODE_FCT) exit(CAN_E_OOB_CODE_FCT);
	if(rep.rep_id < 0 || rep.rep_id > CAN_MAX_VALUE_REP_NBR) exit(CAN_E_OOB_REP_NBR);
	if (frame.DLC > 8) exit(CAN_E_DATA_SIZE_TOO_LONG);

	rep.data_len = frame.DLC;

	for (int i = 0; i < frame.DLC; i++){
		/*if(data[i] <0 || data[i] > 255)
			return CAN_E_OOB_DATA;*/

		rep.data[i] = data[i];
	}

    return rep;
}
