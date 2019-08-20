# LidarBot

中文 | [English](https://github.com/m5stack/Applications-LidarBot/blob/master/README.md)

*文档地址：https://docs.m5stack.com/#/zh_CN/app/lidarbot*

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


### 连接与配对

当手柄不能控制或者显示地图的时候，需要将雷达车和手柄重新配对一下。

1. 雷达车启动之后按住Button B键（中间的按键）不放，直到雷达车声音响起，雷达车进入广播模式，主机的地址回显示到屏幕上。
2. 手柄启动之后按住Button B(中间的按键)不放，直到手柄声音响起，此时应该接到来自雷达车的广播消息，并且会显示雷达车的Mac地址。如果有多个Mac地址可以通过```down```键向下选择，如果错过了可以按```update```键刷新当前操作。选择与雷达车对应的Mac地址后按下```select```即可确认需要配对的雷达车地址。
3. 这时候雷达车，会收到手柄的确认信息，手柄的Mac地址并且会显示到雷达车的屏幕上，按下屏幕的```confirm```即可完成配对。


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

   * 迷宫的使用

      ```arduino
      lidarcar.CarMaze(); //迷宫执行函数
      ```

   * 循迹的使用

      ```arduino
      lidarcar.TrackControl(); //循迹执行函数
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

## 开发环境安装
   点击链接：[https://github.com/m5stack/M5Stack](https://github.com/m5stack/M5Stack)

## 原理图

<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Schematic_Diagram/lidarbot_04.png" width="500" height="500">


## 源码

   雷达车：[Bot](https://github.com/m5stack/Applications-LidarBot/tree/master/LidarBot/Example/LidarBot_CarMain_V1.1)

   遥控器：[Remote](https://github.com/m5stack/Applications-LidarBot/blob/master/LidarBot/Example/LidarBot_RemoteController_V1.2)

## 上位机
   新加上位机调试功能。

   功能一：实时显示LidarBot地图。

## 激光车示图

**长：142mm， 宽：117mm， 高：120mm**

<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_03.png" width="500" height="500">
<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_01.png" width="500" height="500">
<img src="https://raw.githubusercontent.com/m5stack/Applications-LidarBot/master/LidarBot/Product_Picture/lidarbot_02.png" width="500" height="500">
