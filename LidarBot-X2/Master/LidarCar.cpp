#include "lidarcar.h"

uint8_t rent_pp[29];
Rprtrack rprtrack;


/*!
 * \brief LidarCar::LidarCar
 * \param 
 * - 类初始值
 */
LidarCar::LidarCar(){
  commandStatus = 0;
  ver = 0;
  dataLength = 0;
  lidarSpeed = 0;
  angleOffset = 0;
  startAngle = 0;
  packCount = 0;
  showAngle = 0;
}

/*!
 * \brief LidarCar::Init
 * \param 
 * - 雷达车初始化
 */
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
}


/*!
 * \brief LidarCar::LedShow
 * \param 
 * - 灯光展示
 */
void LidarCar::LedShow(void){

  for (int i = 0; i < 16; i++){
    setLedColor(i, 250, 250, 250);
    delay(10);
  }
  for (int i = 0; i < 16; i++)
  {
    setLedColor(i, 250, 250, 250);
    delay(10);
  }

  for (int i = 0; i < 16; i++){
    setLedColor(15-i, 250, 250, 250);
    delay(10);
  }
  for (int i = 0; i < 16; i++)
  {
    setLedColor(15-i, 0, 0, 0);
    delay(10);
  }


  setLedAll(0, 0, 0); delay(100); setLedAll(255, 255, 255); delay(50);
  setLedAll(0, 0, 0); delay(100); setLedAll(255, 255, 255); delay(120);
  setLedAll(0, 0, 0); delay(100); setLedAll(255, 255, 255); delay(50);
  setLedAll(0, 0, 0); delay(100); setLedAll(255, 255, 255); delay(120);
  setLedAll(0, 0, 0); delay(100); setLedAll(255, 255, 255); delay(50);
  setLedAll(0, 0, 0);

  setLedAll(0, 0, 0); delay(100); setLedAll(255, 255, 255); delay(50);
}

/*!
 * \brief LidarCar::setLedColor
 * \param 
 *      i   第几个灯  范围0-15
 *      r   颜色 r 值
 *      g   颜色 g 值
 *      b   颜色 b 值
 * - 设置灯的颜色
 */
void LidarCar::setLedColor(byte i, byte r, byte g, byte b){
  Serial2.write(0xAB);
  Serial2.write(i);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}
/*!
 * \brief LidarCar::setFrontLedBar
 * \param 
 *      r   颜色 r 值
 *      g   颜色 g 值
 *      b   颜色 b 值
 * - 设置车前灯的颜色
 */
void LidarCar::setFrontLedBar(byte r, byte g, byte b)
{
  Serial2.write(0xAC);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}
/*!
 * \brief LidarCar::setBackLedBar
 * \param 
 *      r   颜色 r 值
 *      g   颜色 g 值
 *      b   颜色 b 值
 * - 设置车后灯的颜色
 */
void LidarCar::setBackLedBar( byte r, byte g, byte b)
{
  Serial2.write(0xAD);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}
/*!
 * \brief LidarCar::setLedAll
 * \param 
 *      r   颜色 r 值
 *      g   颜色 g 值
 *      b   颜色 b 值
 * - 设置所有灯的颜色
 */
void LidarCar::setLedAll( byte r, byte g, byte b)
{
  Serial2.write(0xAE);
  Serial2.write(r);
  Serial2.write(g);
  Serial2.write(b);
  Serial2.write(0x55);
}
/*!
 * \brief LidarCar::setServo0Angle
 * \param 
 *     angle   角度范围 0 - 180 
 * - 设置0号舵机的角度
 */
void LidarCar::setServo0Angle(uint8_t angle)
{
  if(angle > 180) angle = 180;
  if(angle < 0) angle = 0;
  Serial2.write(0xAF);
  Serial2.write(angle);
  Serial2.write(0x55);
}

/*!
 * \brief LidarCar::setServo1Angle
 * \param 
 *     angle   角度范围 0 - 180
 * - 设置1号舵机的角度
 */
