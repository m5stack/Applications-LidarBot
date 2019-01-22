#include"espnow.h"
#include<M5Stack.h>
#include<WiFi.h>
#include<esp_now.h>
#include<Wire.h>

Espnow::Espnow(void)
{
  
}

void Espnow::BotInit(void){
  
  //m5.begin();
  //Serial.begin(115200);
  
  pinMode(37, INPUT);
  pinMode(38, INPUT);
  pinMode(39, INPUT);
  //digitalWrite(37, HIGH);
 // digitalWrite(38, HIGH);
  //digitalWrite(39, HIGH);

  //WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_AP_STA);//WIFI_AP_STA
  Serial.println("ESPNow Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());
  //sscanf(WiFi.softAPmacAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &sta_addr[0], &sta_addr[1], &sta_addr[2], &sta_addr[3], &sta_addr[4], &sta_addr[5] );
//WiFi.softAPmacAddress()
  //if(!bot)
  sscanf(WiFi.macAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &sta_addr[0], &sta_addr[1], &sta_addr[2], &sta_addr[3], &sta_addr[4], &sta_addr[5] );
   //else
   //sscanf(WiFi.softAPmacAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &sta_addr[0], &sta_addr[1], &sta_addr[2], &sta_addr[3], &sta_addr[4], &sta_addr[5] );
//WiFi.softAPmacAddress()
   //WiFi.softAPmacAddress()
  InitEspNow();
  
  preferences.begin("my-app", false);
  String mac_addr;
  mac_addr = preferences.getString("mac_addr", "error");
  Serial.print("mac_addr = ");Serial.print(mac_addr);
 
  sscanf(mac_addr.c_str(), "%x:%x:%x:%x:%x:%x%c",  &peer_addr[0], &peer_addr[1], &peer_addr[2], &peer_addr[3], &peer_addr[4], &peer_addr[5]);
  
  for (int i = 0; i < 6; ++i ){
    slave.peer_addr[i] = (uint8_t) peer_addr[i];
  }
  slave.channel = CHANNEL;
  esp_now_add_peer(&slave);
}

void Espnow::RemoteInit(void)
{
   //m5.begin();
  //Serial.begin(115200);
  
  pinMode(37, INPUT);
  pinMode(38, INPUT);
  pinMode(39, INPUT);
  //digitalWrite(37, HIGH);
 // digitalWrite(38, HIGH);
  //digitalWrite(39, HIGH);

  WiFi.mode(WIFI_AP_STA);
  //WiFi.mode(WIFI_AP);
  ConfigDeviceAP();
  Serial.println("ESPNow Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print(" AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  //sscanf(WiFi.softAPmacAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &sta_addr[0], &sta_addr[1], &sta_addr[2], &sta_addr[3], &sta_addr[4], &sta_addr[5] );
//WiFi.softAPmacAddress()
  //if(!bot)
  sscanf(WiFi.softAPmacAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &sta_addr[0], &sta_addr[1], &sta_addr[2], &sta_addr[3], &sta_addr[4], &sta_addr[5] );
   //else
   //sscanf(WiFi.softAPmacAddress().c_str(), "%x:%x:%x:%x:%x:%x",  &sta_addr[0], &sta_addr[1], &sta_addr[2], &sta_addr[3], &sta_addr[4], &sta_addr[5] );
//WiFi.softAPmacAddress()
   //WiFi.softAPmacAddress()
  InitEspNow();
  
  preferences.begin("my-app", false);
  String mac_addr;
  mac_addr = preferences.getString("mac_addr", "error");
  Serial.print("mac_addr = ");Serial.print(mac_addr);
 
  sscanf(mac_addr.c_str(), "%x:%x:%x:%x:%x:%x%c",  &peer_addr[0], &peer_addr[1], &peer_addr[2], &peer_addr[3], &peer_addr[4], &peer_addr[5]);
  
  for (int i = 0; i < 6; ++i ){
    slave.peer_addr[i] = (uint8_t) peer_addr[i];
  }
  slave.channel = CHANNEL;
  esp_now_add_peer(&slave);
}

// config AP SSID
void Espnow::ConfigDeviceAP(void)
{
 
  String Mac = WiFi.macAddress();
  String SSID = "Slave:"+ Mac;
  bool result = WiFi.softAP(SSID.c_str(), "12345678", 5, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  } else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// Init ESP Now with fallback
void Espnow::InitEspNow(void)
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

void Espnow::OnRemotSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  
}
int8_t Espnow::OnRemotRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){

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
    if(connect_num> 20) connect_num = 0;

  
  int mac_comp = 0;
  for(mac_comp = 0; mac_comp < 6; mac_comp++){
    if(mac_addr[mac_comp] != peer_addr[mac_comp]){
      return 1;
    }  
  }      
  return 0;
}
   
   void Espnow::OnBotSent(const uint8_t *mac_addr, esp_now_send_status_t status)
   {
    }
int8_t Espnow::OnBotRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  //Serial.print("connectflag = ");Serial.println(connectflag);
  if ((data_len == 7) && (!connectflag))
  {
    connectflag = true;
    int i = 0;
    for (i = 0; i < connect_num; i++)
    {
      int j = 0;
      for (j = 0; j < 6; j++)
      {
        if (connect_addr[i][j] != mac_addr[j])
        {
          break;
        }
      }
      //Serial.printf("j = %d",j);
      if (j == 6)
      {
        break;
      }
    }
    if (i == connect_num)
    {
      for (int i = 0; i < 6; i++)
      {
        connect_addr[connect_num][i] = mac_addr[i];
      }
      connect_num++;
    }
  }
  //if (connect_num > 20) connect_num = 0;

  int mac_comp = 0;
  for(mac_comp = 0; mac_comp < 6; mac_comp++){
    if(mac_addr[mac_comp] != peer_addr[mac_comp]){
      return 1;
    }  
  }  
  return 0; 
}

