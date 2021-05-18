#ifndef __MAPDATA_H
#define __MAPDATA_H

#include <Arduino.h>

struct SendingData {
	uint16_t mapdata[1500] = {0};
	int datalen = 0;
};

struct DisplayData {
	float mapdata[1500] = {0};
	int count = 0;
};

#endif
