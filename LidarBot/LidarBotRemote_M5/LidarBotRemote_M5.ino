//201810261400
#include<Wire.h>
#include<M5Stack.h>
#include <esp_now.h>
#include<WiFi.h>
#include <Preferences.h>
esp_now_peer_info_t slave;

#define CHANNEL 1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0
bool connectflag = false;
uint8_t connect_addr[20][6] = {0};
//const uint8_t peer_addr[6]= {0x84, 0x0D, 0x8E, 0x21, 0x10, 0x1D};
uint8_t peer_addr[6] = {0xB4, 0xE6, 0x2D, 0x8B, 0x7D, 0x42};//B4:E6:2D:8B:95:96  30:AE:A4:04:F8:4D  30:AE:A4:57:F8:B9  //B4:E6:2D:8B:95:96
uint8_t phy_addr[6] = {0};
//const uint8_t peer_addr[6] = {0xB4, 0xE6, 0x2D, 0x8B, 0x79, 0xD5};//B4:E6:2D:8B:95:96  30:AE:A4:04:F8:4D  30:AE:A4:57:F8:B9  //B4:E6:2D:8B:95:96
//B4:E6:2D:8B:79:86          B4:E6:2D:8B:A4:12  B4:E6:2D:8B:7F:F6    B4:E6:2D:8B:75:72
extern const unsigned char gImage_BK[];
extern const unsigned char gImage_w0[];
extern const unsigned char gImage_w1[];
extern const unsigned char gImage_w2[];
extern const unsigned char gImage_w3[];
extern const unsigned char gImage_w4[];
extern const unsigned char gImage_w5[];
extern const unsigned char gImage_w6[];
extern const unsigned char gImage_w7[];
extern const unsigned char gImage_w_0[];
extern const unsigned char gImage_w_1[];
extern const unsigned char gImage_w_2[];
extern const unsigned char gImage_w_3[];
extern const unsigned char gImage_w_4[];
extern const unsigned char gImage_w_5[];
extern const unsigned char gImage_w_6[];
extern const unsigned char gImage_w_7[];
extern const unsigned char gImage_ROLL[];
extern const unsigned char gImage_UNROLL[];
int wheelPowerA = 7, wheelPowerB = 2, wheelPowerC = -3, wheelPowerD = -6;
Preferences preferences;
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
  {0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { -4, -4, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 4, 4},
  { -4, -4, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 4, 4},
  { -4, -4, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 4, 4},
  { -4, -4, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 4, 4},
  { -4, -4, -3, -2, -2, -1, -1, 0, 1, 1, 2, 2, 3, 4, 4},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0, 0}
};
/*
  { -2, -2, -1, 0, 1, 2, 3, 5, 4, 4, 4, 4, 4, 4, 4},
  { -2, -2, -1, 0, 0, 1, 2, 4, 3, 3, 3, 3, 3, 3, 4},
  { -3, -2, -1, -1, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 4},
  { -3, -2, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 3, 3, 4},
  { -3, -2, -2, -1, -1, 0, 1, 2, 2, 2, 2, 2, 3, 3, 4},
  { -3, -2, -2, -2, -1, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4},
  { -3, -2, -2, -2, -2, -1, 0, 1, 1, 1, 2, 2, 3, 3, 4},
  { -5, -4, -2, -2, -2, -1, -1, 0, 1, 1, 2, 2, 2, 4, 5},
  { -4, -4, -2, -2, -2, -1, -1, -1, 0, 1, 2, 2, 2, 2, 3},
  { -4, -4, -2, -2, -2, -1, -1, -1, 0, 0, 1, 2, 2, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 2, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 1, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 2, 3},
  { -4, -4, -4, -4, -4, -4, -4, -4, -2, -1, 0, 0, 1, 2, 2},
  { -4, -4, -4, -4, -4, -4, -5, -4, -3, -2, -1, 0, 1, 2, 2}

  };*/
int controlMapY[15][15] =
{
  {0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {4, 4, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -4, -4} ,
  {4, 4, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -4, -4} ,
  {4, 4, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -4, -4} ,
  {4, 4, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -4, -4} ,
  { 4, 4, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -4, -4

  } ,
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, -4, -4, -4, 0, 0, 0, 0, 0, 0}
};

