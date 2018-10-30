#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            11
#define NUMPIXELS      16
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )

#define EnableMotor  Clr_Bit(PORTB,0)
#define DisableMotor  Set_Bit(PORTB,0)
#define MotorYP1     Set_Bit(PORTD,2)
#define MotorYP0     Clr_Bit(PORTD,2)
#define MotorYD1     Set_Bit(PORTD,5)
#define MotorYD0     Clr_Bit(PORTD,5)

#define MotorXP1     Set_Bit(PORTD,3)
#define MotorXP0     Clr_Bit(PORTD,3)
#define MotorXD0     Set_Bit(PORTD,6)
#define MotorXD1     Clr_Bit(PORTD,6)

#define MotorAP1     Set_Bit(PORTD,4)
#define MotorAP0     Clr_Bit(PORTD,4)
#define MotorAD1     Set_Bit(PORTD,7)
#define MotorAD0     Clr_Bit(PORTD,7)

#define MotorZP1     Set_Bit(PORTB,4)
#define MotorZP0     Clr_Bit(PORTB,4)
#define MotorZD0     Set_Bit(PORTB,5)
#define MotorZD1     Clr_Bit(PORTB,5)

#include <Servo.h>

Servo servoA0,servoA1; 

#define ServoA0_1   Set_Bit(PORTC,0)
#define ServoA0_0   Clr_Bit(PORTC,0)
#define ServoA1_1   Set_Bit(PORTC,1)
#define ServoA1_0   Clr_Bit(PORTC,1)

void setup()
{
  DDRB = 0xff;
  PORTB = 0x00;
  DDRD = 0xff;
  PORTD = 0;
  DDRC = 0xff;
  PORTC = 0x00;
  servoA0.attach(A0);  
  servoA1.attach(A1);
  DisableMotor;
  Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  for (int i = 0; i < 200; i++)
  {
    for (int j = 0; j < 16; j++)
      pixels.setPixelColor(j, pixels.Color(i, i, i));
    pixels.show();
    delay(1);
  }
  for (int i = 200; i > 0; i--)
  {
    for (int j = 0; j < 16; j++)
      pixels.setPixelColor(j, pixels.Color(i, i, i));
    pixels.show();
    delay(1);
  }
  for (int j = 0; j < 10; j++)
    pixels.setPixelColor(j, pixels.Color(0, 0, 0));
  pixels.show();
}
#define MidSpeed 140
int SpeedMap[16] = {140, 171, 198, 222, 242, 259, 272, 280, 140, 109, 82, 58, 38, 21, 8, 0};
int TargetSpeedX = MidSpeed, TargetSpeedY = MidSpeed, TargetSpeedZ = MidSpeed, TargetSpeedA = MidSpeed;
int SpeedX = MidSpeed, SpeedY = MidSpeed, SpeedZ = MidSpeed, SpeedA = MidSpeed;
int IdleX = 0, IdleY = 0, IdleZ = 0, IdleA = 0;
int NowTimeX = 0, NowTimeY = 0, NowTimeZ = 0, NowTimeA = 0;
int LColorR = 0, LColorG = 0, LColorB = 0;
int RColorR = 0, RColorG = 0, RColorB = 0;