void LidarCar::setServo1Angle(uint8_t angle)// angle: 0 ~ 180
{
  if(angle > 180) angle = 180;
  if(angle < 0) angle = 0;
  Serial2.write(0xB0);
  Serial2.write(angle);
  Serial2.write(0x55);
}


/*!
 * \brief LidarCar::ControlWheel
 * \param 
 *      X   范围 -7 - 7
 *      Y   范围 -7 - 7
 *      A   范围 0/1  横向移动标志，为1时车可以向左右横向移动，
 * - 车方向及其速度控制
 */
void LidarCar::ControlWheel(int8_t X, int8_t Y, byte A)// X, Y, A: -7 ~ 7
{  
  //！将参数通过controlMap表格转化为四个电机的参数值
  //! controlMapX 为横向移动 时数据表格
  //！controlMap  为正常移动 时数据表格
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
   
  //通过串口下达指令
  Serial2.write(0xAA);
  Serial2.write(wheelPowerA);
  Serial2.write(wheelPowerB);
  Serial2.write(wheelPowerC);
  Serial2.write(wheelPowerD);
  Serial2.write(0x55);

  //灯光设置，灯光随着车的方向速度 变化而变化
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

/*!
 * \brief LidarCar::MapDisplay
 * \param 
 * - 雷达扫描的地图处理及其显示
 */
void LidarCar::MapDisplay(void){

  //每帧数据长度/雷达旋转速度/数据帧数
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.print(dataLength);
  M5.Lcd.print("D/");
  M5.Lcd.print(lidarSpeed);
  M5.Lcd.print("S/");
  M5.Lcd.print(packCount);

  
  for(int i = 0; i < 45; i++)
  { 
    //Cortrol_flag 模式为迷宫时标志
    //当成功获取到一帧数据，标志置位
    if (showAngle >=  359){
       Cortrol_flag = true;
       count++;
    }
      
    if(showAngle >= 359)
      showAngle = 0;
    else
      showAngle++;
    //Serial.print("showAngle = ");Serial.println(showAngle);
    // distance[showAngle]为雷达扫描的直线距离，将其通过cos和sin分解为x y方向上的距离
    disX[showAngle] = ( 80 + (distance[showAngle] / 70) * cos(3.14159 * showAngle / 180 + 0.13))*2;
    disY[showAngle] = (100 + (distance[showAngle] / 70) * sin(3.14159 * showAngle / 180 + 0.13))*2;
    //！显示屏显示
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
	
	  //迷宫有效数据获取  获取角度为180到360的数据
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

  //得到雷达数据
  GetData();

}

//！摄像头模块
void LidarCar::CarCamera(void){

   Serial.print(" rent_pp = ");
   for(int i = 9; i < 29;i++){
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(rent_pp[i]);
   }
    uint8_t max_data = rent_pp[0];
    uint8_t max_num = 0;
#if 0     
  for(int i = 1; i < 9; i++){
    if(rent_pp[i]>max_data){
      max_data = rent_pp[i];
      max_num = i;
    }
  }
  /*
  Serial.print(" max_data = ");
  Serial.println(max_data);
  Serial.print(" max_num = ");
  Serial.println(max_num);
*/
  
  if(max_data <= 5){
    last_motor_out = motor_out;
  }else{
    motor_out = (max_num % 3 - 1);
  }
  if(max_data > 95){
   ControlWheel(0,-2,0);
  }
  else{
    if(max_data <= 5)
      ControlWheel(last_motor_out,0, 0);
    else
      ControlWheel(motor_out,2, 0);
  } 
#else
  for(int i = 1; i < 9; i++){
    if(rent_pp[i]>max_data){
      max_data = rent_pp[i];
      //max_num = i;
    }
  }
  uint8_t max_data_ = rent_pp[9];
  for(int i = 9; i < 29; i++){
    if(i <= 19){
      if(rent_pp[i]>=max_data_){
        max_data_ = rent_pp[i];
        max_num = i - 9;
      }
    }
    else
    {
      if(rent_pp[i]>max_data_){
        max_data_ = rent_pp[i];
        max_num = i - 9;
      }
    }
  }
  int line = 0;
  if(max_data_ < 50){
    line = last_line;
  }
  else
  {
    line = max_num - 10;
    last_line = line;   
  }
  if(line > 50 || line < -50)  return;
  //M5.Lcd.setCursor(300, 0, 2);
  //M5.Lcd.printf("%2d",line);
  //Serial.print(" line = ");Serial.println(line);
 if((max_data > 40) || ((max_data < 5)&&(!line))){
    if(max_data > 55)
    ControlWheel(0, -2, 0);
    else
    ControlWheel(0, 0, 0);
  }else{
  if(abs(line) < 2)
     ControlWheel(0, 2, 0);
  else if(abs(line) < 5){
    if(line >= 0)
     ControlWheel(2,1, 0);
     else
     ControlWheel(-2, 1, 0);
  }else{
     if(line >= 0)
       ControlWheel(2 ,2, 0);
     else
       ControlWheel(-2 ,2, 0);
     }
  }
  
 // last_line = line;
#endif     
}


//! 循迹模块
void LidarCar::TrackControl(void){
  
  rprtrack.SensorStatus();
  rprtrack.CalTrackDev();
 // Serial.print(" OffsetLine = ");Serial.println(rprtrack.OffsetLine);
  //M5.Lcd.setCursor(300, 0, 2);
  //M5.Lcd.print(rprtrack.OffsetLine);
  if(abs(rprtrack.OffsetLine) == 0)
   ControlWheel(rprtrack.OffsetLine,4, 0);
  else if(abs(rprtrack.OffsetLine) == 1)
   ControlWheel(rprtrack.OffsetLine,2, 0);
  else if(abs(rprtrack.OffsetLine) == 2)
   ControlWheel(rprtrack.OffsetLine,2, 0);
  else 
   ControlWheel(rprtrack.OffsetLine,1, 0);
}


//!迷宫模式
void LidarCar::CarMaze(void){
if((Cortrol_flag) && (count >= 10))
  {
   CarCortrol();
   Cortrol_flag = false;
   count = 10;
  }
  if((count >= 10))
    ControlWheel(motor_out, motor_y_out, 0); 
}

//！迷宫处理
void LidarCar::CarCortrol(void)
{    
    //！判断左 右 前 距离
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
    //！左边距离
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
    //！前边距离
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
    //！右边距离
    right_line = buf /count;

    //！pid计算
    /* pid量化公式为
    *      out = kp * error + kd * (error - last_error)
    *      此处仅使用kp值，kd模式为0， 可以尝试加入kd值，
    *      pid的基础概念： http://blog.sina.cn/dpool/blog/s/blog_80f7b8e90101ikk8.html?md=gd
    */ 

    //！通过左右距离调整kp值
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
    
    //!计算出当前道路的位置偏差
    float error_line = (left_line + right_line)/2.0 - 157.80;
    if((left_line == 0) || (right_line == 0))error_line = last_error_line;
    last_error_line = error_line;
    //if(error_line)
    //common
    int ret = 0;
    //！判断是否为死区
    ret = MazaCom(error_line ,left_line,right_line,front_line) ;
    if(!ret){
      if(go_flag)
    motor_out =  -kp  * error_line/2;
    else
    motor_out =  kp  * error_line;
    //！输出
    if(motor_out >= 4.0)motor_out = 4.0;
    else if(motor_out <= -4.0)motor_out = -4.0;
    ControlWheel(motor_out,motor_y_out, 0);
    }
}


//！死区检测，检测前面无路可走，掉头返回
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


//!雷达车模式控制
void LidarCar::ControlMode(void){

  //!mode flag
  if(digitalRead(37) == LOW){
   contro_mode++;
   if(contro_mode >= 4) contro_mode = 0;
   while(digitalRead(37) == LOW);
  }

  Serial.print("contro_mode = ");Serial.println(contro_mode);

  switch(contro_mode){
    case 0:break;
    default:break;
  }
}

//！雷达数据获取
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
