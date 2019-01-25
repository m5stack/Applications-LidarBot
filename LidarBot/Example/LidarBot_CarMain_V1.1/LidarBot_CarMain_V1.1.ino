#include <M5Stack.h>
#include <esp_now.h>
#include "espnow.h"
#include "lidarcar.h"
#include "rprtrack.h"
#include "iic.h"
#include "AccessService.h"

I2C i2c;
Espnow espnow;
LidarCar lidarcar;
AccessService service;

extern const unsigned char gImage_logo[];

void Service(void * pvParameters) {

    for(;;) {
        service.Listen();
        vTaskDelay(2 / portTICK_RATE_MS); 
    }
    vTaskDelete(NULL);
}

void setup() {
  m5.begin();
  Serial1.begin(230400, SERIAL_8N1, 16, 2);  //Lidar
  Serial2.begin(115200);                     //motor
  
  //!esp
  espnow.BotInit();
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  //!service
  service.Init();
  
  //!logo
  M5.Lcd.fillScreen(TFT_BLACK);
  m5.lcd.pushImage(0, 0, 320, 240, (uint16_t *)gImage_logo);
  delay(2000);
  M5.Lcd.fillScreen(TFT_BLACK);

  M5.Lcd.setCursor(240, 220, 2);    
  M5.Lcd.printf("mode");

  //!Motor
  lidarcar.Init();

  //!Camrea
  i2c.master_start();

  //!Service
  xTaskCreatePinnedToCore(
                    Service,
                    "Service",
                    40960,
                    NULL, 
                    5,        
                    NULL,
                    0); 
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  
}
int flag = 0;
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  if(espnow.OnBotRecv(mac_addr,data,data_len)){
    return;
  }
  
 if((data_len == 3) && (!flag)) {
    lidarcar.ControlWheel(data[0], data[1], data[2]);
 }
  
}

void loop()
{
  espnow.BotConnectUpdate();
  lidarcar.MapDisplay();
  //lidarcar.ControlMode();
  
  if(digitalRead(37) == LOW){
   flag++;
   if(flag >= 4) flag = 0;
   while(digitalRead(37) == LOW);
  }
  
  if(flag == 0){ 
    i2c.master_hangs();
    esp_now_send(espnow.peer_addr, lidarcar.mapdata, 180);
    M5.Lcd.setCursor(240, 0);    
    M5.Lcd.printf("Control");
  }
  
  if(flag == 1) {
    i2c.master_hangs();
    esp_now_send(espnow.peer_addr, lidarcar.mapdata, 180);
    lidarcar.CarMaze();
    M5.Lcd.setCursor(240, 0);    
    M5.Lcd.printf("Maze  ");
 }
                 
  if(flag == 2) {
    i2c.master_recovery();
    lidarcar.CarCamera();
    M5.Lcd.setCursor(240, 0);    
    M5.Lcd.printf("Camera  ");
  }

   if(flag == 3) {
    i2c.master_hangs();
    lidarcar.TrackControl();
    M5.Lcd.setCursor(240, 0);    
    M5.Lcd.printf("Track  ");
  }
                 
}
