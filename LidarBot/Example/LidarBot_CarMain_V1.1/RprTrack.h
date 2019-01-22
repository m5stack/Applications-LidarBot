#ifndef __RPRTRACK_H__
#define __RPRTRACK_H__
#include <M5Stack.h>
class Rprtrack {

   
public:
   Rprtrack();
   void TrackControl(void);
   void SensorStatus(void);
   void CalTrackDev(void);
  /*
  void BotInit(void);
  void RemoteInit(void);
 
  void OnRemotSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void OnRemotRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
   
  void OnBotSent(const uint8_t *mac_addr, esp_now_send_status_t status); 
  void OnBotRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

  void RemoteConnectUpdate(void);
  void BotConnectUpdate(void);
  */
   //uint8_t peer_addr[6]= {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};
public:
 //int Original_X;
 //int Original_Y;
 int OffsetLine;
/*
   uint8_t peer_addr[6];//= {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};
   esp_now_peer_info_t slave;
   */
private:
/*
  void ConfigDeviceAP(void);
  void InitEspNow(void);
  void ScanForSlave(void);
  void manageSlave(void); 
  void sendData(void);
  */
   
private: 
 int sensorValue[4];
 //int OffsetLine;
 int LastOffsetLine;
/*
  Preferences preferences; 
  uint8_t sta_addr[6];
  //esp_now_peer_info_t slave;
  esp_now_peer_info_t slaves[20] = {};
  int SlaveCnt = 0;
  bool connectflag = false; 
  uint8_t connect_addr[20][6];
  int connect_num;
  //uint8_t peer_addr[6]= {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};
  */
};


#endif