void Led()
{
  for (int j = 0; j < 8; j++)
    pixels.setPixelColor(j, pixels.Color(LColorR, LColorG, LColorB));
  for (int j = 8; j < 16; j++)
    pixels.setPixelColor(j, pixels.Color(RColorR, RColorG, RColorB));
  pixels.show();
}
int commandStatus = 0;
byte trans(byte i)
{
  switch (i)
  {
    case 0: case 1: case 2: case 3:
    case 4: case 5: case 6: case 7:
      return i; break;
    case 255: return 9; break;
    case 254: return 10; break;
    case 253: return 11; break;
    case 252: return 12; break;
    case 251: return 13; break;
    case 250: return 14; break;
    case 249: return 15; break;
    default: return 0; break;
  }
}
int8_t temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
void serialEvent()
{
  while (Serial.available())
  {
    byte r = Serial.read();
    switch (commandStatus)
    {
      case 0: switch (r)
        {
          case 0xAA: commandStatus = 1; break; //motor control
          case 0xAB: commandStatus = 10; break; //LED single control
          case 0xAC: commandStatus = 20; break; //LED Front Bar control
          case 0xAD: commandStatus = 30; break; //LED Back Bar control
          case 0xAE: commandStatus = 40; break; //LED all control
          case 0xAF: commandStatus = 50; break; //Servo A0
          case 0xB0: commandStatus = 60; break; //Servo A1
          default: commandStatus = 0; break;
        } break;
      case 1: temp1 = r; commandStatus = 2; break;
      case 2: temp2 = r; commandStatus = 3; break;
      case 3: temp3 = r; commandStatus = 4; break;
      case 4: temp4 = r; commandStatus = 5; break;
      case 5: if (r == 0x55)
        {
          TargetSpeedX =  SpeedMap[trans(temp1)];
          TargetSpeedY =  SpeedMap[trans(temp2)];
          TargetSpeedZ =  SpeedMap[trans(temp3)];
          TargetSpeedA =  SpeedMap[trans(temp4)];
        } commandStatus = 0; break;
      //Led single control
      case 10: temp1 = r; commandStatus = 11; break;
      case 11: temp2 = r; commandStatus = 12; break;
      case 12: temp3 = r; commandStatus = 13; break;
      case 13: temp4 = r; commandStatus = 14; break;
      case 14: if (r == 0x55)
        {
          pixels.setPixelColor(15 - temp1, pixels.Color(temp2, temp3, temp4));
          pixels.show();
        } commandStatus = 0; break;

      case 20: temp1 = r; commandStatus = 21; break;
      case 21: temp2 = r; commandStatus = 22; break;
      case 22: temp3 = r; commandStatus = 23; break;
      case 23: if (r == 0x55)
        {
          for (int j = 8; j < 16; j++)
            pixels.setPixelColor(j, pixels.Color(temp1, temp2, temp3));
          pixels.show();
        } commandStatus = 0; break;

      case 30: temp1 = r; commandStatus = 31; break;
      case 31: temp2 = r; commandStatus = 32; break;
      case 32: temp3 = r; commandStatus = 33; break;
      case 33: if (r == 0x55)
        {
          for (int j = 0; j < 8; j++)
            pixels.setPixelColor(j, pixels.Color(temp1, temp2, temp3));
          pixels.show();
        } commandStatus = 0; break;

      case 40: temp1 = r; commandStatus = 41; break;
      case 41: temp2 = r; commandStatus = 42; break;
      case 42: temp3 = r; commandStatus = 43; break;
      case 43: if (r == 0x55)
        {
          for (int j = 0; j < 16; j++)
            pixels.setPixelColor(j, pixels.Color(temp1, temp2, temp3));
          pixels.show();
        } commandStatus = 0; break;

      case 50: temp1 = r; commandStatus = 51; break;
      case 51: if (r == 0x55)
        {
          servoA0.write(temp1); 
          //int t = 988 + temp1 * 4;
         // ServoA0_1; delayMicroseconds(t);  ServoA0_0;
        }
        commandStatus = 0; break;

      case 60: temp1 = r; commandStatus = 61; break;
      case 61: if (r == 0x55)
        {
           servoA1.write(temp1); 
          //int t = 988 + temp1 * 4;//256 
          //ServoA1_1; delayMicroseconds(t);  ServoA1_0;
        }
        commandStatus = 0; break;
    }
  }
}

