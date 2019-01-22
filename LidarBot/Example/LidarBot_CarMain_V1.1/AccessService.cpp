#include "AccessService.h"
#include <WebServer.h>

WebServer server(80);
WebServer server1(81);
LidarCar serverLidarCar;
extern LidarCar lidarcar;
//uint16_t distance[360];
extern const char* str;
extern const char* accelerate;
extern const char* reduce;

AccessService::AccessService(){
   speed_x = 2;
   speed_y = 2;
   speed_flag = 0;
}

void AccessService::Init(){
  //typedef std::function<void(void)> THandlerFunction;
  Serial.println("Configuring access point...");
  // You can remove the password parameter if you want the AP to be open.
  //WiFi.mode(WIFI_AP);
  WiFi.mode(WIFI_AP_STA);
  String Mac = WiFi.macAddress();
  String SSID = "LidarBot:"+ Mac;
  bool result = WiFi.softAP(SSID.c_str(), "12345678", 0, 0);
  if (!result){
    Serial.println("AP Config failed.");
  } else
  {
    Serial.println("AP Config Success. AP NAME: " + String(SSID));
  }
  //WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/left",  left);
  server.on("/backward", backward);
  server.on("/forward", forward);
  server.on("/right", right);
  server.on("/shift", shift);
  server.on("/stop", stop);
  server.on("/reduce", reduce);
  server.on("/accelerate", accelerate);
  server.on("/rightrear", rightrear);
  server.on("/rightfront", rightfront);
  server.on("/leftrear", leftrear);
  server.on("/leftfront", leftfront);
  server.on("/GetCoordinates", GetCoordinates);
  server.begin();
}

void AccessService::GetCoordinates(){
   Serial.print("forward() = ");
   Serial.println(server.arg(0));
   Serial.println(server.arg(1));
  //LidarCar::ControlWheel(0, 3, 0);
  //serverLidarCar.ControlWheel(0, 2, 0);
  server.send(200, "text/html", "");
}
void AccessService::stop(){
 serverLidarCar.ControlWheel(0, 0, 0);
 server.send(200, "text/html", "");
}
void AccessService::leftfront(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(-2, 4, 0);
 server.send(200, "text/html", "");
}
void AccessService::leftrear(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(-2, -4, 0);
 server.send(200, "text/html", "");
}
void AccessService::rightfront(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(2, 4, 0);
 server.send(200, "text/html", "");
}

void AccessService::rightrear(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(2, -4, 0);
 server.send(200, "text/html", "");
}
void AccessService::accelerate(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(-2, 0, 0);
 server.send(200, "text/html", "");
}
void AccessService::reduce(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(-2, 0, 0);
 server.send(200, "text/html", "");
}
//LidarCar::ControlWheel(int8_t X, int8_t Y, byte A);
void AccessService::left(){
 //Serial.println("left()");
 serverLidarCar.ControlWheel(-2, 0, 1);
 server.send(200, "text/html", "");
}
void AccessService::right(){
  //Serial.println("right");
  serverLidarCar.ControlWheel(2, 0, 1);
  server.send(200, "text/html", "");
}
void AccessService::backward(){
  //Serial.println("backward");
  serverLidarCar.ControlWheel(0, -2, 0);
  server.send(200, "text/html", "");
}
void AccessService::forward(){
   //Serial.print("forward() = ");
   //Serial.println(server.arg(0));
   //Serial.println(server.arg(1));
  //LidarCar::ControlWheel(0, 3, 0);
  serverLidarCar.ControlWheel(0, 2, 0);
  server.send(200, "text/html", "");
}
void AccessService::shift(){
  //Serial.println("shift()");
  //ControlWheel(1, 1, 0);
  //serverLidarCar.ControlWheel(0, 0, 0);
  server.send(200, "text/html", "");
}

void AccessService::drawGraph() {
  String out = "";
  char temp[100];
  int disX,disY;
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"360\" height=\"240\">\n";
  out += "<rect width=\"360\" height=\"240\" fill=\"rgb(0, 0, 0)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  for (int x = 0; x < 360; x++) {
    disX = ( 80 + (lidarcar.distance[x] / 70) * cos(3.14159 * x / 180 + 0.13))*2;
    disY = (100 + (lidarcar.distance[x] / 70) * sin(3.14159 * x / 180 + 0.13))*2;
    #if 1
    if(lidarcar.distance[x] == 250)
    sprintf(temp, "<rect x=\"%d\" y=\"%d\" width=\"1\" height=\"1\" stroke-width=\"1\" stroke=\"blue\" />\n", 360-disY, disX);
    else
    sprintf(temp, "<rect x=\"%d\" y=\"%d\" width=\"1\" height=\"1\" stroke-width=\"1\" stroke=\"yellow\" />\n", 360-disY, disX);
    #else
    sprintf(temp, "<rect x=\"%d\" y=\"%d\" width=\"1\" height=\"1\" stroke-width=\"1\" stroke=\"blue\" />\n", 360-disY, disX);
    #endif
    out += temp;
  }
  out += "</g>\n</svg>\n";
//Serial.printf("-----------------------> out len %d\r\n", out.strlen()));
  server.send(200, "image/svg+xml", out);
}

