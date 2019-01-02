# LidarBot
   LidarBot是能够应用于地图扫描、运动控制研究、路径规划、人工智能、自动驾驶以及多机协同任务的非常强大，好用的轮式机器人套件。

## 目录树

**LidarBot目录树**

    ├─Example

    │  ├─LidarBot_CarMain_V1.1 - 雷达车主控程序

    │  ├─LidarBot_RemoteController_V1.0 - 遥控手柄程序V1.0

    │  └─LidarBot_RemoteController_V1.2 - 遥控手柄程序V1.2(相比V1.0精度提高一倍)

    │

    ├─Firmware(底层MEGA328P固件，使用时可忽略)

    │  ├─LidarBot_Bottom_FW - 车底板轮子控制板主控MEGA328P程序(可忽略)

    │  └─LidarBot_Remote_FW - 手柄内部MEGA328P程序(可忽略)

    │

    ├─Product_Picture - 产品图片

    ├─Schematic_Diagram - 原理图示意

    └─SComAssistant - 上位机(暂时只有雷达地图显示功能)


## 程序解析：

### **雷达车主控程序：**

```arduino
/* 主循环 */
void loop()
{
  espnow.BotConnectUpdate();// ESPNOW断开重连/换设备重连
  lidarcar.MapDisplay();// 显示地图
  esp_now_send(espnow.peer_addr, lidarcar.mapdata, 180);// ESPNOW发送地图数据
}
```

* **单个功能解析：**

   * 读取雷达数据的使用

      ```arduino
      #include "lidarcar.h"
      LidarCar lidarcar;

      lidarcar.Init();
      GetData();//得到雷达保存到数组distance[]
      ```

   * 循迹的使用

      ```arduino
      #include "rprtrack.h"
      Rprtrack rprtrack;

      SensorStatus();// 巡线数值保存到数组sensorValue[]
      CalTrackDev();// 处理数组sensorValue[]，得出小车偏移情况，保存至变量OffsetLine
      ```

   * ESP_NOW的使用

      https://github.com/m5stack/M5-espnow


### **手柄主控程序：**

```arduino
/* 主循环 */
void loop()
{
  espnow.RemoteConnectUpdate();// ESPNOW断开重连/换设备重连
  keyboard.GetValue();//读取摇杆数据
  esp_now_send(espnow.peer_addr, keyboard.keyData, 3);// ESPNOW发送摇杆数据给小车主控
  MapDisplay();// 显示地图
  accessport.AnalzyCommand();// 发送地图数据给上位机
}
```

* **单个功能解析：**

   * JOYSTICK的使用

      ```arduino
      #include "keyboard.h"
      KeyBoard keyboard;

      keyboard.Init();
      GetValue();//手柄读数保存在adX, adY，并控制赋值给数组keyData[]和手柄RGB灯
      ```

   * 与上位机通信的使用

      ```arduino
      #include "accessport.h"
      AccessPort accessport;

      accessport.AnalzyCommand();// 发送地图数据给PC上位机
      ```

## 1、开发环境安装
   点击链接：[https://github.com/m5stack/M5Stack](https://github.com/m5stack/M5Stack)

## 2、原理图

![image](./LidarBot//Schematic_Diagram/576571642811095946.jpg)


## 3、源码

   雷达车：[Bot](https://github.com/m5stack/Applications-LidarBot/tree/master/LidarBot/Example/LidarBot_CarMain_V1.1)

   遥控器：[Remote](https://github.com/m5stack/Applications-LidarBot/blob/master/LidarBot/Example/LidarBot_RemoteController_V1.2)

## 4、上位机
   新加上位机调试功能。

   功能一：实时显示LidarBot地图。

## 5、激光车示图

**长：142mm， 宽：117mm， 高：120mm**

<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_03.png" width="500" height="500">
<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_01.png" width="500" height="500">
<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_02.png" width="500" height="500">