/*
  {4, 4, 4, 4, 4, 4, 4, 5, 3, 2, 1, 0, -1, -2, -2} ,
  {4, 3, 3, 3, 3, 3, 3, 4, 2, 1, 0, 0, -1, -2, -2} ,
  {4, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, -1, -1, -2, -3},
  {4, 3, 3, 2, 2, 2, 2, 2, 1, 0, -1, -1, -1, -2, -3} ,
  {4, 3, 3, 2, 2, 2, 2, 2, 1, 0, -1, -1, -2, -2, -3} ,
  {4, 3, 3, 2, 2, 2, 1, 1, 0, 0, -1, -2, -2, -2, -3} ,
  {4, 3, 3, 2, 2, 1, 1, 1, 0, -1, -2, -2, -2, -2, -3} ,
  {5, 4, 2, 2, 2, 1, 1, 0, -1, -1, -2, -2, -2, -4, -5} ,
  {3, 2, 2, 2, 2, 1, 0, -1, -1, -1, -2, -2, -2, -4, -4} ,
  {3, 2, 2, 2, 1, 0, 0, -1, -1, -1, -2, -2, -2, -4, -4} ,
  {3, 2, 2, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {3, 2, 1, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {3, 2, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {2, 2, 1, 0, 0, -1, -2, -4, -4, -4, -4, -4, -4, -4, -4} ,
  {2, 1, 0, 0, -1, -2, -3, -5, -4, -4, -4, -4, -4, -4, -4}
  };
*/
// Init ESP Now with fallback
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
  char* SSID = "Slave_3";
  bool result = WiFi.softAP(SSID, "12345678", CHANNEL, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  } else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}
// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  //Serial.print("Last Packet Send Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "S" : "F");
}

int ox = 160, oy = 200;
int showAngle = 0;
uint16_t distance[360];
int disX[360], disY[360];
int oldDisX[360], oldDisY[360];
int connect_num;
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  char macStr[18];
  //static int connect_num;
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) data[ii], HEX);
        if (ii != 5) Serial.print(":");
      }

    if(data_len == 4)
    {
      connectflag = true;
    }
    if(data_len == 6)
    {
        int i = 0;
        for(i = 0; i < connect_num; i++)
        {
           int j = 0;
           for(j = 0; j < 6; j++)
           {    
               if(connect_addr[i][j] != data[j])
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
                connect_addr[connect_num][i] = data[i];
            }
            connect_num++;
        }
    }
    if(connect_num>= 20) connect_num = 0;
   
    
  if(data_len == 180)
  {
    for (int i = 0; i < 45; i++)
    {
      int j = data[4 * i] * 256 + data[4 * i + 1];
      distance[j] = data[4 * i + 2] * 256 + data[4 * i + 3];
   //   Serial.print(j); Serial.print(','); Serial.println(distance[j]);
    }
   // Serial.println("...............");
  }
}


