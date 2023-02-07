# Librairie C++ du CAN (Raspberry)

## Installation

Dans le `.ioc` :
- Activer le bus **can1** ou **can**
- Dans **Parameter Settings**, modifier :
    - **Prescaler (for Time Quantum)**
    - **Time Quanta in Bit Segment 1**
    - **Time Quanta in Bit Segment 2**

Le tout, de tel sorte à obtenir le **Baud Rate** voulu (identique aux autres cartes, 181818 bit/s par example).
Sur une carte en confguration par defaut :

| PARAMETRE                        | VALEUR  |
|----------------------------------|---------|
| **Prescaler (for Time Quantum)** | 16      |
| **Time Quanta in Bit Segment 1** | 8 Times | 
| **Time Quanta in Bit Segment 2** | 2 Times | 

- Tout cocher dans **NVIC Settings** (4 cases ou 1 en fonction de la carte)
- Vérifier que les pins sont bien associés :

| PIN  | DESCRIPTION |
|------|-------------|
| PA12 | CAN1_TX     |
| PA11 | CAN1_RX     |

**Attention :** S'assurer que le CAN est bien démarré :
```bash
sudo ip link set can0 up type can bitrate 181818 loopback off
```
Pour des tests avec la base roulante, il faut aussi activer le relais :
```bash
cd interface-maintenance/public/cpp
./maintenance Relais,ON
./maintenance TestComm,BR
```
