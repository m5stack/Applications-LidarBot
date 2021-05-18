#include "RprTrack.h"
#include <Wire.h>


Rprtrack::Rprtrack(){
  for(int i = 0;i < 4;i++)
    sensorValue[i] = 0;
  Wire.begin();
}

void Rprtrack::SensorStatus(void){ 
  uint8_t level;
  Wire.beginTransmission(0x5a);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x5a,1); 
  while(Wire.available()){
      level = Wire.read();
  }

  sensorValue[0] = (level&0x08)>>3;
  sensorValue[1] = (level&0x04)>>2;
  sensorValue[2] = (level&0x02)>>1;
  sensorValue[3] = (level&0x01)>>0;

  //Serial.print(" OffsetLine 0 = ");Serial.println(sensorValue[0]);
 // Serial.print(" OffsetLine 1 = ");Serial.println(sensorValue[1]);
  //Serial.print(" OffsetLine 2 = ");Serial.println(sensorValue[2]);
  //Serial.print(" OffsetLine 3 = ");Serial.println(sensorValue[3]);
}


void Rprtrack::CalTrackDev(void)
{
  //OffsetLine;
  //LastOffsetLine;
  if((sensorValue[0])&&(!sensorValue[1])&&(!sensorValue[2])&&(sensorValue[3]))
    OffsetLine = 0;
  else if((sensorValue[0])&&(!sensorValue[1])&&(sensorValue[2])&&(sensorValue[3]))
    OffsetLine = 1;
  else if((!sensorValue[0])&&(sensorValue[1])&&(sensorValue[2])&&(sensorValue[3]))
    OffsetLine = 2;
  //else if((sensorValue[0])&&(sensorValue[1])&&(sensorValue[2])&&(sensorValue[3]))
    //OffsetLine = 3;
  else if((sensorValue[0])&&(sensorValue[1])&&(!sensorValue[2])&&(sensorValue[3]))
    OffsetLine = -1;
  else if((sensorValue[0])&&(sensorValue[1])&&(sensorValue[2])&&(!sensorValue[3]))
    OffsetLine = -2;
  //else if((sensorValue[0])&&(sensorValue[1])&&(sensorValue[2])&&(sensorValue[3]))
    //OffsetLine = -3;
  else 
  {
    if(LastOffsetLine == -2)
    OffsetLine = LastOffsetLine - 1;
    else if(LastOffsetLine == 2)
    OffsetLine = LastOffsetLine + 1;
    else
    OffsetLine = LastOffsetLine;
  } 
    //if(OffsetLine == 0)
    LastOffsetLine = OffsetLine;   
}
