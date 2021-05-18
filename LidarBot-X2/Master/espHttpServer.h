#ifndef __ESPHTTPSERVER_H
#define __ESPHTTPSERVER_H

#include "esp_http_server.h"
#include <WiFi.h>
#include "lock.h"
#include "mapData.h"

class HttpServer {
public:
    httpd_handle_t server;

    HttpServer();
    bool init();
    static esp_err_t dataProcess(httpd_req_t *req);
    static esp_err_t mapDisplay(httpd_req_t *req);  
};

#endif