void AccessService::handleRoot() {
  #if 1
  char temp[5000];
  snprintf(temp, 5000,
           "<html>\
  <head>\
    <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1, minimum-scale=1, user-scalable=no, minimal-ui'>\
    <meta http-equiv='X-UA-Compatible' content='ie=edge'>\
    <style>\
        * { margin: 0; padding: 0; box-sizing: border-box; }\
        html, body { width: 100vw; height: 100vh; overflow: hidden; background-color: black; }\
        \#bg { position: absolute; width: 100vw; z-index: 1; display: block; top: 0; bottom: 0; margin: auto; }\
        .wrap-btn { position: absolute; width: 100vw; height: 100vh; bottom: 0; left: 0; z-index: 2; }\
        .wrap-btn > button { -webkit-user-select: none; outline: none; border: none; width: 80px; height: 80px; border-radius: 80px; font-size: 30px; line-height: 80px; background-color: \#ccc; }\
        \#forward, #backward, #left, #right, #shift, #stop, #leftfront, #leftrear, #rightfront, #rightrear, #accelerate, #reduce { position: absolute; }\
        \#forward { top: 10px; left: 90px; }\
        \#backward { top: 150px; left: 90px; }\
        \#left { top: 80px; left: 10px; }\
        \#right { top: 80px; left: 170px; }\
        \#shift { bottom: 10px; left: 50px; width: 120px; height: 120px; border-radius: 120px; font-size: 32px; transform: rotate(90deg); font-family: Arial; }\
        \#stop {top: 90px; left: 100px; width: 60px; height: 60px;}\
        \#leftfront {top: 18px; left: 20px; width: 60px; height: 60px;transform: rotate(-40deg);}\
        \#leftrear {top: 162px; left: 20px; width: 60px; height: 60px;transform: rotate(-40deg);}\
        \#rightfront {top: 18px; left: 182px; width: 60px; height: 60px;transform: rotate(-40deg);}\
        \#rightrear {top: 162px; left: 182px; width: 60px; height: 60px;transform: rotate(-40deg);}\
        \#accelerate {bottom: 220px; left: 20px; width: 70px; height:70px;transform: rotate(90deg);}\
        \#reduce {bottom: 140px; left: 40px; width: 70px; height: 70px;transform: rotate(90deg);}\
    </style>\
    <title>M5STACK LidarBot</title>\
  </head>\
  <body>\
    <img id=\"bg\" src=\"/test.svg\">\
    <div class=\"wrap-btn\" id=\"wrap-btn\" >\
        <button id=\"forward\" ontouchstart=\"handleTouchstart('/left')\" ontouchend=\"handleTouchend()\">&#8593;</button>\
        <button id=\"left\" ontouchstart=\"handleTouchstart('/backward')\" ontouchend=\"handleTouchend()\">&#8592;</button>\
        <button id=\"right\" ontouchstart=\"handleTouchstart('/forward')\" ontouchend=\"handleTouchend()\">&#8594;</button>\
        <button id=\"backward\" ontouchstart=\"handleTouchstart('/right')\" ontouchend=\"handleTouchend()\">&#8595;</button>\
        <button id=\"shift\" ontouchstart=\"handleTouchstart('/shift')\" ontouchend=\"handleTouchend()\">shift</button>\
        <button id=\"stop\" ontouchstart=\"handleTouchstart('/stop')\" ontouchend=\"handleTouchend()\">&#8226;</button>\
        <button id=\"leftfront\" ontouchstart=\"handleTouchstart('/leftfront')\" ontouchend=\"handleTouchend()\">&#8593;</button>\
        <button id=\"leftrear\" ontouchstart=\"handleTouchstart('/leftrear')\" ontouchend=\"handleTouchend()\">&#8592;</button>\
        <button id=\"rightfront\" ontouchstart=\"handleTouchstart('/rightfront')\" ontouchend=\"handleTouchend()\">&#8594;</button>\
        <button id=\"rightrear\" ontouchstart=\"handleTouchstart('/rightrear')\" ontouchend=\"handleTouchend()\">&#8595;</button>\
        <button id=\"accelerate\" onclick=\"ajax('/shift')\">+</button>\
        <button id=\"reduce\" onclick=\"ajax('/stop')\">-</button>\
    </div>\
  </body>\
  <script>\
    function ajax(url) {\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', url, true);\
        xhr.send();\
    }\
    var ajax_timer = null;\
    function handleTouchstart(url) {\
        ajax_timer = setInterval(function(){\
            ajax(url);\
     8}, 100);\
    }\
    function handleTouchend() {\
        clearInterval(ajax_timer);\
    }\
    function wapperClick(e) {\
         var cx = e.touches[0].clientX;\
         var cy  = e.touches[0].clientY;\
         var url = `/GetCoordinates?x=${cx}&&y=${cy}`;\
              ajax(url);\
    }\
    document.body.addEventListener('click', function (e) {  e.preventDefault();}, { passive: false });\
    document.body.addEventListener('touchmove', function (e) { e.preventDefault(); }, { passive: false });\
    document.getElementById('wrap-btn').addEventListener('touchstart', wapperClick);\
    var bg = document.getElementById('bg');\
    var datetime = '';\
    var tmp_datetime = '';\
    bg.onload = function() {\
        datetime = tmp_datetime;\
    };\
    bg.onerror = function() {\
        bg.src = '/test.svg?t=' + datetime;\
    };\
    var timer = setInterval(function(){\
        tmp_datetime = Date.now();\
        bg.src = '/test.svg?t=' + tmp_datetime;\
    }, 500);\
  </script>\
</html>");
//</html>",str,accelerate,reduce);
 server.send(200, "text/html", temp);
  #else
  char temp[400];
  snprintf(temp, 400,
           "<html>\
  <head>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>LidarBot Map</h1>\
    <img src=\"/test.svg\" /><br />\
    <button>up<button/>\
    <button>up<button/>\
    <button>up<button/>\
  </body>\
</html>");
  server.send(200, "text/html", temp);
  #endif
  
}

void AccessService::Listen(){
  server.handleClient();
  //server1.handleClient();
}

void AccessService::Listen1(){
  server1.handleClient();
  //server1.handleClient();
}
/*
void AccessService::Listen(void){
  //server.handleClient();
   }
}
*/