void Espnow::RemoteConnectUpdate(){

  static int updatetime;
  if (digitalRead(38) == LOW)
  {
    updatetime++;
  }
  //Serial.println(updatetime);
  if (updatetime >= 300){
    updatetime = 0;
    delay(200);
    M5.Speaker.tone(80, 200);
    delay(200);
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Speaker.mute();
    int ypos = 0;
    int old_ypos = 0;
    connect_num = 0;
    connectflag = false;
    while(true)
    {
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
      
        if(ypos < 1){
         //M5.Lcd.fillScreen(TFT_BLACK);
           if(old_ypos != ypos)
           M5.Lcd.fillRect(30, 210, 90, 50, TFT_BLACK);
           M5.Lcd.setCursor(30, 210);  
           M5.Lcd.println("update");
        }
        else
        {
           if(old_ypos != ypos)
           M5.Lcd.fillRect(30, 210, 100, 50, TFT_BLACK);
           //M5.Lcd.fillScreen(TFT_BLACK);
           M5.Lcd.setCursor(50, 210);  
           M5.Lcd.println("up");
         }
         old_ypos = ypos;
         M5.Lcd.setCursor(125, 210);
         M5.Lcd.println("down");
         M5.Lcd.setCursor(220, 210);
         M5.Lcd.println("select");


       //select down
         if ((digitalRead(37) == LOW) && (connect_num))
         {
             M5.Lcd.fillScreen(TFT_BLACK);
             for (int i = 0; i < 6; ++i ){
              peer_addr[i] = (uint8_t) connect_addr[ypos][i];
             }
            //Serial.printf("%2x:%2x:%2x:%2x:%2x:%2x",slave.peer_addr[0],slave.peer_addr[1],slave.peer_addr[2], slave.peer_addr[3],slave.peer_addr[4], slave.peer_addr[5]);
             for (int i = 0; i < 6; ++i ){
               slave.peer_addr[i] = (uint8_t) peer_addr[i];
             }
             slave.channel = CHANNEL;
             esp_now_add_peer(&slave);
             M5.Lcd.setCursor(140, 210);
             M5.Lcd.println("Quit");
             while(!connectflag){
             //for(int i = 0;i < 6; i++){
               esp_err_t result = esp_now_send(slave.peer_addr, sta_addr, sizeof(sta_addr)+ 1);
               Serial.print("Send Status: ");
               if (result == ESP_OK) {
                 Serial.println("Success");
               }else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
               Serial.println("ESPNOW not Init.");
               } else if (result == ESP_ERR_ESPNOW_ARG) {
               Serial.println("Invalid Argument");
               } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
               Serial.println("Internal Error");
               } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
               Serial.println("ESP_ERR_ESPNOW_NO_MEM");
               }else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
               Serial.println("Peer not found.");
               }else {
               Serial.println("Not sure what happened");
              }
              delay(100);
        // }

       
 
       // while(!connectflag)
       // {
          if(digitalRead(38) == LOW) break;
        }
        if(connectflag){
           char macStr[18];
           snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", peer_addr[0], peer_addr[1], peer_addr[2], peer_addr[3], peer_addr[4], peer_addr[5]);
           String mac_addr  = (char *)macStr;
           preferences.putString("mac_addr", mac_addr);
           delay(100);
          M5.Lcd.fillScreen(TFT_BLACK);
           break;
       }  
       }
       
     
         delay(100);
         if (digitalRead(39) == LOW && (connect_num) )
         {
            M5.Lcd.fillScreen(TFT_BLACK);
            ypos--;
            if(ypos <= 0) 
            {
              ypos = 0;
            }
         }
    
        if (digitalRead(38) == LOW && (connect_num))
        {      
         // Serial.println("IdigitalRead(38) == LOW");
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
} 


