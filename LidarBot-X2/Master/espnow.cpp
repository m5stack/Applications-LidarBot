#include "espnow.h"

EspNow::EspNow() {
    memset(&slave, 0, sizeof(slave));
}

EspNow::~EspNow() {
}

esp_now_peer_info_t EspNow::slave;

// send data
void EspNow::sendData(void *buf, int len) {
    const uint8_t *peer_addr = slave.peer_addr;
    // Serial.print("Sending: "); Serial.println(data);
    esp_err_t result = esp_now_send(peer_addr, (uint8_t *)buf, len);
    /*
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
    } else {
      Serial.println("Not sure what happened");
    }*/
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool EspNow::manageSlave() {
    if (slave.channel == CHANNEL) {
        if (DELETEBEFOREPAIR) {
            deletePeer();
        }

        Serial.print("Slave Status: ");
        const esp_now_peer_info_t *peer = &slave;
        const uint8_t *peer_addr        = slave.peer_addr;
        // check if the peer exists
        bool exists = esp_now_is_peer_exist(peer_addr);
        if (exists) {
            // Slave already paired.
            Serial.println("Already Paired");
            return true;
        } else {
            // Slave not paired, attempt pair
            esp_err_t addStatus = esp_now_add_peer(peer);
            if (addStatus == ESP_OK) {
                // Pair success
                Serial.println("Pair success");
                return true;
            } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
                // How did we get so far!!
                Serial.println("ESPNOW Not Init");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
                Serial.println("Invalid Argument");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
                Serial.println("Peer list full");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
                Serial.println("Out of memory");
                return false;
            } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
                Serial.println("Peer Exists");
                return true;
            } else {
                Serial.println("Not sure what happened");
                return false;
            }
        }
    } else {
        // No slave found to process
        Serial.println("No Slave found to process");
        return false;
    }
}

void EspNow::deletePeer() {
    const esp_now_peer_info_t *peer = &slave;
    const uint8_t *peer_addr        = slave.peer_addr;
    esp_err_t delStatus             = esp_now_del_peer(peer_addr);
    Serial.print("Slave Delete Status: ");
    if (delStatus == ESP_OK) {
        // Delete success
        Serial.println("Success");
    } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
    } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
    } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
        Serial.println("Peer not found.");
    } else {
        Serial.println("Not sure what happened");
    }
}

EspNowMaster::EspNowMaster() {
    memset(peerlist.list, 0, sizeof(peerlist.list));
}

EspNowMaster::~EspNowMaster() {
}

peerList EspNowMaster::peerlist;

// broadcast every address that scanSlaveList found
void EspNowMaster::InitBroadcastSlave() {
    // clear slave data
    memset(&slave, 0, sizeof(slave));
    for (int ii = 0; ii < 6; ++ii) {
        slave.peer_addr[ii] = (uint8_t)0xff;
    }
    slave.channel = CHANNEL;  // pick a channel
    slave.encrypt = 0;        // no encryption
    manageSlave();
}

void EspNowMaster::Broadcast() {
    if (isConnected) {
    } else {
        uint8_t req[] = {0xaa, 0x66};
        sendData(req, 2);
    }
}

void EspNowMaster::Init(void) {
    WiFi.mode(WIFI_STA);
    Serial.print(" MAC: ");
    Serial.println(WiFi.macAddress());

    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init Success");
    } else {
        Serial.println("ESPNow Init Failed");
        // Retry InitESPNow, add a counte and then restart?
        // InitESPNow();
        // or Simply Restart
        ESP.restart();
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    // ScanForSlave();

    InitBroadcastSlave();
}

int8_t EspNowMaster::isPeerExist(esp_now_peer_info_t peer) {
    char macStr[18];
    if (esp_now_is_peer_exist(peer.peer_addr)) {
        Serial.printf("this peer is already exist\r\n");
        return 1;
    } else {
        for (int i = 0; i < 6; i++) {
            peerlist.list[peerlist.count].peer_addr[i] = peer.peer_addr[i];
            // slave.peer_addr[i] = peer.peer_addr[i];
        }
        peerlist.count++;
        esp_now_add_peer(&peer);
        return 0;
    }
}

bool EspNowMaster::confirmPeer(esp_now_peer_info_t peer) {
    for (int i = 0; i < 6; i++) {
        slave.peer_addr[i] = peer.peer_addr[i];
    }
    isConnected   = true;
    slave.channel = CHANNEL;  // pick a channel
    slave.encrypt = 0;
    manageSlave();
    return true;
}

void EspNowMaster::OnDataSent(const uint8_t *mac_addr,
                              esp_now_send_status_t status) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    //   Serial.print("Last Packet Sent to: "); Serial.println(macStr);
    //   Serial.print("Last Packet Send Status: "); Serial.println(status ==
    //   ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

    if (sendCallBack != NULL) {
        sendCallBack(mac_addr, status);
    }
}

