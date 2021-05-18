#include<M5Stack.h>
//#include "sdk/include/angles.h"
#include "doProcess.h"
#include "mapData.h"
#include "espnow.h"
//#include <espnow.h>/
#include "espHttpServer.h"
#include "X2driver.h"
#include "lidarcar.h"
#include "lock.h"
#define TFT_GREY 0x5AEB // New colour

int state = 0;

EspNowMaster espnow;
HttpServer httpServer;
X2 lidar;
LidarCar lidarcar;
Preferences preferences;

void setLcd() {
  M5.Lcd.clear(BLACK);
//  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(3, 10);
  M5.Lcd.println("X2 LidarBot");
  M5.Lcd.drawCircle(160, 120 , 4, RED);
}

void printPeerList() {
  setLcd();
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.printf("slave list:\r\n");
  for(int i = 0; i < espnow.peerlist.count; i++) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           espnow.peerlist.list[i].peer_addr[0], 
           espnow.peerlist.list[i].peer_addr[1],
           espnow.peerlist.list[i].peer_addr[2],
           espnow.peerlist.list[i].peer_addr[3],
           espnow.peerlist.list[i].peer_addr[4],
           espnow.peerlist.list[i].peer_addr[5]);
    M5.Lcd.println(macStr);
    M5.Lcd.println();
  }
}

void readAddressFromRom(){
  esp_now_peer_info_t slave_set;
  uint8_t get_len = sizeof(esp_now_peer_info_t);
  if(preferences.getBytes("mac_addr1", &slave_set, get_len) == get_len)
  {
    Serial.printf("<<<<<<<--->>>>>>> \r\n");
    espnow.confirmPeer(slave_set);
  }
}



void disPlay(void) {
  float map_data_stash[720] = { 0.0 };
  xSemaphoreTake( xSemaphore, portMAX_DELAY);
  memcpy(map_data_stash, lidar.dismap.mapdata, 720*sizeof(float));
  xSemaphoreGive( xSemaphore );

  for (int i = 0; i < 720; i++) {
      float oldAng = from_degrees((i / 2.0));
      float Ang = from_degrees((i / 2.0));

      float oldX = (sin(oldAng) * lidar.oldmap.mapdata[i]/5);
      float oldY = (cos(oldAng) * lidar.oldmap.mapdata[i]/5);
      float X = (sin(Ang) * map_data_stash[i]/5);
      float Y = (cos(Ang) * map_data_stash[i]/5);

      if (lidar.oldmap.mapdata[i] > 0) {
        M5.Lcd.drawPixel(-oldX + 160, oldY + 120, BLACK);  
      }

      if (map_data_stash[i] > 0) {
        M5.Lcd.drawPixel(-X + 160, Y + 120, WHITE);  
      }
      lidar.oldmap.mapdata[i] = map_data_stash[i];
  }
}

void sendMap() {
//   ScanForSlave();

    // If Slave is found, it would be populate in `slave` variable
  // We will check if `slave` is defined and then we proceed further
  if (espnow.slave.channel == CHANNEL) { // check if slave channel is defined
    // `slave` is defined
    // Add slave as peer if it has not been added already
    bool isPaired = espnow.manageSlave();
     if (isPaired) {
      // pair success or already paired
      // Send data to device

        uint8_t head[2] = {0xaa, 0x55};
        espnow.sendData(head, 2);
        for (int i = 0; i < 6; i++) {
          espnow.sendData(lidar.tmpData.mapdata + i * 120, sizeof(uint16_t) * 120);
        }
        memset(lidar.tmpData.mapdata, 0, 1500);
        
    } else {
      // slave pair failed
      Serial.println("Slave pair failed!");
    }
  }
  else {
    // No slave found to process
  }
}

int lastCount = 0;
static void dis_task(void *arg) {
  Serial.printf("uart_task\r\n");
  while(1) {
    if (espnow.isConnected == true) {
      if (lidar.disPlayFlag){
        sendMap();
        disPlay();
        delay(5); 
        lidar.disPlayFlag = 0;
      }
    } else {
      Serial.printf("peer count: %d\r\n", espnow.peerlist.count);
      if (espnow.peerlist.count > lastCount) {
        printPeerList();     
        lastCount = espnow.peerlist.count;
      }
    }
    
    delay(10);
  }

}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  // Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  // Serial.println("");
  static uint8_t data_last[3] = {0xff, 0xff, 0xff};
  bool update = false;

  if((data_len == 3)) {
    for(uint8_t i = 0; i < 3; i++)
    {
      if(data_last[i] != data[i])
      {
        data_last[i] = data[i];
        update = true;
      }
    }
    
    if(update)
    {
      lidarcar.ControlWheel(data[0], data[1], data[2]);
    }
  }
}

int choice = 0;

void setup() {
  Serial.begin(115200);
  M5.begin(true, false, false);
  
  // put your setup code here, to run once:
//  BtnSet();
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  Serial2.begin(115200);

   
  setLcd();
  //Serial.printf("got crc %x\r\n", cover_crc(msg_data, data, 80));

  espnow.Init();
  preferences.begin("lidarBot", false);

  if (!M5.BtnC.isPressed()){
    readAddressFromRom();
  } 
  
  httpServer.init();
  

  espnow.setRecvCallBack(&OnDataRecv);
  // espnow.setSendCallBack(&OnDataSent);
  xTaskCreatePinnedToCore(dis_task, "lidar", 10 * 1024, NULL, 1, NULL, 1); 
}

void loop() {
  M5.update();
  espnow.Broadcast();

  if (M5.BtnB.wasPressed()) {
		espnow.confirmPeer(espnow.peerlist.list[choice]);	
    preferences.putBytes("mac_addr1", &espnow.peerlist.list[choice], sizeof(esp_now_peer_info_t));
    setLcd();
    delay(100);
	}

  if (!espnow.isConnected) {
    if(M5.BtnA.wasPressed()) {
		choice--;
		if (choice < 0) {
			choice = 0;
		}
		//setLcd();
		printPeerList();
		M5.Lcd.fillCircle(220, choice*15 + 80, 3, RED);
	} else if(M5.BtnC.wasPressed()) {
		  choice++;
		  if (choice > espnow.peerlist.count - 1) {
			  choice = espnow.peerlist.count - 1;
		  }
		  //setLcd();
		  printPeerList();
		  M5.Lcd.fillCircle(220, choice*15 + 80, 3, RED);
    }
  }
	

  while (Serial1.available()) {
    lidar.lidar_data_deal(Serial1.read());
  }
  delay(10);
}
