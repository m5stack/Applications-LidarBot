# X2 LidarBot

### Init detail 

```
  setLcd();

  espnow.Init();
  if (!M5.BtnC.isPressed()){
    readAddressFromRom();
  } 
  
  httpServer.init();
  

  espnow.setRecvCallBack(&OnDataRecv);
  // espnow.setSendCallBack(&OnDataSent);
  xTaskCreatePinnedToCore(dis_task, "lidar", 10 * 1024, NULL, 1, NULL, 1); 
```

- lcd of M5Core will be init with the title of "X2 lidarBot".
- reset to the broadcast mode if the button C is pressed at the system initialization.
- register the callback function.
- create the task to display the map data.
- init http server.

### Function List in .ino

- ```setLcd(void)```: print title to the screen.
- ```disPlay(void)```: display the map when the data is ready.
- ```sendMap(void)```: send map data to slave when the data is ready.
- ```dis_task(void *arg)```: check if the data is ready and call the ```disPlay()``` function and ```sendMap``` function.
- ```OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)```: call back when the data was sent.
- ```OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)```: call back when system receive data.
- ```printPeerList(void)``` print the mac address if the peerlist of espnow was update.
- ```readAddressFromRom```: check whether the slave mac address is already have, if not, the system will initialize to broadcast itseft to the slave device.
- ```loop(void)```: code here is use to select mac address of slave and receive lidar package from the serial.