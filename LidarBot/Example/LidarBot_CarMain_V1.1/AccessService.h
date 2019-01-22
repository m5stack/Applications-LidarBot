#ifndef __ACCESSSERVICE_H__
#define __ACCESSSERVICE_H__

#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "lidarcar.h"


class AccessService{
   public:
     AccessService();
     void Init(void);
     void Listen(void);
     void Listen1(void);
     //static void handleRoot();
     static void drawGraph();
   private:
     static void GetCoordinates();
     static void handleRoot();
     static void stop();
     static void leftfront();
     static void leftrear();
     static void rightfront();
     static void rightrear();
     static void accelerate();
     static void reduce();
     static void left();
     static void right();
     static void backward();
     static void forward();
     static void shift();
   public:
     //stuin *distance; 
   private:
     int speed_x;
     int speed_y;
     int speed_flag;
    
   
};


#endif
