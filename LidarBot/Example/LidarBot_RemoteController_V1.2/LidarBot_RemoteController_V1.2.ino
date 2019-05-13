#include<M5Stack.h>
#include <esp_now.h>
#include "keyboard.h"
#include"espnow.h"
#include "accessport.h"
Espnow espnow;
KeyBoard keyboard;
AccessPort accessport;

static uint16_t distance[360],oldDisX[360], oldDisY[360] ;
extern const unsigned char gImage_logo[];

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){
  if(espnow.OnRemotRecv(mac_addr,data,data_len)){
    return;
  }
  if(data_len ==180){
    int j = 0;
    for (int i = 0; i < 45; i++){
      j = data[4 * i] * 256 + data[4 * i + 1];
      distance[j] = data[4 * i + 2] * 256 + data[4 * i + 3];
    }

    accessport.MapSend(data,180);
    #if 0
     if(!send_flag){
       Serial.write(data,180);
       send_flag = 1;
      }
    #endif
  }
}

void MapDisplay(void){
  uint16_t disX = 0,disY = 0;
  for(int showAngle = 0; showAngle < 360; showAngle++){
    disX = ( 80 + (distance[showAngle] / 70) * cos(3.14159 * showAngle / 180 + 0.13))*2;
    disY = (100 + (distance[showAngle] / 70) * sin(3.14159 * showAngle / 180 + 0.13))*2;
    M5.Lcd.drawPixel(oldDisX[showAngle] , oldDisY[showAngle], BLACK);
    if(distance[showAngle] == 250)
      M5.Lcd.drawPixel(disX, disY, BLUE);
    else
      M5.Lcd.drawPixel(disX, disY, YELLOW);
    oldDisX[showAngle] = disX;
    oldDisY[showAngle] = disY;
  }
}
void setup() {
  m5.begin();

  //!logo
  M5.Lcd.fillScreen(TFT_BLACK);
  m5.lcd.pushImage(0, 0, 320, 240, (uint16_t *)gImage_logo);
  M5.Lcd.setCursor(240, 1, 4);    
  //M5.Lcd.printf("V 0.0.2");
  delay(2000);
  M5.Lcd.fillScreen(TFT_BLACK);

  //!key
  keyboard.Init();

  //!esp
  espnow.RemoteInit();
  esp_now_register_recv_cb(OnDataRecv);

  

}

uint8_t led[5] = {0x03,0x03,0x03,0x03,0x03};
void loop()
{
  espnow.RemoteConnectUpdate();
  keyboard.GetValue();
  esp_now_send(espnow.peer_addr, keyboard.keyData, 3);
  MapDisplay();
  accessport.AnalzyCommand();
  #if 0
  uint8_t buffer[4];
  if(Serial.available() == 3){
   Serial.readBytes(buffer,3);
   if(!strcmp((char *)buffer,"ack")){
    send_flag = 0;
   }
   Serial.flush();
  }else{
    while(Serial.available() > 0){
      Serial.read();
    }
  }
  #endif
  M5.Lcd.setCursor(240, 220, 2);    
  //M5.Lcd.printf("led");
  if(digitalRead(37) == LOW){
   while(digitalRead(37) == LOW);
   //esp_now_send(espnow.peer_addr, led, 4);
  }
}
