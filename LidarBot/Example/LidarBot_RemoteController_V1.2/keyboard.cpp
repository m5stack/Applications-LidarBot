#include "keyboard.h"

KeyBoard::KeyBoard(){
  
}

void KeyBoard::Init(){
  Wire.begin();
  for (int i = 0; i < 256; i++)
  {
    Wire.beginTransmission(0x5e);
    Wire.write(i % 4);
    Wire.write(random(256) * (256 - i) / 256);
    Wire.write(random(256) * (256 - i) / 256);
    Wire.write(random(256) * (256 - i) / 256);
    Wire.endTransmission();
    delay(2);
  }
  Led(0, 0, 0, 0);
  Led(1, 0, 0, 0);
  Led(2, 0, 0, 0);
  Led(3, 0, 0, 0);
}


void KeyBoard::Led(int i, int r, int g, int b){
  Wire.beginTransmission(0x5e);
  Wire.write(i);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.endTransmission();
}



void KeyBoard::GetValue(void){
   //Serial.printf("dconsoleX:%d   dconsoleY:%d\n", adconsoleX, adconsoleY);
   Wire.requestFrom(0x5e, 5);  
   if(Wire.available()){
     adYL = Wire.read();
     adYH = Wire.read();
     adXL = Wire.read();
     adXH = Wire.read();
     adX = adXH << 8 |adXL;
     adY = adYH << 8 |adYL;

     if ((oldAdX != adX) || (oldAdY != adY)) {
      oldAdX = adX;
      oldAdY = adY;
     }else
    {
      if(oldKey != digitalRead(39)){
        oldKey = digitalRead(39);
      }
      else return;
    }
    //Serial.printf("dconsoleX:%d   dconsoleY:%d\n", adX, adY);
#if 1
    //Serial.printf("dconsoleX:%d   dconsoleY:%d\n", adX, adY);
    switch ((adX - 162) / 40)
    {
      case -3: consoleX = -7; break;
      case -2: consoleX = -7; break;
      case -1: consoleX = -7; break;
      case 0:  consoleX = -7; break;
      case 1:  consoleX = -6; break;
      case 2:  consoleX = -5; break;
      case 3:  consoleX = -4; break;
      case 4:  consoleX = -3; break;
      case 5:  consoleX = -2; break;
      case 6:  consoleX = -1; break;
      case 7:  consoleX = 0; break;
      case 8:  consoleX = 0; break;
      case 9:  consoleX = 0; break;
      case 10: consoleX = 1; break;
      case 11: consoleX = 2; break;
      case 12: consoleX = 3; break;
      case 13: consoleX = 4; break;
      case 14: consoleX = 5; break;
      case 15: consoleX = 6; break;
      case 16: consoleX = 7; break;
      case 17: consoleX = 7; break;
      case 18: consoleX = 7; break;
      case 19: consoleX = 7; break;
    }
    //adconsoleY  min = 199 maconsoleX = 872 admed = 509 - 522
    switch ((adY - 155) / 40)
    {  
      case -3: consoleY = -7; break;
      case -2: consoleY = -7; break;
      case -1: consoleY = -7; break;
      case 0:  consoleY = -7; break;
      case 1:  consoleY = -6; break;
      case 2:  consoleY = -5; break;
      case 3:  consoleY = -4; break;
      case 4:  consoleY = -3; break;
      case 5:  consoleY = -2; break;
      case 6:  consoleY = -1; break;
      case 7:  consoleY = 0; break;
      case 8:  consoleY = 0; break;
      case 9:  consoleY = 0; break;
      case 10: consoleY = 1; break;
      case 11: consoleY = 2; break;
      case 12: consoleY = 3; break;
      case 13: consoleY = 4; break;
      case 14: consoleY = 5; break;
      case 15: consoleY = 6; break;
      case 16: consoleY = 7; break;
      case 17: consoleY = 7; break;
      case 18: consoleY = 7; break;
      case 19: consoleY = 7; break;
    }
    #else
  switch ((adX - 162) / 40)
    {
      case -3: consoleX = -5; break;
      case -2: consoleX = -5; break;
      case -1: consoleX = -5; break;
      case 0:  consoleX = -5; break;
      case 1:  consoleX = -5; break;
      case 2:  consoleX = -5; break;
      case 3:  consoleX = -4; break;
      case 4:  consoleX = -3; break;
      case 5:  consoleX = -2; break;
      case 6:  consoleX = -1; break;
      case 7:  consoleX = 0; break;
      case 8:  consoleX = 0; break;
      case 9:  consoleX = 0; break;
      case 10: consoleX = 1; break;
      case 11: consoleX = 2; break;
      case 12: consoleX = 3; break;
      case 13: consoleX = 4; break;
      case 14: consoleX = 5; break;
      case 15: consoleX = 5; break;
      case 16: consoleX = 5; break;
      case 17: consoleX = 5; break;
      case 18: consoleX = 5; break;
      case 19: consoleX = 5; break;
    }
    //adconsoleY  min = 199 maconsoleX = 872 admed = 468 - 480
    switch ((adY - 213) / 40)
    {  
      case -3: consoleY = -5; break;
      case -2: consoleY = -5; break;
      case -1: consoleY = -5; break;
      case 0:  consoleY = -5; break;
      case 1:  consoleY = -5; break;
      case 2:  consoleY = -5; break;
      case 3:  consoleY = -4; break;
      case 4:  consoleY = -3; break;
      case 5:  consoleY = -2; break;
      case 6:  consoleY = -1; break;
      case 7:  consoleY = 0; break;
      case 8:  consoleY = 0; break;
      case 9:  consoleY = 0; break;
      case 10: consoleY = 1; break;
      case 11: consoleY = 2; break;
      case 12: consoleY = 3; break;
      case 13: consoleY = 4; break;
      case 14: consoleY = 5; break;
      case 15: consoleY = 5; break;
      case 16: consoleY = 5; break;
      case 17: consoleY = 5; break;
      case 18: consoleY = 5; break;
      case 19: consoleY = 5; break;
    }
    
    #endif
   }else{
      
   }
   //Serial.printf("dconsoleX:%d   dconsoleY:%d\n", consoleX, consoleY);
   LedControl();
}


void KeyBoard::LedControl(void){

  if (consoleX > 0)
    {
      if (digitalRead(37) == HIGH)
        Led(2, consoleX * 8, 0, 0);
      else
        Led(2,  0, 0, consoleX * 8);
      Led(0, 0, 0, 0);
    }
    else
    {
      if (digitalRead(37) == HIGH)
        Led(0, -consoleX * 8, 0, 0);
      else
        Led(0,  0, 0, -consoleX * 8);
      Led(2, 0, 0, 0);
    }
    if (consoleY > 0)
    {
      if (digitalRead(37) == HIGH)
        Led(3, consoleY * 8, 0, 0);
      else
        Led(3,  0, 0, consoleY * 8);
      Led(1, 0, 0, 0);
    }
    else
    {
      if (digitalRead(37) == HIGH)
        Led(1, -consoleY * 8, 0, 0);
      else
        Led(1,  0, 0, -consoleY * 8);
      Led(3, 0, 0, 0);
    }


    if (digitalRead(39) == LOW){   
      keyData[0] = consoleX;
      keyData[1] = consoleY;
      keyData[2] = 1;
    }
    else
    {
      keyData[0] = consoleX;
      keyData[1] = consoleY;
      keyData[2] = 0;
    }
}
