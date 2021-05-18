#ifndef __X2DRIVER_H
#define __X2DRIVER_H

#include "Arduino.h"
#include "mapData.h"
#include "lock.h"

typedef struct {
  uint8_t buf[1024];
  uint16_t length;
} _lidar_data_t;

class X2 {
public:
	uint8_t state = 0;
	uint8_t last_value = 0x00;
	uint8_t rx_buf[1024] = {0};
	uint8_t rx_ptr;
	float startAngle = 0;
	float endAngle = 0;
	int disPlayFlag = 0;
	DisplayData dismap;
	DisplayData tmpmap;
	DisplayData oldmap;
	SendingData tmpData;
	SendingData sendingData;
	_lidar_data_t lidar_data;

	X2();
	uint16_t cover_crc(uint8_t *msg, uint8_t *data, uint16_t data_len) ;
	void lidar_data_deal(uint8_t data_in) ;
	void bufCopy(uint8_t *buff, uint8_t len) ;

};
#endif
