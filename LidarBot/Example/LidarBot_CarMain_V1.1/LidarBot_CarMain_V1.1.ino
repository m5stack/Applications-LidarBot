// 2018/10/26
// AP MAC: 30:AE:A4:67:2F:9D
#include <M5Stack.h>
#include <Preferences.h>
#include "esp32-hal-uart.h"
#include <esp_now.h>
#include <WiFi.h>
#include <Math.h>
uint16_t distance[512];
uint8_t signalValue[512];
int commandStatus = 0;
int frame = 0;
int ver = 0;
int frameType = 0;
int commandType = 0;
uint16_t dataLength = 0;
int lidarSpeed = 0;
uint16_t angleOffset = 0;
uint16_t startAngle = 0;
uint16_t temp = 0;
int ox = 160, oy = 200;
int showAngle = 0;
int disX[360], disY[360];
int oldDisX[360], oldDisY[360];
int packCount = 0;
bool connectflag = false;
int connect_num;
//extern const unsigned char gImage_bk[];
#define CHANNEL 1

//84:0D:8E:25:B3:64
esp_now_peer_info_t slave;
uint8_t connect_addr[20][6] = {0};
uint8_t peer_addr[6] = {0x84, 0x0D, 0x8E, 0x25, 0xBC, 0xE8};
//const uint8_t peer_addr[6] = {0xB4, 0xE6, 0x2D, 0x8B, 0x7d, 0x42};
uint8_t phy_addr[6] = {1,2,3,4,5,6};
// Init ESP Now with fallback
Preferences preferences;
void InitESPNow()
{
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// config AP SSID

void configDeviceAP()
{
  char* SSID = "Slave_2";
  bool result = WiFi.softAP(SSID, "12345678", CHANNEL, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  } else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

int wheelPowerA = 0, wheelPowerB = 0, wheelPowerC = -0, wheelPowerD = -0;
int controlMap[15][15] =
{
  {  4,  4,  5,  5,  6,  6,  7,  7,  7,  7, 7, 7, 7, 7, 7},
  {  2,  3,  4,  4,  5,  5,  6,  6,  6,  6, 6, 6, 6, 6, 7},
  {  0,  1,  3,  4,  4,  5,  5,  5,  5,  5, 5, 5, 5, 6, 7},
  { -1,  0,  1,  2,  2,  3,  4,  4,  4,  4, 4, 4, 5, 6, 7},
  { -2, -1,  0,  0,  2,  2,  3,  3,  3,  3, 3, 4, 5, 6, 7},
  { -4, -3, -1, -1,  0,  1,  2,  2,  2,  2, 3, 4, 5, 6, 7},
  { -6, -5, -3, -2, -1,  0,  0,  1,  1,  2, 3, 4, 5, 6, 7},
  { -7, -6, -5, -4, -3, -2, -1,  0,  1,  2, 3, 4, 5, 6, 7},
  { -7, -6, -5, -4, -3, -2,  0, -1, -1,  0, 1, 2, 3,  4, 5},
  { -6, -5, -4, -3, -2, -1,  0, -2, -2, -2, -1, 0, 1, 2, 3},
  { -6, -5, -4, -3, -2, -2, -3, -3, -3, -3, -3, -2, -1, 0, 1},
  { -5, -4, -3, -2, -3, -3, -4, -4, -4, -4,  -4,  -4,  -3,  2, -1},
  { -5, -4, -3, -3, -4, -4, -5, -5, -5, -5,  -5,  -5,  -5,  4, -3},
  { -4, -3, -4, -4, -5, -5, -6, -6, -6, -6,  -6,  -6,  -6,  -6,  -5},
  { -4, -4, -5, -5, -6, -6, -7, -7, -7, -7,  -7,  -7,  -7,  -7,  -7}
};
int controlMapX[15][15] =
{
  { -2, -2, -1, 0, 1, 2, 3, 7, 4, 4, 4, 4, 4, 4, 4},
  { -2, -2, -1, 0, 0, 1, 2, 4, 3, 3, 3, 3, 3, 3, 4},
  { -3, -2, -1, -1, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 4},
  { -3, -2, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 3, 3, 4},
  { -3, -2, -2, -1, -1, 0, 1, 2, 2, 2, 2, 2, 3, 3, 4},
  { -3, -2, -2, -2, -1, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4},
  { -3, -2, -2, -2, -2, -1, 0, 1, 1, 1, 2, 2, 3, 3, 4},
  { -7, -4, -2, -2, -2, -1, -1, 0, 1, 1, 2, 2, 2, 4, 7},
  { -4, -4, -2, -2, -2, -1, -1, -1, 0, 1, 2, 2, 2, 2, 3},
  { -4, -4, -2, -2, -2, -1, -1, -1, 0, 0, 1, 2, 2, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 2, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 1, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 2, 3},
  { -4, -4, -4, -4, -4, -4, -4, -4, -2, -1, 0, 0, 1, 2, 2},
  { -4, -4, -4, -4, -4, -4, -4, -7, -3, -2, -1, 0, 1, 2, 2}

};
int controlMapY[15][15] =
{
  {4, 4, 4, 4, 4, 4, 4, 7, 3, 2, 1, 0, -1, -2, -2} ,
  {4, 3, 3, 3, 3, 3, 3, 4, 2, 1, 0, 0, -1, -2, -2} ,
  {4, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, -1, -1, -2, -3},
  {4, 3, 3, 2, 2, 2, 2, 2, 1, 0, -1, -1, -1, -2, -3} ,
  {4, 3, 3, 2, 2, 2, 2, 2, 1, 0, -1, -1, -2, -2, -3} ,
  {4, 3, 3, 2, 2, 2, 1, 1, 0, 0, -1, -2, -2, -2, -3} ,
  {4, 3, 3, 2, 2, 1, 1, 1, 0, -1, -2, -2, -2, -2, -3} ,
  {7, 4, 2, 2, 2, 1, 1, 0, -1, -1, -2, -2, -2, -4, -7} ,
  {3, 2, 2, 2, 2, 1, 0, -1, -1, -1, -2, -2, -2, -4, -4} ,
  {3, 2, 2, 2, 1, 0, 0, -1, -1, -1, -2, -2, -2, -4, -4} ,
  {3, 2, 2, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {3, 2, 1, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {3, 2, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {2, 2, 1, 0, 0, -1, -2, -4, -4, -4, -4, -4, -4, -4, -4} ,
  {2, 1, 0, 0, -1, -2, -3, -7, -4, -4, -4, -4, -4, -4, -4}
};
void controlWheel(int8_t X, int8_t Y, byte A)
{

  if (A == 0x01)
  {
    wheelPowerA = controlMapX[7 + Y][X + 7];
    wheelPowerB = controlMapY[7 + Y][X + 7];
    wheelPowerC = controlMapY[7 + Y][X + 7];
    wheelPowerD = controlMapX[7 + Y][X + 7];
  }
  else
  {
    wheelPowerA = controlMap[-Y + 7][X + 7];
    wheelPowerB = controlMap[-Y + 7][14 - X - 7];
    wheelPowerC = controlMap[-Y + 7][X + 7];
    wheelPowerD = controlMap[-Y + 7][14 - X - 7];
  }

  Serial2.write(0xAA);
  Serial2.write(wheelPowerA);
  Serial2.write(wheelPowerB);
  Serial2.write(wheelPowerC);
  Serial2.write(wheelPowerD);
  Serial2.write(0x55);
}
void setLedColor(byte i, byte r, byte g, byte b)
{
  Serial2.write(0xAB);
  Serial2.write(i);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}
void setLedBar(byte fb, byte r, byte g, byte b)
{
  Serial2.write((fb == 0) ? 0xAC : 0xAD);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}
void setLedAll( byte r, byte g, byte b)
{
  Serial2.write(0xAE);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}

#define NUMSLAVES 20
#define PRINTSCANRESULTS 0
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;
// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf("Slave") == 0) {
        // SSID of interest
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];

        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x%c",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
      }
    }
  }

  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      const esp_now_peer_info_t *peer = &slaves[i];
      const uint8_t *peer_addr = slaves[i].peer_addr;
      Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
      Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist(peer_addr);
      if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(peer);
        if (addStatus == ESP_OK) {
          // Pair success
          Serial.println("Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          // How did we get so far!!
          Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
          Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          Serial.println("Peer Exists");
        } else {
          Serial.println("Not sure what happened");
        }
        delay(100);
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}

// send data
void sendData() {
  //const uint8_t *phy_addr = peer_addr;
  for (int i = 0; i < SlaveCnt; i++) {
    const uint8_t *scan_addr = slaves[i].peer_addr;
    if (i == 0) { // print only for first slave
      Serial.print("Sending: ");
      //Serial.println(phy_addr);
      //Serial.print(phy_addr);
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) phy_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
    }
    //WiFi.softAPmacAddress()
    //esp_err_t result = esp_now_send(scan_addr, WiFi.softAPmacAddress(), sizeof(WiFi.softAPmacAddress()));
    esp_err_t result = esp_now_send(scan_addr, phy_addr, sizeof(phy_addr));
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
    }
    delay(100);
  }
}
void ConnectUpdate(void)
{
    int ypos = 0;
    connect_num = 0;
    connectflag = false;
    while(1)
    {  
       M5.Lcd.setCursor(0, 0, 4);
       M5.Lcd.println("Broadcasting...");
       M5.Lcd.printf("(");
       M5.Lcd.print(WiFi.softAPmacAddress());
       M5.Lcd.printf(")");
       for(int i = 0; i < connect_num; i++)
       {
           M5.Lcd.setCursor(30, 75 + 25 * i);    // Set cursor near top left corner of screen
           M5.Lcd.printf("%02x:%02x:%02x:%02x:%02x:%02x", connect_addr[i][0], connect_addr[i][1], connect_addr[i][2], connect_addr[i][3], connect_addr[i][4], connect_addr[i][5]);
       }
       //M5.Lcd.setCursor(270, 25 + ypos * 25);
       //M5.Lcd.println("<<");
       //M5.Lcd.setCursor(50, 210);  
      // M5.Lcd.println("up");
      // M5.Lcd.setCursor(125, 210);
       //M5.Lcd.println("down");
       if(connect_num){
           M5.Lcd.setCursor(30, 125);
           M5.Lcd.println("Accept?");
           M5.Lcd.setCursor(220, 210);
           M5.Lcd.println("Confirm");
       }
       if ((digitalRead(37) == LOW) && (connect_num))
       {
           M5.Lcd.fillScreen(TFT_BLACK);
           for (int i = 0; i < 6; ++i )
          {
              peer_addr[i] = (uint8_t) connect_addr[ypos][i];
          }
          for (int i = 0; i < 6; ++i )
         {
               slave.peer_addr[i] = (uint8_t) peer_addr[i];
         }
         slave.channel = CHANNEL;
         esp_now_add_peer(&slave);
         uint8_t ack[4] = {0x41,0x43,0x4B};
         for(int i = 0;i < 6; i++)
         {
             esp_err_t result = esp_now_send(slave.peer_addr, ack, sizeof(ack));
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
          char macStr[18];
          snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", peer_addr[0], peer_addr[1], peer_addr[2], peer_addr[3], peer_addr[4], peer_addr[5]);
          String mac_addr  = (char *)macStr;
          preferences.putString("mac_addr", mac_addr);
          delay(100);
          preferences.end();
          break;
       }
       if (digitalRead(39) == LOW  && (connect_num))
       {
          M5.Lcd.fillScreen(TFT_BLACK);
          ypos--;
          if(ypos <= 0) 
          {
              ypos = 0;
          }
      }

       if(!connect_num)
       {
         ScanForSlave();
         if (SlaveCnt > 0)
         {
             manageSlave();
             sendData();
         } 
       }
  }

}
void setup() {
  m5.begin();
  pinMode(35, INPUT);
  pinMode(36, INPUT);
  pinMode(13, INPUT);
  pinMode(26, INPUT);
  digitalWrite(13, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(36, HIGH);
  Serial.begin(115200);
  Serial1.begin(230400, SERIAL_8N1, 16, 2); //Lidar (actually, we only use GPIO16 for receiving sensor's data. GPIO2 is unused.)
  Serial2.begin(115200, SERIAL_8N1, 5, 17); //Motor (actually, we only use GPIO16 for receiving sensor's data. GPIO5 is unused.) 
  preferences.begin("my-app", false);
  //Set device in AP mode to begin with
  m5.lcd.setBrightness(0);
  // m5.lcd.pushImage(0, 0, 320, 240, (uint16_t *)gImage_bk);
  m5.lcd.setBrightness(100);
  WiFi.mode(WIFI_STA);
  configDeviceAP();
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  M5.Lcd.setCursor(100, 222, 2);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  //M5.Lcd.println(WiFi.softAPmacAddress());
  //phy_addr = WiFi.softAPmacAddress().c_str;
  //sprintf((char *)phy_addr,"%s", WiFi.softAPmacAddress());
  //strcpy((char *)phy_addr,WiFi.softAPmacAddress().c_str());

  sscanf(WiFi.softAPmacAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &phy_addr[0], &phy_addr[1], &phy_addr[2], &phy_addr[3], &phy_addr[4], &phy_addr[5] );
  for (int i = 0; i < 360; i++)
  {
    disX[i] = 0;
    disY[i] = 0;
    oldDisX[i] = 0;
    oldDisY[i] = 0;
  }
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  String mac_addr;
  mac_addr = preferences.getString("mac_addr", "error");
  Serial.print("mac_addr = ");
  Serial.print(mac_addr);
  if(mac_addr != "error"){
  sscanf(mac_addr.c_str(), "%x:%x:%x:%x:%x:%x%c",  &peer_addr[0], &peer_addr[1], &peer_addr[2], &peer_addr[3], &peer_addr[4], &peer_addr[5]);
  for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) peer_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
  }
  else{
         ConnectUpdate();
    }
  #if 0
  ConnectUpdate();
  #else
   for (int i = 0; i < 6; ++i )
         {
               slave.peer_addr[i] = (uint8_t) peer_addr[i];
         }
         slave.channel = CHANNEL;
         esp_now_add_peer(&slave);
  #endif
  for (int i = 0; i < 16; i++)
  {
    setLedColor(i, 20, 20, 20);
    delay(15);
  }
  for (int i = 0; i < 16; i++)
  {
    setLedColor(i, 0, 0, 0);
    delay(15);
  }
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(50);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(120);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(50);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(120);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(50);
  setLedAll(0, 0, 0);

}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18];
  //  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  //  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
   char macStr[18];
   snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

   if((data_len == 7) && (!connectflag))
   {
      connectflag = true;
      int i = 0;
      for(i = 0; i < connect_num; i++)
      {
          int j = 0;
          for(j = 0; j < 6; j++)
          {    
              if(connect_addr[i][j] != mac_addr[j])
              {
                 break;
              }
          }
          //Serial.printf("j = %d",j);
          if(j == 6)
          {
              break;  
          }
        }
        if(i == connect_num)
        {
            for(int i = 0; i < 6;i++)
            {
                connect_addr[connect_num][i] = mac_addr[i];
            }
            connect_num++;
        }
    }
    if(connect_num>= 20) connect_num = 0;
  
    if (data_len == 3)
    {
        controlWheel(data[0], data[1], data[2]);
    }
}
void loop()
{ 
  M5.update();
  if(M5.BtnA.wasReleasefor(800))
  {
      M5.Lcd.fillScreen(TFT_BLACK);
      ConnectUpdate();
     //goto loop; 
  }
  
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.print(dataLength);
  M5.Lcd.print("D/");
  M5.Lcd.print(lidarSpeed);
  M5.Lcd.print("S/");
  M5.Lcd.print(packCount);
  M5.Lcd.setCursor(280, 0, 2);
  M5.Lcd.print(digitalRead(13) == LOW ? '1' : '0');
  M5.Lcd.print(digitalRead(26) == LOW ? '1' : '0');
  M5.Lcd.print(digitalRead(36) == LOW ? '1' : '0');
  M5.Lcd.print(digitalRead(35) == LOW ? '1' : '0');
  
  uint8_t data[180];
  for (int i = 0; i < 45; i++)
  {
    if (showAngle > 360)
      showAngle = 0;
    else
      showAngle++;
    disX[showAngle] = ox + (distance[showAngle] / 70) * cos(3.14159 * showAngle / 180 + 0.13);
    disY[showAngle] = oy + (distance[showAngle] / 70) * sin(3.14159 * showAngle / 180 + 0.13);
    M5.Lcd.drawPixel(oldDisX[showAngle] , oldDisY[showAngle], BLACK);
    if (distance[showAngle] == 250)
      M5.Lcd.drawPixel(disX[showAngle] , disY[showAngle], BLUE);
    else
      M5.Lcd.drawPixel(disX[showAngle] , disY[showAngle], YELLOW);
    oldDisX[showAngle] = disX[showAngle];
    oldDisY[showAngle] = disY[showAngle];


    data[i * 4] = showAngle / 256;
    data[i * 4 + 1] = showAngle % 256;
    data[i * 4 + 2] = distance[showAngle] / 256;
    data[i * 4 + 3] = distance[showAngle] % 256;
  }
  
  esp_now_send(peer_addr, data, 180);
  while (Serial1.available())
  {
    uint16_t r = Serial1.read();
    switch (commandStatus)
    {
      case 0: if (r == 0xAA) commandStatus = 1;
        else commandStatus = 0; break; //Package Header
      case 1: if (r == 0x00) commandStatus = 2; else commandStatus = 0; break; //
      case 2: commandStatus = 3;        break; //
      case 3: ver = r; commandStatus = 4; break;
      case 4: if (r == 0x61)
          commandStatus = 5;
        else
        {
          commandStatus = 0;
        } break;
      case 5: if (r == 0xAD) commandStatus = 6;
        else
        {
          commandStatus = 0;
        } break;
      case 6: commandStatus = 7;  break;
      case 7:  dataLength = (r - 5) / 3; commandStatus = 8; break;
      case 8: lidarSpeed = r;  commandStatus = 9; break;
      case 9: angleOffset = r * 256; commandStatus = 10; break;
      case 10: angleOffset += r; commandStatus = 11;  break;
      case 11: startAngle = r * 256; commandStatus = 12; break;
      case 12: startAngle += r; commandStatus = 13;  break;
      default:
        if (commandStatus == ( 14 + 3 * 44)) //finish.
        {
          packCount++;
          commandStatus = 0;
          return;
        }
        switch ((commandStatus - 13) % 6)
        {
          case 0: signalValue[(startAngle / 2250) * 22 + ((commandStatus - 13) / 6)] = r;  commandStatus++; break;
          case 1: temp = r * 256;  commandStatus++; break;
          case 2: temp += r; distance[(startAngle / 2250) * 22 + ((commandStatus - 13) / 6)] = temp > 2 ? temp : 250;  commandStatus++; break;
          case 3: commandStatus++; break;
          case 4: commandStatus++; break;
          case 5: commandStatus++; break;
        } break;
    }
  }

}
