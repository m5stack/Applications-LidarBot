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

void LidarCar::setFrontLedBar( byte r, byte g, byte b)
{
  Serial2.write(0xAC);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}

void LidarCar::setBackLedBar( byte r, byte g, byte b)
{
  Serial2.write(0xAD);
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

void LidarCar::setServo0Angle(uint8_t angle)// angle: 0 ~ 180
{
  if(angle > 180) angle = 180;
  if(angle < 0) angle = 0;
  Serial2.write(0xAF);
  Serial2.write(angle);
  Serial2.write(0x55);
}

void LidarCar::setServo1Angle(uint8_t angle)// angle: 0 ~ 180
{
  if(angle > 180) angle = 180;
  if(angle < 0) angle = 0;
  Serial2.write(0xB0);
  Serial2.write(angle);
  Serial2.write(0x55);
}

void LidarCar::ControlWheel(int8_t X, int8_t Y, byte A)// X, Y, A: -7 ~ 7
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
    if (showAngle >=  359){
       Cortrol_flag = true;count++;
    }
      
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
	
	
	  if((showAngle >= 180) && (showAngle <= 360)){
      if((distance[showAngle] == 250)||(distance[showAngle] == 0)||(distance[showAngle] >= 10000))
      {
         Dis[showAngle - 180][0] = 0;
         Dis[showAngle - 180][1] = 0;
      }
      else
      {
        Dis[showAngle - 180][0] = disX[showAngle];
        Dis[showAngle - 180][1] = disY[showAngle];
      } 
     }
       
   }

  GetData();

}
void LidarCar::CarMaze(void){
if((Cortrol_flag) && (count >= 10))
  {
   CarCortrol();
   Cortrol_flag = false;
   count = 10;
  }
  if((count >= 10))
    ControlWheel(motor_out,motor_y_out, 0); 
}
void LidarCar::CarCortrol(void)
{
    float left_line = 0,right_line = 0,front_line = 0;
    int buf = 0; 
    
    int count = 0;
    for(int i = 0;i < 55;i++)
    {
       if(Dis[i][0])
       {
        buf += Dis[i][0];
         //Serial.print(" left_line = ");Serial.println(left_line);
        count++;
        //delay(1);
       }
    }
   // Serial.print("left_line  count = "),Serial.println(count);
    if(count == 0)count = 1;
    left_line = (float)buf/count;
 
    buf = 0;
    count = 0;
    for(int i = 55;i < 105;i++)
    {
       if(Dis[i][1])
       {
         buf += Dis[i][1];
         count++;
       }
      
    }//Serial.print("front_line  count = "),Serial.println(count);
    if(count == 0)count = 1;
    front_line = (float)buf/count;

    count = 0;
    buf = 0;
    for(int i = 105;i < 180;i++)
    {
      if(Dis[i][0])
       {
        buf += Dis[i][0];
        count++;
       }
    }//Serial.print("right_line  count = "),Serial.println(count);
    if(count == 0)count = 1;
    right_line = buf /count;

    
    float kp = 0.4;motor_y_out = 7;
    if((right_line <= 1.0) || (left_line <= 1.0))
    {
      kp = 1;
      motor_y_out = 4;
    }
    if((front_line >= 170.0) || (front_line <= 1.0))
    {
      kp = 1;
      motor_y_out = 3;
    }

    
    float error_line = (left_line + right_line)/2.0 - 157.80;
    if((left_line == 0) || (right_line == 0))error_line = last_error_line;
    last_error_line = error_line;
    //if(error_line)
    //common
    int ret = 0;
    ret = MazaCom(error_line ,left_line,right_line,front_line) ;
    if(!ret){
      if(go_flag)
    motor_out =  -kp  * error_line/2;
    else
    motor_out =  kp  * error_line;
    if(motor_out >= 4.0)motor_out = 4.0;
    else if(motor_out <= -4.0)motor_out = -4.0;
    ControlWheel(motor_out,motor_y_out, 0);
    }
}

int LidarCar::MazaCom(float error_line,float left_line,float right_line,float front_line)
{
   //Serial.print(" error_line = ");Serial.print(error_line);Serial.print(" left_line = ");Serial.print(left_line);Serial.print(" right_line = ");Serial.print(right_line);Serial.print(" front_line = ");Serial.println(front_line);


   static int stop_count;
   if(((abs(error_line) <= 4.0) &&  (front_line >= 180.0))  &&(((right_line <= 190)) || (left_line >= 120.0)))
   {
      motor_out = 0;
      motor_y_out = 0;
      stop_count++;
      
      if(stop_count>=50)
      {
        motor_out = 1;
        motor_y_out = 0;
        stop_count = 50;
      }
      setLedAll(20, 0, 0);
      return 1;
   }


   if((stop_count >= 50) && ((abs(error_line) >= 5.0) ||  ((front_line >= 160.0)  ||  (front_line <= 1.0))))
   {
        motor_out = 1;
        motor_y_out = 0;
        return 1;
   }
   setLedAll(0, 0, 0);
   
   
   stop_count = 0;
   return 0;
    
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
