#ifndef __ESPNOW_H__
#define __ESPNOW_H__
#define bot 1
#include<esp_now.h>
#include <Preferences.h>
#include "Arduino.h"
#define CHANNEL 1
#define PRINTSCANRESULTS 0


class Espnow {

   
public:
  Espnow();
  void BotInit(void);
  void RemoteInit(void);
 
  void OnRemotSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  int8_t OnRemotRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
   
  void OnBotSent(const uint8_t *mac_addr, esp_now_send_status_t status); 
  int8_t OnBotRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

  void RemoteConnectUpdate(void);
  void BotConnectUpdate(void);

   //uint8_t peer_addr[6]= {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};
public:
   uint8_t peer_addr[6];//= {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};
   esp_now_peer_info_t slave;
private:

  void ConfigDeviceAP(void);
  void InitEspNow(void);
  void ScanForSlave(void);
  void manageSlave(void); 
  void sendData(void);
   
private:
  Preferences preferences; 
  uint8_t sta_addr[6];
  //esp_now_peer_info_t slave;
  esp_now_peer_info_t slaves[20] = {};
  int SlaveCnt = 0;
  bool connectflag = false; 
  uint8_t connect_addr[20][6];
  int connect_num;
  //uint8_t peer_addr[6]= {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};
};
#endif
