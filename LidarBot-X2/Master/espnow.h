#ifndef __ESPNOW_H
#define __ESPNOW_H

#include <esp_now.h>
#include <Preferences.h>
#include <WiFi.h>

#define SLAVE_CHANNEL    1
#define CHANNEL          1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

typedef void (*recvCB)(const uint8_t *mac_addr, const uint8_t *data,
                       int data_len);
typedef void (*sendCB)(const uint8_t *, esp_now_send_status_t);

class EspNow {
   public:
    static esp_now_peer_info_t slave;

    EspNow();
    ~EspNow();
    static void sendData(void *buff, int len);
    void deletePeer(void);
    bool manageSlave(void);
};

struct peerList {
    esp_now_peer_info_t list[40];
    uint8_t count = 0;
};

class EspNowMaster : public EspNow {
   public:
    bool isConnected = false;
    static peerList peerlist;
    static recvCB recvCallBack;
    static sendCB sendCallBack;

    EspNowMaster();
    ~EspNowMaster();
    static int8_t isPeerExist(esp_now_peer_info_t peer_info);
    static void OnDataSent(const uint8_t *mac_addr,
                           esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                           int data_len);
    void setRecvCallBack(recvCB cb) {
        recvCallBack = cb;
    };
    void setSendCallBack(sendCB cb) {
        sendCallBack = cb;
    };
    void InitBroadcastSlave(void);
    bool confirmPeer(esp_now_peer_info_t peer);
    void Broadcast();
    void Init();

   private:
    Preferences preferences;
};

class EspNowSlave : public EspNow {
   public:
    bool isConnected = false;
    static peerList peerlist;
    // static void (*recvCallBack)(const uint8_t *mac_addr, const uint8_t *data,
    // int data_len);
    static recvCB recvCallBack;
    static sendCB sendCallBack;

    EspNowSlave();
    ~EspNowSlave();
    void Ack(esp_now_peer_info_t peer);
    void Init();
    void configDeviceAP();
    static int8_t isPeerExist(esp_now_peer_info_t peer);
    static void OnDataSent(const uint8_t *mac_addr,
                           esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                           int data_len);
    void setRecvCallBack(recvCB cb) {
        recvCallBack = cb;
    };
    void setSendCallBack(sendCB cb) {
        sendCallBack = cb;
    };

   private:
    Preferences preferences;
};

#endif
