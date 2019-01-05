#include <M5Stack.h>
#include <esp_now.h>
#include "espnow.h"
#include "lidarcar.h"
#include "rprtrack.h"
Espnow espnow;
LidarCar lidarcar;
extern const unsigned char gImage_logo[];
void setup() {
  m5.begin();
  Serial1.begin(230400, SERIAL_8N1, 16, 2);  //Lidar
  Serial2.begin(115200);                     //motor
  
  //!esp
  espnow.BotInit();
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  
  //!logo
  M5.Lcd.fillScreen(TFT_BLACK);
  m5.lcd.pushImage(0, 0, 320, 240, (uint16_t *)gImage_logo);
  delay(2000);
  M5.Lcd.fillScreen(TFT_BLACK);

  //!Motor
  lidarcar.Init();
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  if(espnow.OnBotRecv(mac_addr,data,data_len)){
    return;
  }
  
 if(data_len == 3) {
    lidarcar.ControlWheel(data[0], data[1], data[2]);
 }
  
}

void loop()
{
  espnow.BotConnectUpdate();
  lidarcar.MapDisplay();
  lidarcar.CarMaze();
  esp_now_send(espnow.peer_addr, lidarcar.mapdata, 180);
}