unsigned int TimeMap[281] =
{
  //2, 3 , 3 , 4 , 4, 4 , 4 , 4 , 4 , 4,
  // 5, 6 , 7 , 8 , 9, 10 , 10 , 10 , 10 , 10 ,
  6, 6, 7, 7, 7, 7, 7, 8, 8, 8,
  8, 8, 9, 9, 9, 9, 10, 10, 10, 10,
  10 , 10 , 11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 ,
  11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 ,
  11 , 12 , 12 , 12 , 12 , 12 , 12 , 12 , 12 , 12 ,
  13 , 13 , 13 , 13 , 13 , 13 , 14 , 14 , 14 , 14 ,
  14 , 15 , 15 , 15 , 15 , 16 , 16 , 16 , 17 , 17 ,
  18 , 18 , 19 , 19 , 20 , 20 , 21 , 21 , 22 , 23 ,
  23 , 24 , 25 , 26 , 27 , 28 , 29 , 30 , 31 , 32 ,
  34 , 35 , 36 , 38 , 39 , 41 , 43 , 45 , 47 , 49 ,
  51 , 54 , 56 , 59 , 62 , 65 , 68 , 71 , 74 , 78 ,
  82 , 86 , 91 , 95 , 100 , 105 , 111 , 117 , 123 , 129 ,
  136 , 144 , 151 , 159 , 168 , 177 , 187 , 197 , 208 , 219 ,
  231 , 244 , 257 , 272 , 287 , 303 , 319 , 337 , 356 , 376 ,
  0,
  376 , 356 , 337 , 319 , 303 , 287 , 272 , 257 , 244 , 231 ,
  219 , 208 , 197 , 187 , 177 , 168 , 159 , 151 , 144 , 136 ,
  129 , 123 , 117 , 111 , 105 , 100 , 95 , 91 , 86 , 82 ,
  78 , 74 , 71 , 68 , 65 , 62 , 59 , 56 , 54 , 51 ,
  49 , 47 , 45 , 43 , 41 , 39 , 38 , 36 , 35 , 34 ,
  32 , 31 , 30 , 29 , 28 , 27 , 26 , 25 , 24 , 23 ,
  23 , 22 , 21 , 21 , 20 , 20 , 19 , 19 , 18 , 18 ,
  17 , 17 , 16 , 16 , 16 , 15 , 15 , 15 , 15 , 14 ,
  14 , 14 , 14 , 14 , 13 , 13 , 13 , 13 , 13 , 13 ,
  12 , 12 , 12 , 12 , 12 , 12 , 12 , 12 , 12 , 11 ,
  11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 ,
  11 , 11 , 11 , 11 , 11 , 11 , 11 , 11 , 10 , 10 ,
  10, 10, 10, 10, 9, 9, 9, 9, 8, 8,
  8, 8, 8, 7, 7, 7, 7, 7, 6, 6
  // 10 , 10 , 10 , 10 , 10 , 9 , 8, 7 , 6 , 5,
  // 4 , 4 , 4, 4, 4, 4 , 4 , 3, 3 , 2

};
unsigned int idleX = 0, idleY = 0, idleZ = 0, idleA = 0;
unsigned int NoSignal = 0;
void loop()
{
  if (SpeedX == MidSpeed && SpeedY == MidSpeed && SpeedZ == MidSpeed && SpeedA == MidSpeed )
  {
    if (NoSignal < 1000)
      NoSignal++;
    else
    {
      NoSignal = 100;
      DisableMotor;
    }
  }
  else
  {
    NoSignal = 0;
    EnableMotor;
  }

  if (idleX < NowTimeX)
    idleX++;
  else //时间到了
  {
    //产生脉冲
    if (SpeedX > MidSpeed)
      MotorXD0;
    else
      MotorXD1;
    MotorXP0;
    if (SpeedX != MidSpeed) MotorXP1;
    MotorXP0;
    //计算下次的时间间隔
    if (TargetSpeedX != SpeedX)
      if (TargetSpeedX > SpeedX)
        SpeedX++;
      else
        SpeedX--;
    NowTimeX = TimeMap[SpeedX] / 2;
    idleX = 0;
  }

  if (idleY < NowTimeY)
    idleY++;
  else //时间到了
  {
    //产生脉冲
    if (SpeedY > MidSpeed)
      MotorYD0;
    else
      MotorYD1;
    MotorYP0;
    if (SpeedY != MidSpeed) MotorYP1;
    MotorYP0;
    //计算下次的时间间隔
    if (TargetSpeedY != SpeedY)
      if (TargetSpeedY > SpeedY)
        SpeedY++;
      else
        SpeedY--;
    NowTimeY = TimeMap[SpeedY] / 2;
    idleY = 0;
  }

  if (idleZ < NowTimeZ)
    idleZ++;
  else //时间到了
  {
    //产生脉冲
    if (SpeedZ > MidSpeed)
      MotorZD0;
    else
      MotorZD1;
    MotorZP0;
    if (SpeedZ != MidSpeed) MotorZP1;
    MotorZP0;
    //计算下次的时间间隔
    if (TargetSpeedZ != SpeedZ)
      if (TargetSpeedZ > SpeedZ)
        SpeedZ++;
      else
        SpeedZ--;
    NowTimeZ = TimeMap[SpeedZ] / 2;
    idleZ = 0;
  }

  if (idleA < NowTimeA)
    idleA++;
  else //时间到了
  {
    //产生脉冲
    if (SpeedA > MidSpeed)
      MotorAD0;
    else
      MotorAD1;
    MotorAP0;
    if (SpeedA != MidSpeed) MotorAP1;
    MotorAP0;
    //计算下次的时间间隔
    if (TargetSpeedA != SpeedA)
      if (TargetSpeedA > SpeedA)
        SpeedA++;
      else
        SpeedA--;
    NowTimeA = TimeMap[SpeedA] / 2;
    idleA = 0;
  }

  delayMicroseconds(14);
}

