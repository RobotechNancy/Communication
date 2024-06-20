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

    while (true) {
    //demande d'information à une caméra présise avec fonction de demande des positions, aucune donnée n'est envoyé, et un temps d'attente d'info max de 5 seconde (s'arrete si aucune réponse)
    xbee_result_t res = xbee.send(XB_ADDR_CAMERA_01, XB_FCT_GET_ARUCO_POS, {}, 5);

    //arret du programme en cas d'erreur
    if (res.status != XB_E_SUCCESS) {
        std::cout << "Erreur de communication" << std::endl;
        //return 1;
        continue;
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
        uint16_t X = (data[i+1] << 8) | data[i+2];
        uint16_t Y = (data[i+3] << 8) | data[i+4];
        uint16_t Z = (data[i+5] << 8) | data[i+6];
        uint16_t TZ = (data[i+7] << 8) | data[i+8];
        //push_back = ajouter à la fin les varibales (créé une nouvelle ligne)
        markers_pos.push_back({IDc, X, Y, Z, TZ});
        std::cout << std::dec << IDc << " : x= " << X << ", y= " << Y << ", z= " << Z << ", Tz= " << TZ << std::endl;
    }

    std::array<uint16_t, 5> position_robot;
    int markerRobot = 53;
    int numeroCamera = 1;
    bool empty = 0;
    int pasTrouve = 0;

    for (int i=0; i < markers_pos.size(); i++){
        if (markers_pos[i][0] == markerRobot) {
            pasTrouve = -markers_pos.size()+1;
            empty = 0;
            uint16_t Xr = markers_pos[i][1];
            uint16_t Yr = markers_pos[i][2];
            uint16_t TZr = markers_pos[i][4];
            uint16_t numCam = numeroCamera;
            position_robot = {empty, Xr, Yr, TZr, numCam};
            }
        else pasTrouve++;
    }

    if (pasTrouve >= 0 ) {
        uint16_t Xr = 0;
        uint16_t Yr = 0;
        uint16_t TZr = 0;
        uint16_t numCam = numeroCamera;
        position_robot = {empty, Xr, Yr, TZr, numCam};
        std::cout << "Le Robot n'a pas été repéré" << std::endl;
        }
    else std::cout << "Le Robot a  été repéré aux coords : Xr= " << position_robot[1] << ", Yr= " << position_robot[2] << ", et orientation TZr= " << position_robot[3] << std::endl;

    }

    return XB_E_SUCCESS;
}