void EspNowMaster::OnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                              int data_len) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    // Serial.print("Last Packet Recv from: "); Serial.println(macStr);
    // Serial.print("Last Packet Recv Data: "); Serial.println(*data);
    // Serial.println("");

    if (data_len == 2 && data[0] == 0xaa && data[1] == 0x77) {
        Serial.printf("recv slave confirm");
        Serial.println(macStr);
        esp_now_peer_info_t peer;
        memset(&peer, 0, sizeof(peer));
        for (int i = 0; i < 6; i++) {
            peer.peer_addr[i] = mac_addr[i];
        }
        isPeerExist(peer);
    }

    if (recvCallBack != NULL) {
        recvCallBack(mac_addr, data, data_len);
    }
}

recvCB EspNowMaster::recvCallBack;
sendCB EspNowMaster::sendCallBack;

EspNowSlave::EspNowSlave() {
    memset(peerlist.list, 0, sizeof(peerlist.list));
}

EspNowSlave::~EspNowSlave() {
}

void EspNowSlave::configDeviceAP() {
    WiFi.mode(WIFI_AP_STA);

    String mac  = WiFi.softAPmacAddress();
    String SSID = "Slave" + mac;
    bool result =
        WiFi.softAP(SSID.c_str(), "Slave_1_Password", SLAVE_CHANNEL, 0);
    if (!result) {
        Serial.println("AP Config failed.");
    } else {
        Serial.println("AP Config Success. Broadcasting with AP: " +
                       String(SSID));
    }
}

peerList EspNowSlave::peerlist;

void EspNowSlave::Init() {
    configDeviceAP();

    preferences.begin("test", false);
    String mac_addr;
    mac_addr = preferences.getString("mac_addr", "error");
    Serial.print("Slavemac_addr = ");
    Serial.print(mac_addr);

    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init Success");
    } else {
        Serial.println("ESPNow Init Failed");
        // Retry InitESPNow, add a counte and then restart?
        // InitESPNow();
        // or Simply Restart
        ESP.restart();
    }

    char peer_addr[6] = {0};
    memset(&slave, 0, sizeof(slave));
    sscanf(mac_addr.c_str(), "%x:%x:%x:%x:%x:%x%c", &slave.peer_addr[0],
           &slave.peer_addr[1], &slave.peer_addr[2], &slave.peer_addr[3],
           &slave.peer_addr[4], &slave.peer_addr[5]);

    for (int i = 0; i < 6; ++i) {
        //    slave.peer_addr[i] = (uint8_t) peer_addr[i];
        Serial.printf("%x\r\n", slave.peer_addr[i]);
    }
    slave.channel = SLAVE_CHANNEL;
    //   esp_now_add_peer(&slave);

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void EspNowSlave::OnDataSent(const uint8_t *mac_addr,
                             esp_now_send_status_t status) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    if (sendCallBack != NULL) {
        sendCallBack(mac_addr, status);
    }
}

void EspNowSlave::Ack(esp_now_peer_info_t peer) {
    slave         = peer;
    slave.channel = SLAVE_CHANNEL;
    slave.encrypt = 0;
    slave.ifidx   = WIFI_IF_AP;
    isPeerExist(slave);
    // Serial.println("add peer");
    uint8_t ackdata[] = {0xaa, 0x77};
    Serial.print("macaddress");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%x:", slave.peer_addr[i]);
    }
    // Serial.println("add peer");
    Serial.println();
    isConnected = true;
    sendData(ackdata, 2);
}

int8_t EspNowSlave::isPeerExist(esp_now_peer_info_t peer) {
    char macStr[18];
    if (esp_now_is_peer_exist(peer.peer_addr)) {
        Serial.printf("this peer is already exist\r\n");
        return 1;
    } else {
        for (int i = 0; i < 6; i++) {
            peerlist.list[peerlist.count].peer_addr[i] = peer.peer_addr[i];
            // slave.peer_addr[i] = peer.peer_addr[i];
        }
        peerlist.count++;
        esp_now_add_peer(&peer);
        return 0;
    }
}

void EspNowSlave::OnDataRecv(const uint8_t *mac_addr, const uint8_t *data,
                             int data_len) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);

    if (data_len == 2 && data[0] == 0xaa && data[1] == 0x66) {
        Serial.println("recv aa 66");
        esp_now_peer_info_t peer;
        for (int i = 0; i < 6; i++) {
            peer.peer_addr[i] = mac_addr[i];
        }
        peer.channel = SLAVE_CHANNEL;  // pick a channel
        peer.encrypt = 0;              // no encryption
        peer.ifidx   = WIFI_IF_AP;
        isPeerExist(peer);
    }

    if (recvCallBack != NULL) {
        recvCallBack(mac_addr, data, data_len);
    }
}

recvCB EspNowSlave::recvCallBack;
sendCB EspNowSlave::sendCallBack;