/*
   while (1)
  {
    if (Serial.available())
    {
      int rd = Serial.read();
      switch (rd)
      {
        case '1': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorXD0;
            MotorXP0;
            MotorXP1;
            Serial.write('1');
            delay(2);
          } break;
        case '2': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorXD1;
            MotorXP0;
            MotorXP1;
            Serial.write('1');
            delay(2);
          } break;
        case '3': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorYD0;
            MotorYP0;
            MotorYP1;
            Serial.write('1');
            delay(2);
          } break;
        case '4': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorYD1;
            MotorYP0;
            MotorYP1;
            Serial.write('1');
            delay(2);
          } break;
        case '5': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorZD0;
            MotorZP0;
            MotorZP1;
            Serial.write('1');
            delay(2);
          } break;
        case '6': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorZD1;
            MotorZP0;
            MotorZP1;
            Serial.write('1');
            delay(2);
          } break;
        case '7': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorAD0;
            MotorAP0;
            MotorAP1;
            Serial.write('1');
            delay(2);
          } break;
        case '8': EnableMotor; for (int i = 0; i < 100; i++) {
            MotorAD1;
            MotorAP0;
            MotorAP1;
            Serial.write('1');
            delay(2);
          } break;
        case 'a': DisableMotor; break;
      }
    }
  }
  17, 18, 18, 19, 20, 20, 21, 21, 22, 23,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
  34, 35, 36, 38, 39, 41, 43, 45, 47, 49,
  51, 54, 56, 59, 62, 65, 68, 71, 74, 78,
  82, 86, 91, 95, 100, 105, 111, 117, 123, 129,
  136, 144, 151, 159, 168, 177, 187, 197, 208, 219,
  231, 244, 257, 272, 287, 303, 319, 337, 356, 380,
  0,
  380 , 356 , 337 , 319 , 303, 287 , 272 , 257 , 244 , 231 ,
  219 , 208 , 197 , 187 , 177 , 168 , 159 , 151 , 144, 136 ,
  129 , 123 , 117 , 111 , 105, 100 , 95 , 91 , 86 , 82 ,
  78 , 74 , 71 , 68 , 65 , 62, 59 , 56 , 54 , 51 ,
  49 , 47 , 45 , 43 , 41 , 39 , 38, 36 , 35 , 34 ,
  32 , 31 , 30 , 29 , 28 , 27 , 26 , 25, 24 , 23 ,
  23 , 22 , 21 , 21 , 20 , 20 , 19 , 18, 18 , 17
  {
  22, 23, 24, 24, 25, 25, 26,
  27, 27, 28, 29, 30, 31, 32,
  33, 35, 36, 38, 40, 42, 44,
  47, 50, 53, 56, 60, 65, 69,
  74, 80, 86, 93, 101, 110, 119,
  129, 141, 154, 168, 183, 201, 219,
  240, 264, 289, 318, 349, 373, 400,
  0,
  400, 373, 349, 318, 289, 264, 240,
  219, 201, 183, 168, 154, 141, 129,
  119, 110, 101, 93, 86, 80, 74,
  69, 65, 60, 56, 53, 50, 47,
  44, 42, 40, 38, 36, 35, 33,
  32, 31, 30, 29, 28, 27, 27,
  26, 25, 25, 24, 24, 23, 22
  };
  {
  2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 4,
  4, 4, 4, 5, 5, 6, 6,
  7, 8, 9, 10, 11, 13, 15,
  17, 19, 22, 25, 29, 33, 38,
  44, 51, 59, 68, 79, 91, 106,
  123, 142, 165, 191, 222, 257, 299,
  0,
  299, 257, 222, 191, 165, 142, 123,
  106,  91,  79,  68,  59,  51, 44,
  38,  33,  29,  25,  22,  19, 17,
  15,  13,  11,  10,   9,   8,  7,
  6,   6,   5,   5,   4,   4,  4,
  4,   3,   3,   3,   3,   3,  3,
  2,   2,   2,   2,   2,   2,  2
  };
  {
  11, 12, 12, 12, 12, 12, 12, 13, 13, 13,
  13, 13, 14, 14, 14, 15, 15, 15, 16, 16,
  17, 17, 17, 18, 19, 19, 20, 21, 21, 22,
  23, 24, 25, 26, 27, 29, 30, 31, 33, 35,
  36, 38, 40, 43, 45, 47, 50, 53, 56, 60,
  63, 67, 71, 76, 80, 85, 91, 97, 103, 110,
  117, 125, 133, 142, 152, 162, 173, 185, 197,
  0,
  197, 185, 173, 162, 152, 142, 133, 125, 117,
  110, 103, 97, 91, 85, 80, 76, 71, 67, 63,
  60, 56, 53, 50, 47, 45, 43, 40, 38, 36,
  35, 33, 31, 30, 29, 27, 26, 25, 24, 23,
  22, 21, 21, 20, 19, 19, 18, 17, 17, 17,
  16, 16, 15, 15, 15, 14, 14, 14, 13, 13,
  13, 13, 13, 12, 12, 12, 12, 12, 12, 11
  };
*/

