#include "lidarcar.h"

LidarCar::LidarCar(){
  //commandType = 0;
  commandStatus = 0;
  ver = 0;
  dataLength = 0;
  lidarSpeed = 0;
  angleOffset = 0;
  startAngle = 0;
  packCount = 0;
  showAngle = 0;
}

void LidarCar::Init(void){
  
  for (int i = 0; i < 16; i++){
    setLedColor(i, 20, 20, 20);
    delay(15);
  }
  for (int i = 0; i < 16; i++)
  {
    setLedColor(i, 0, 0, 0);
    delay(15);
  }
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(50);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(120);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(50);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(120);
  setLedAll(0, 0, 0); delay(100); setLedAll(20, 20, 20); delay(50);
  setLedAll(0, 0, 0);
  //Serial.println("Lidar Init su.....");
}

void LidarCar::setLedColor(byte i, byte r, byte g, byte b){
  Serial2.write(0xAB);
  Serial2.write(i);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}

void LidarCar::setLedAll( byte r, byte g, byte b)
{
  Serial2.write(0xAE);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}



void LidarCar::ControlWheel(int8_t X, int8_t Y, byte A)
{
  if (A == 0x01){
    wheelPowerA = controlMapX[7 + Y][X + 7];
    wheelPowerB = controlMapY[7 + Y][X + 7];
    wheelPowerC = controlMapY[7 + Y][X + 7];
    wheelPowerD = controlMapX[7 + Y][X + 7];
  }else{
    wheelPowerA = controlMap[-Y + 7][X + 7];
    wheelPowerB = controlMap[-Y + 7][14 - X - 7];
    wheelPowerC = controlMap[-Y + 7][X + 7];
    wheelPowerD = controlMap[-Y + 7][14 - X - 7];
  }

  Serial2.write(0xAA);
  Serial2.write(wheelPowerA);
  Serial2.write(wheelPowerB);
  Serial2.write(wheelPowerC);
  Serial2.write(wheelPowerD);
  Serial2.write(0x55);

  if(Y>0){
    if (A == 0x01)
     setLedAll(Y,0,0);
    else
     setLedAll(0,0,Y);
  }else{
   if(A == 0x01)
      setLedAll(-Y,0,0);
   else
      setLedAll(0,0,-Y);
  }
 
} 

void LidarCar::MapDisplay(void){
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.print(dataLength);
  M5.Lcd.print("D/");
  M5.Lcd.print(lidarSpeed);
  M5.Lcd.print("S/");
  M5.Lcd.print(packCount);


  for(int i = 0; i < 45; i++)
  {
    if(showAngle >= 359)
      showAngle = 0;
    else
      showAngle++;
    //Serial.print("showAngle = ");Serial.println(showAngle);
    disX[showAngle] = ( 80 + (distance[showAngle] / 70) * cos(3.14159 * showAngle / 180 + 0.13))*2;
    disY[showAngle] = (100 + (distance[showAngle] / 70) * sin(3.14159 * showAngle / 180 + 0.13))*2;
    M5.Lcd.drawPixel(oldDisX[showAngle] , oldDisY[showAngle], BLACK);
    if(distance[showAngle] == 250)
      M5.Lcd.drawPixel(disX[showAngle] , disY[showAngle], BLUE);
    else
      M5.Lcd.drawPixel(disX[showAngle] , disY[showAngle], YELLOW);
    oldDisX[showAngle] = disX[showAngle];
    oldDisY[showAngle] = disY[showAngle];

    //Serial.print(" distance[showAngle] = ");Serial.print(distance[showAngle]);Serial.print(" disX[showAngle] = ");Serial.print(disX[showAngle]);Serial.print(" disY[showAngle] = ");Serial.println(disY[showAngle]);
    #if 1
    mapdata[i * 4 + 0] = showAngle / 256;
    mapdata[i * 4 + 1] = showAngle % 256;
    mapdata[i * 4 + 2] = distance[showAngle] / 256;
    mapdata[i * 4 + 3] = distance[showAngle] % 256;
    #else
    mapdata[i * 4 + 0] = 233;
    mapdata[i * 4 + 1] = 233;
    mapdata[i * 4 + 2] = 236;
    mapdata[i * 4 + 3] = 236;
    #endif
  }

  GetData();

}

void LidarCar::GetData(void){

  while(Serial1.available()){
    uint16_t r = Serial1.read();
    switch (commandStatus){
      case 0: if (r == 0xAA){
                commandStatus = 1;
              }else{
                commandStatus = 0;
              }
              break;
      case 1: if(r == 0x00) {
               commandStatus = 2; 
               }
              else{
                commandStatus = 0; 
              }
              break; 
       case 2:commandStatus = 3;break;
       case 3: ver = r; commandStatus = 4; break;
       case 4:if(r == 0x61){
                commandStatus = 5;
              }
              else{
                commandStatus = 0;
              }
              break;
        case 5:if(r == 0xAD){
                commandStatus = 6;
               }else{
                commandStatus = 0;
               } 
               break;
        case 6: commandStatus = 7;  break;
        case 7: dataLength = (r - 5) / 3; commandStatus = 8; break;
        case 8: lidarSpeed = r;  commandStatus = 9; break;
        case 9: angleOffset = r * 256; commandStatus = 10; break;
        case 10: angleOffset += r; commandStatus = 11;  break;
        case 11: startAngle = r * 256; commandStatus = 12; break;
        case 12: startAngle += r; commandStatus = 13;  break;
    
    default:
        if (commandStatus == ( 14 + 3 * 44)) //finish.
        {
          packCount++;
          commandStatus = 0;
          return;
        }
        switch ((commandStatus - 13) % 6)
        {
          case 0: signalValue[(startAngle / 2250) * 22 + ((commandStatus - 13) / 6)] = r;  commandStatus++; break;
          case 1: temp = r * 256;  commandStatus++; break;
          case 2: temp += r; distance[(startAngle / 2250) * 22 + ((commandStatus - 13) / 6)] = temp > 2 ? temp : 250;  commandStatus++; break;
          case 3: commandStatus++; break;
          case 4: commandStatus++; break;
          case 5: commandStatus++; break;
        } break;
    }
  }
}
