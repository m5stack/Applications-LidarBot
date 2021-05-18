/**
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and a Slave ESP32
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/

#include <M5Stack.h>
//#include <esp_now.h>
#include <WiFi.h>
#include "keyboard.h"
#include <Preferences.h>
#include "espnow.h"

#define CHANNEL 1

uint8_t databuff[1500] = {0};
uint8_t olddatabuff[1500] = {0};
uint8_t disdatabuff[1500] = {0};
int count = 0;
int disPlayFlag = 0;
int scale = 5;
int last_scale = 5;

uint8_t master_mac[] = {0x30,0xae,0xa4,0x58,0x79,0x64};

esp_now_peer_info_t slave;
KeyBoard keyBoard;
Preferences preferences;

EspNowSlave espnow;

float from_degrees(float deg) {
  return deg * 3.14 /180;
}

void disPlay(void) {
//  Serial.printf("display \r\n");
  for (int i = 0; i < 1440; i+=2) {
      float oldAng = from_degrees((i/4.00));
      float Ang = from_degrees((i/4.00));

      uint16_t olddistance = olddatabuff[i] | (olddatabuff[i + 1] << 8);
      uint16_t distance = disdatabuff[i] | (disdatabuff[i + 1] << 8); 
      float oldX = (sin(oldAng) * olddistance / 10);
      float oldY = (cos(oldAng) * olddistance / 10);
      float X = (sin(Ang) * distance / 10);
      float Y = (cos(Ang) * distance / 10);

      if (olddistance > 0) {
        M5.Lcd.drawPixel(-oldX + 160, oldY + 120, BLACK);  
      }

      if (distance > 0) {
        M5.Lcd.drawPixel(-X + 160, Y + 120, WHITE);  
      }
    
	  olddatabuff[i] = disdatabuff[i];
      olddatabuff[i + 1] = disdatabuff[i + 1];
      last_scale = scale;

      M5.update();
      if(M5.BtnB.wasPressed()) {
        setLcd();
        scale++;
        if (scale > 10) {
          scale = 10;
        }
      } else if(M5.BtnC.wasPressed()) {
        setLcd();
        scale--;
        if (scale < 1) {
          scale = 1;
        }
      }
  }
}

void setLcd() {
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(3, 10);
  M5.Lcd.println("X2 Lidarbot Remote");
  M5.Lcd.drawCircle(160, 120 , 4, RED);

}

int lastCount = 0;
static void dis_task(void *arg) {
  Serial.println("dis_task");
  while(1) {
	if(espnow.isConnected == true) {
		if(disPlayFlag == 1) {
			disPlay();  
			memset(disdatabuff, 0, 1500);
			disPlayFlag = 0;
		}
	}
	else {
		if (espnow.peerlist.count > lastCount) {
			printPeerList();
			lastCount = espnow.peerlist.count;
		}
	}
    delay(20);
  }
}

void setup() {
  M5.begin();
  M5.Speaker.begin();
  
  setLcd();
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  
  WiFi.onEvent(WiFiEvent);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.mode(WIFI_AP_STA);
  // configure device AP mode 
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());  
  // Init ESPNow with a fallback logic
	
  espnow.Init();
  if (!M5.BtnC.isPressed()) {
	  readAddressFromRom(); 
  }
  keyBoard.Init();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
  xTaskCreatePinnedToCore(dis_task, "lidar", 10 * 1024, NULL, 1, NULL, 1);
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
     case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Connected to access point");
            break;
  }
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  String addr = (char *)macStr;
  preferences.putString("mac_addr", addr);
  delay(1);

  //Serial.println();
  //Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  float a = 0;
  if (count > 1499) {
    count = 0;  
  }

  memcpy(databuff + count, data, data_len);
  count += data_len;

  if (data_len == 2 && data[0] == 0xaa && data[1] == 0x55) {
//    Serial.println("map recv");
    if (disPlayFlag == 0) {
//      Serial.println("map dis");
      memcpy(disdatabuff, databuff, count); 
      disPlayFlag = 1;  
    }
    memset(databuff, 0, count);
    count = 0;  
  }
}

void readAddressFromRom(){
  preferences.begin("lidarBot", false);
  String mac_addr;
  mac_addr = preferences.getString("mac_addr", "error");
  Serial.print("Slavemac_addr = "); Serial.print(mac_addr);
  if (mac_addr.equals("error")) {
    Serial.println("mac_addr is empty");
  } else {
    esp_now_peer_info_t slave_set;
	memset(&slave_set, 0, sizeof(slave_set));
    
	sscanf(mac_addr.c_str(), "%x:%x:%x:%x:%x:%x",  
                                                    &slave_set.peer_addr[0], 
                                                    &slave_set.peer_addr[1], 
                                                    &slave_set.peer_addr[2], 
                                                    &slave_set.peer_addr[3], 
                                                    &slave_set.peer_addr[4], 
                                                    &slave_set.peer_addr[5]);
	
	for(int i = 0; i < 6; i++){
		Serial.println(slave_set.peer_addr[i]);
	} 
	espnow.Ack(slave_set);
  }
}

void printPeerList() {
  setLcd();
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.printf("who is broadcasting :\r\n");
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

int choice = 0;
void loop() {
  // Chill
	M5.update();
	if (M5.BtnB.wasPressed()) {
		espnow.Ack(espnow.peerlist.list[choice]);
		char macStr[18];
		snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", 
                                      espnow.peerlist.list[choice].peer_addr[0], 
                                      espnow.peerlist.list[choice].peer_addr[1], 
                                      espnow.peerlist.list[choice].peer_addr[2], 
                                      espnow.peerlist.list[choice].peer_addr[3], 
                                      espnow.peerlist.list[choice].peer_addr[4], 
                                      espnow.peerlist.list[choice].peer_addr[5]);					
		String mac_addr  = (char *)macStr;
		preferences.putString("mac_addr", mac_addr);	
		setLcd();
	}

	//Serial.printf("peer count: %d\r\n", espnow.peerlist.count);
  if (!espnow.isConnected){
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
	
  keyBoard.GetValue();
  // Serial.printf("keyboard:%d,%d,%d,%d,%d\r\n", keyBoard.keyData[0], keyBoard.keyData[1], keyBoard.keyData[2], keyBoard.keyData[3], keyBoard.keyData[4]);
  //esp_now_send(slave.peer_addr, keyBoard.keyData, 3);
  if (espnow.isConnected) {
	espnow.sendData(keyBoard.keyData, 3);
  }
  delay(20);
}
