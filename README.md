# LidarBot

English | [中文](https://github.com/m5stack/Applications-LidarBot/blob/master/README_zh_CN.md)

*Document Link：https://docs.m5stack.com/#/zh_CN/app/lidarbot*

LidarBot is a very powerful and easy-to-use wheeled robot kit that can be applied to map scanning, motion control research, path planning, artificial intelligence, autopilot and multi-machine collaborative tasks.

## Directory tree

**LidarBot Directory tree**

    ├─Example

    │  ├─LidarBot_CarMain_V1.1 - LidarBot master program

    │  ├─LidarBot_RemoteController_V1.0 - Remote control handle programV1.0

    │  └─LidarBot_RemoteController_V1.2 - Remote control handle programV1.2(Double the accuracy of V1.0)

    │

    ├─Firmware(The bottom MEGA328P firmware can be ignored when used)

    │  ├─LidarBot_Bottom_FW - Car floor wheel control board master MEGA328P program (can be ignored)

    │  └─LidarBot_Remote_FW - MEGA328P program inside the handle (can be ignored)

    │

    ├─Product_Picture - Product Image

    ├─Schematic_Diagram - Schematic diagram

    └─SComAssistant - Host computer (temporary only radar map display function)


### Connect And Pair

If you have no map on the screen of your remote, reconnect it.

1. Press Button B of the lidarBot until the sound raised and the car will get into Broadcast mode with it's mac address on the screen which will send the lidarBot Mac address to the remote.
2. And Press Button B for a few second,you will hear the same sound and the Mac address of lidarBot will show on the screen,use ```down``` button to select and press ```select``` when you choose the right Mac address on the screen.
3. Check out the lidarBot screen whether it receive the message from the remote,press ```confirm``` and you finish the match.


## Program analysis：

### **LidarBot Master program：**

```arduino
/* Main loop */
void loop()
{
  espnow.BotConnectUpdate();// ESPNOW disconnect reconnect / change device reconnection
  lidarcar.MapDisplay();// Show map
  esp_now_send(espnow.peer_addr, lidarcar.mapdata, 180);// ESPNOW sends map data
}
```

* **Single function resolution：**

   * Read the use of radar data

      ```arduino
      #include "lidarcar.h"
      LidarCar lidarcar;

      lidarcar.Init();
      GetData();//Get the radar saved to the array distance[]
      ```

   * Use of the maze

      ```arduino
      lidarcar.CarMaze(); //Maze execution function
      ```

   * Tracking use

      ```arduino
      lidarcar.TrackControl(); //Tracking execution function
      ```

   * Use of ESP_NOW

      https://github.com/m5stack/M5-espnow


### **Handle master program：**

```arduino
/* Main loop */
void loop()
{
  espnow.RemoteConnectUpdate();// ESPNOW disconnect reconnect / change device reconnection
  keyboard.GetValue();//Read joystick data
  esp_now_send(espnow.peer_addr, keyboard.keyData, 3);// ESPNOW sends joystick data to the car master
  MapDisplay();// Show map
  accessport.AnalzyCommand();// Send map data to the host computer
}
```

* **Single function resolution：**

   * JOYSTICK

      ```arduino
      #include "keyboard.h"
      KeyBoard keyboard;

      keyboard.Init();
      GetValue();//Handle readings are saved in adX, adY, and control assignments to array keyData[] and handle RGB lights
      ```

   * Use of communication with the host computer
      ```arduino
      #include "accessport.h"
      AccessPort accessport;

      accessport.AnalzyCommand();// Send map data to PC host computer
      ```

## Development environment installation
   click the link：[https://github.com/m5stack/M5Stack](https://github.com/m5stack/M5Stack)

## Schematic

<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Schematic_Diagram/lidarbot_04.png" width="500" height="500">


## Source code

   [LidarBott](https://github.com/m5stack/Applications-LidarBot/tree/master/LidarBot/Example/LidarBot_CarMain_V1.1)

   [Remote](https://github.com/m5stack/Applications-LidarBot/blob/master/LidarBot/Example/LidarBot_RemoteController_V1.2)

## Host computer

   New plus bit machine debugging features.

   Function 1: Display the LidarBot map in real time.

## Laser car diagram

**Length: 142mm, width: 117mm, height: 120mm**

<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_03.png" width="500" height="500">
<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_01.png" width="500" height="500">
<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_02.png" width="500" height="500">
