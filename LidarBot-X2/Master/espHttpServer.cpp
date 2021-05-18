#include "espHttpServer.h"
#include "page.h"
#include "mapData.h"
#include "X2driver.h"

DisplayData httpData_save;
extern X2 lidar;

void updateData(void *buf) {
	memcpy(httpData_save.mapdata, buf, sizeof(float) * 720);
}

HttpServer::HttpServer() {
    ;;
}

//!Init
//WiFi will init here with the ap mode.
//SSID: X2Lidar:xx:xx:xx:xx:xx:xx
//password: 12345678
bool HttpServer::init() {
  String Mac = WiFi.macAddress();
  String SSID = "X2Lidar:"+ Mac;
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

  //register the callback function here
  httpd_uri_t _dataProcess = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = &dataProcess,
      .user_ctx = NULL
  };

  httpd_uri_t _mapDisplay = {
      .uri = "/map",
      .method = HTTP_GET,
      .handler = &mapDisplay,
      .user_ctx = NULL
  };

  httpd_config_t http_options = HTTPD_DEFAULT_CONFIG();
  http_options.stack_size = 10 * 1024;
  http_options.task_priority = 1;
  ESP_ERROR_CHECK(httpd_start(&server, &http_options));

  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &_dataProcess));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &_mapDisplay));
  // ESP_ERROR_CHECK(httpd_register_uri_handler(server, &control));
  return ESP_OK;
}

String data;
esp_err_t HttpServer::dataProcess(httpd_req_t *req) {
  float *map_data_stash = (float *)calloc(720, sizeof(float));
  
  xSemaphoreTake( xSemaphore, portMAX_DELAY);
  memcpy(map_data_stash, httpData_save.mapdata, 720*sizeof(float));
  xSemaphoreGive( xSemaphore );

  data = "";
  for (int i = 0; i < 720; i++) {
    String tmp = String(map_data_stash[i]);
    data = data + tmp + ",";
  }
  free(map_data_stash);
  httpd_resp_send(req, data.c_str(), data.length());
  return ESP_OK;
}

esp_err_t HttpServer::mapDisplay(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)mapPage, 6463);
}