void Espnow::ScanForSlave(void) {
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
void Espnow::manageSlave(void) {
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
void Espnow::sendData(void) {
  //const uint8_t *phy_addr = peer_addr;
  for (int i = 0; i < SlaveCnt; i++) {
    const uint8_t *scan_addr = slaves[i].peer_addr;
    if (i == 0) { // print only for first slave
      Serial.print("Sending: ");
      //Serial.println(phy_addr);
      //Serial.print(phy_addr);
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) sta_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
    }
    //WiFi.softAPmacAddress()
    //esp_err_t result = esp_now_send(scan_addr, WiFi.softAPmacAddress(), sizeof(WiFi.softAPmacAddress()));
    esp_err_t result = esp_now_send(scan_addr, sta_addr, sizeof(sta_addr));
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


void Espnow::BotConnectUpdate(void)
{
  static int updatetime;
  if (digitalRead(38) == LOW)
  {
    updatetime++;
  }
  //Serial.println(updatetime);
  if (updatetime >= 100){
    updatetime = 0;
    delay(200);
    M5.Speaker.tone(80, 200);
    delay(200);
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Speaker.mute();
    int ypos = 0;
    connect_num = 0;
    connectflag = false;
    while (true){
      M5.Lcd.setCursor(0, 0, 4);
      M5.Lcd.println("Broadcasting...");
      M5.Lcd.printf("(");
      M5.Lcd.print(WiFi.softAPmacAddress());
      M5.Lcd.printf(")");
      if(connect_num>=1) connect_num = 1;
      for (int i = 0; i < connect_num; i++){
        M5.Lcd.setCursor(30, 75 + 25 * i);    // Set cursor near top left corner of screen
        M5.Lcd.printf("%02x:%02x:%02x:%02x:%02x:%02x", connect_addr[i][0], connect_addr[i][1], connect_addr[i][2], connect_addr[i][3], connect_addr[i][4], connect_addr[i][5]);
      }
      if (connect_num) {
        M5.Lcd.setCursor(30, 125);
        M5.Lcd.println("Accept?");
        M5.Lcd.setCursor(220, 210);
        M5.Lcd.println("Confirm");
      }
      if ((digitalRead(37) == LOW) && (connect_num)){
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
        uint8_t ack[4] = {0x41, 0x43, 0x4B};
        for (int i = 0; i < 6; i++)
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
        //preferences.end();
        break;
       }
      if (digitalRead(39) == LOW  && (connect_num))
      {
        M5.Lcd.fillScreen(TFT_BLACK);
        ypos--;
        if (ypos <= 0)
        {
        ypos = 0;
        }
      }

      if (!connect_num)
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
}