void ShowWheelPower()
{return;
  switch (wheelPowerA)
  {
    case -7: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_7); break;
    case -6: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_6); break;
    case -5: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_5); break;
    case -4: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_4); break;
    case -3: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_3); break;
    case -2: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_2); break;
    case -1: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w_1); break;
    case 0: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w0); break;
    case 1: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w1); break;
    case 2: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w2); break;
    case 3: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w3); break;
    case 4: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w4); break;
    case 5: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w5); break;
    case 6: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w6); break;
    case 7: m5.lcd.pushImage(159, 170, 55, 26, (uint16_t *)gImage_w7); break;
  }
  switch (wheelPowerB)
  {
    case -7: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_7); break;
    case -6: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_6); break;
    case -5: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_5); break;
    case -4: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_4); break;
    case -3: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_3); break;
    case -2: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_2); break;
    case -1: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w_1); break;
    case 0: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w0); break;
    case 1: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w1); break;
    case 2: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w2); break;
    case 3: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w3); break;
    case 4: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w4); break;
    case 5: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w5); break;
    case 6: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w6); break;
    case 7: m5.lcd.pushImage(159, 10, 55, 26, (uint16_t *)gImage_w7); break;
  }
  switch (wheelPowerC)
  {
    case -7: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_7); break;
    case -6: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_6); break;
    case -5: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_5); break;
    case -4: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_4); break;
    case -3: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_3); break;
    case -2: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_2); break;
    case -1: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w_1); break;
    case 0: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w0); break;
    case 1: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w1); break;
    case 2: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w2); break;
    case 3: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w3); break;
    case 4: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w4); break;
    case 5: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w5); break;
    case 6: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w6); break;
    case 7: m5.lcd.pushImage(235, 170, 55, 26, (uint16_t *)gImage_w7); break;
  }
  switch (wheelPowerD)
  {
    case -7: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_7); break;
    case -6: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_6); break;
    case -5: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_5); break;
    case -4: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_4); break;
    case -3: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_3); break;
    case -2: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_2); break;
    case -1: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w_1); break;
    case 0: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w0); break;
    case 1: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w1); break;
    case 2: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w2); break;
    case 3: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w3); break;
    case 4: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w4); break;
    case 5: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w5); break;
    case 6: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w6); break;
    case 7: m5.lcd.pushImage(235, 10, 55, 26, (uint16_t *)gImage_w7); break;
  }
}
void Led(int i, int r, int g, int b)
{
  Wire.beginTransmission(0x88);
  Wire.write(i);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.endTransmission();
}
void BotLedL(int r, int g, int b)
{
  uint8_t data[4];
  data[0] = 0x80 | (r >> 4);
  data[1] = 0x90 | (g >> 4);
  data[2] = 0xa0 | (b >> 4);
  esp_now_send(peer_addr, data, 3);
}
void BotLedR(int r, int g, int b)
{

  uint8_t data[4];
  data[0] = 0xb0 | (r >> 4);
  data[1] = 0xc0 | (g >> 4);
  data[2] = 0xd0 | (b >> 4);
  esp_now_send(peer_addr, data, 3);
}
#define NUMSLAVES 20
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;
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

