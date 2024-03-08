/*!
 * @file    main.cpp
 * @version 1.0
 * @date    2022-2023
 * @author  Julien PISTRE
 * @brief   Fichier pour tester la librairie XBee
 * @details Version modifiée de la librairie de Samuel-Charles DITTE-DESTREE (v3.0)
 */

#include "xbee.h"


int main() {
    XBee xbee(XB_ADDR_ROBOT_01);
    int status = xbee.open("/dev/ttyS0");

    if (status != XB_E_SUCCESS)
        return status;
    
    //démarer l'écoute pour reception de trame (car sur processus séparé)
    xbee.startListening();

    //demande d'information à une caméra présise avec fonction de demande des positions, aucune donnée n'est envoyé, et un temps d'attente d'info max de 5 seconde (s'arrete si aucune réponse)
    xbee_result_t res = xbee.send(XB_ADDR_CAMERA_01, XB_FCT_GET_ARUCO_POS, {}, 5);

    //arret du programme en cas d'erreur
    if (res.status != XB_E_SUCCESS) {
        std::cout << "Erreur de communication" << std::endl;
        return 1;
    }

    //raccourcis vers les données envoyés récupérés
    std::vector<uint8_t>& data = res.frame.data;
    
    //récupération de l'id du marqueur de reférence utilisé par la caméra concerné
    //utilise "uint16_t" au lieu de "int" pour avoir un entier non-signé compatible pour tout OS
    uint16_t ref_id = data[0];       

    //création vecteur (s'aggrandi automatiquement si besoin plus d'espace) pour les infos de position des codes
    std::vector<std::array<uint16_t, 5>> markers_pos;

    //récupération des infos des codes
    // => [Id_code, position_X_code_/_code_ref, position_Y_..., position_Z_..., orientation_axe_Z_...]
    for (int i=1; i < data.size(); i+=9) {
        uint16_t IDc = data[i];
        uint16_t Xr = (data[i+1] << 8) | data[i+2];
        uint16_t Yr = (data[i+3] << 8) | data[i+4];
        uint16_t Zr = (data[i+5] << 8) | data[i+6];
        uint16_t TZr = (data[i+7] << 8) | data[i+8];
        //push_back = ajouter à la fin les varibales (créé une nouvelle ligne)
        markers_pos.push_back({IDc, Xr, Yr, Zr, TZr});
        std::cout << IDc << Xr << Yr << Zr << TZr << std::endl;
    }

    return XB_E_SUCCESS;
}