void ConnectUpdate(){
       int ypos = 0;
       connect_num = 0;
       connectflag = false;
       while(1)
       {
    // for (int ii = 0; ii < 6; ++ii ) {
      //  Serial.print((uint8_t) connect_addr[0][ii], HEX);
       // if (ii != 5) Serial.print(":");
      //}
      // Serial.print("\n");
       //M5.Lcd.setTextDatum(MC_DATUM);
       //M5.Lcd.fillScreen(TFT_BLACK);
       //ypos += M5.Lcd.fontHeight(2);

       M5.Lcd.setCursor(0, 0, 4);
       M5.Lcd.println("LidarBot Scanning....");
       for(int i = 0; i < connect_num; i++)
       {
           M5.Lcd.setCursor(30, 25 + 25 * i);    // Set cursor near top left corner of screen
           M5.Lcd.printf("%02x:%02x:%02x:%02x:%02x:%02x", connect_addr[i][0], connect_addr[i][1], connect_addr[i][2], connect_addr[i][3], connect_addr[i][4], connect_addr[i][5]);
       }
       if(connect_num){
       M5.Lcd.setCursor(270, 25 + ypos * 25);
       M5.Lcd.println("<<");
       }
       M5.Lcd.setCursor(50, 210);  
       M5.Lcd.println("up");
       M5.Lcd.setCursor(125, 210);
       M5.Lcd.println("down");
       M5.Lcd.setCursor(220, 210);
       M5.Lcd.println("select");
       if ((digitalRead(37) == LOW) && (connect_num))
       {
           M5.Lcd.fillScreen(TFT_BLACK);
           for (int i = 0; i < 6; ++i )
          {
            peer_addr[i] = (uint8_t) connect_addr[ypos][i];
          }
  //Serial.printf("%2x:%2x:%2x:%2x:%2x:%2x",slave.peer_addr[0],slave.peer_addr[1],slave.peer_addr[2], slave.peer_addr[3],slave.peer_addr[4], slave.peer_addr[5]);
         for (int i = 0; i < 6; ++i )
         {
           slave.peer_addr[i] = (uint8_t) peer_addr[i];
         }
         slave.channel = CHANNEL;
         esp_now_add_peer(&slave);
         for(int i = 0;i < 6; i++)
         {
   esp_err_t result = esp_now_send(slave.peer_addr, phy_addr, sizeof(phy_addr)+ 1);
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

       M5.Lcd.setCursor(125, 210);
       M5.Lcd.println("Quit");
      
      while(!connectflag)
      {
        if(digitalRead(38) == LOW) break;
      }
      if(connectflag){
         char macStr[18];
         snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", peer_addr[0], peer_addr[1], peer_addr[2], peer_addr[3], peer_addr[4], peer_addr[5]);
         String mac_addr  = (char *)macStr;
         preferences.putString("mac_addr", mac_addr);
         M5.Lcd.fillScreen(TFT_BLACK);
        break;
      }  
      }
       
     
       delay(100);
       if (digitalRead(39) == LOW && (connect_num) )
       {Serial.println("IdigitalRead(39) == LOW");
          M5.Lcd.fillScreen(TFT_BLACK);
          ypos--;
          if(ypos <= 0) 
          {
              ypos = 0;
          }
      }
    
   if (digitalRead(38) == LOW && (connect_num))
   {      
          Serial.println("IdigitalRead(38) == LOW");
         //M5.Lcd.print("39 low"); 
          M5.Lcd.fillScreen(TFT_BLACK);
          ypos++; 
          if(ypos >= connect_num - 1) 
          {
              ypos = connect_num - 1;
          }  
    }
   
       }
  }
  
void setup() {
  // put your setup code here, to run once:
  m5.begin();
  pinMode(5, INPUT);
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);
  pinMode(37, INPUT);
  pinMode(38, INPUT);
  pinMode(39, INPUT);
  digitalWrite(37, HIGH);
  digitalWrite(38, HIGH);
  digitalWrite(39, HIGH);
  //Set device in STA mode to begin with
  preferences.begin("my-app", false);
  WiFi.mode(WIFI_STA);
  //WiFi.mode(WIFI_AP);
  configDeviceAP();
  WiFi.disconnect();
  Serial.println("ESPNow Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());


  m5.lcd.setBrightness(100);
  M5.Lcd.setCursor(0, 0, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  // We can now plot text on screen using the "print" class
  //M5.Lcd.println(WiFi.macAddress());//(WiFi.macAddress());
 // M5.Lcd.printf("%x,%x,%x,%x,%x,%x", peer_addr[0], peer_addr[1], peer_addr[2], peer_addr[3], peer_addr[4], peer_addr[5]);
  sscanf(WiFi.macAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &phy_addr[0], &phy_addr[1], &phy_addr[2], &phy_addr[3], &phy_addr[4], &phy_addr[5] );
  delay(1000);
  m5.lcd.setBrightness(0);
 // m5.lcd.pushImage(0, 0, 320, 240, (uint16_t *)gImage_BK);
  m5.lcd.setBrightness(100);
  
  // Init ESPNow with a fallback logic
  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
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
  delay(300);
  Wire.begin();
  for (int i = 0; i < 256; i++)
  {
    Wire.beginTransmission(0x88);
    Wire.write(i % 4);
    Wire.write(random(256) * (256 - i) / 256);
    Wire.write(random(256) * (256 - i) / 256);
    Wire.write(random(256) * (256 - i) / 256);
    Wire.endTransmission();
    delay(2);
  }
  Led(0, 0, 0, 0);
  Led(1, 0, 0, 0);
  Led(2, 0, 0, 0);
  Led(3, 0, 0, 0);
}
byte trans(byte w)
{
  switch (w)
  {
    case 249: return 0x0f; break;
    case 250: return 0x0e; break;
    case 251: return 0x0d; break;
    case 252: return 0x0c; break;
    case 253: return 0x0b; break;
    case 254: return 0x0a; break;
    case 255: return 0x09; break;
    case 0: return 0x00; break;
    case 1: return 0x01; break;
    case 2: return 0x02; break;
    case 3: return 0x03; break;
    case 4: return 0x04; break;
    case 5: return 0x05; break;
    case 6: return 0x06; break;
    case 7: return 0x07; break;
  }
}
byte i = 0;
uint8_t adX, adY;
int X = 0, Y = 0;
uint8_t oldAdX = -1, oldAdY = -1;
uint8_t oldKey = -1;
int BotColor[8][3] =
{
  {255, 0, 0},
  {0, 255, 0},
  { 0, 0, 255},
  {255, 255, 0},
  {255, 0, 255},
  {0, 255, 255},
  {255, 255, 255},
  {0, 0, 0}
};
int BotLColorIndex = 0, BotRColorIndex = 0;
void loop()
{
  M5.update();
   //M5.Lcd.setCursor(125, 210);
   //M5.Lcd.println("Update");
 if (M5.BtnA.wasReleasefor(800))
 {
     M5.Lcd.fillScreen(TFT_BLACK);
     ConnectUpdate();
     //goto loop; 
 }
//M5.BtnB.wasReleasefor(8000) ;
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
  }

  Wire.requestFrom(0x88, 2);  // request 1 byte from keyboard
  while (Wire.available())
  {
    adX = Wire.read();
    adY = Wire.read();
    if ((oldAdX != adX) || (oldAdY != adY))
    {
      oldAdX = adX;
      oldAdY = adY;
    }

    else
    {
      if (oldKey != digitalRead(39))
      {
        oldKey = digitalRead(39);
      //  if (oldKey == HIGH)
       //   m5.lcd.pushImage(226, 207, 63, 33, (uint16_t *)gImage_UNROLL);
      //  else
       //   m5.lcd.pushImage(226, 207, 63, 33, (uint16_t *)gImage_ROLL);
      }
      else return;
    }
    switch ((adX - 56) / 10)
    {
      case 0: X = -7; break;
      case 1: X = -6; break;
      case 2: X = -5; break;
      case 3: X = -4; break;
      case 4: X = -3; break;
      case 5: X = -2; break;
      case 6: X = -1; break;
      case 7: X = 0; break;
      case 8: X = 0; break;
      case 9: X = 1; break;
      case 10: X = 2; break;
      case 11: X = 3; break;
      case 12: X = 4; break;
      case 13: X = 5; break;
      case 14: X = 6; break;
      case 15: X = 7; break;
      case 16: X = 7; break;
    }
    switch ((adY - 56) / 10)
    {
      case 0: Y = -7; break;
      case 1: Y = -6; break;
      case 2: Y = -5; break;
      case 3: Y = -4; break;
      case 4: Y = -3; break;
      case 5: Y = -2; break;
      case 6: Y = -1; break;
      case 7: Y = 0; break;
      case 8: Y = 0; break;
      case 9: Y = 1; break;
      case 10: Y = 2; break;
      case 11: Y = 3; break;
      case 12: Y = 4; break;
      case 13: Y = 5; break;
      case 14: Y = 6; break;
      case 15: Y = 7; break;
      case 16: Y = 7; break;
    }
    uint8_t data[4];

    if (digitalRead(39) == LOW)
    {   
      data[0] = X;
      data[1] = Y;
      data[2] = 1;
      esp_now_send(peer_addr, data, 3);
    }
    else
    {
      data[0] = X;
      data[1] = Y;
      data[2] = 0;
      esp_now_send(peer_addr, data, 3);
    }
    //Serial.println(ad);
    //Serial.print(X);
    //Serial.print(',');
    //Serial.print(Y);

    // Serial.println(' ');
    ShowWheelPower();
   
    if (X > 0)
    {
      if (digitalRead(37) == HIGH)
        Led(2, X * 8, 0, 0);
      else
        Led(2,  0, 0, X * 8);
      Led(0, 0, 0, 0);
    }
    else
    {
      if (digitalRead(37) == HIGH)
        Led(0, -X * 8, 0, 0);
      else
        Led(0,  0, 0, -X * 8);
      Led(2, 0, 0, 0);
    }
    if (Y > 0)
    {
      if (digitalRead(37) == HIGH)
        Led(3, Y * 8, 0, 0);
      else
        Led(3,  0, 0, Y * 8);
      Led(1, 0, 0, 0);
    }
    else
    {
      if (digitalRead(37) == HIGH)
        Led(1, -Y * 8, 0, 0);
      else
        Led(1,  0, 0, -Y * 8);
      Led(3, 0, 0, 0);
    }

  }
  delay(1);
}
