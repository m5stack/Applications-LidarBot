#ifndef __LIDAR_H__
#define __LIDAR_H__
#include <M5Stack.h>
#include "RprTrack.h"

//uint8_t  rent_pp[9];


/*!
 * \brief The LidarCar class
 */
class LidarCar {
 public:
   
   LidarCar();
   void Init(void);
   void ControlMode(void);
   void ControlWheel(int8_t X, int8_t Y, byte A);
   void GetData(void);
   void MapDisplay(void);
   void CarCortrol(void);
   void CarMaze(void);
   void CarCamera();
   void TrackControl(void);
   void LedShow(void);
 public:
   uint8_t mapdata[180];
   uint16_t distance[360];
   float motor_out = 0,motor_y_out = 0;
   float last_motor_out;
   int Cortrol_flag = 0;
   int count = 0;
   float last_error_line = 0;
   int contro_mode = 0;

 private:
   void setLedColor(byte i, byte r, byte g, byte b);
   void setFrontLedBar( byte r, byte g, byte b);
   void setBackLedBar( byte r, byte g, byte b);
   void setLedAll( byte r, byte g, byte b);
   void setServo0Angle(uint8_t angle);
   void setServo1Angle(uint8_t angle);
   int MazaCom(float error_line,float left_line,float right_line,float front_line);
 private:
   int last_line;
   int go_flag = 0;
   int disX[360], disY[360];
   int oldDisX[360], oldDisY[360];
   int Dis[180][2];
   int commandStatus;
   int ver;
   uint16_t dataLength;
   int lidarSpeed;
   uint16_t angleOffset;
   uint16_t startAngle;
   int showAngle;
   int packCount;
   uint8_t signalValue[360];
   uint16_t temp;
   int wheelPowerA, wheelPowerB, wheelPowerC, wheelPowerD;
   int controlMap[15][15] ={
  {  4,  4,  5,  5,  6,  6,  7,  7,  7,  7, 7, 7, 7, 7, 7},
  {  2,  3,  4,  4,  5,  5,  6,  6,  6,  6, 6, 6, 6, 6, 7},
  {  0,  1,  3,  4,  4,  5,  5,  5,  5,  5, 5, 5, 5, 6, 7},
  { -1,  0,  1,  2,  2,  3,  4,  4,  4,  4, 4, 4, 5, 6, 7},
  { -2, -1,  0,  0,  2,  2,  3,  3,  3,  3, 3, 4, 5, 6, 7},
  { -4, -3, -1, -1,  0,  1,  2,  2,  2,  2, 3, 4, 5, 6, 7},
  { -6, -5, -3, -2, -1,  0,  0,  1,  1,  2, 3, 4, 5, 6, 7},
  { -7, -6, -5, -4, -3, -2, -1,  0,  1,  2, 3, 4, 5, 6, 7},
  { -7, -6, -5, -4, -3, -2,  0, -1, -1,  0, 1, 2, 3,  4, 5},
  { -6, -5, -4, -3, -2, -1,  0, -2, -2, -2, -1, 0, 1, 2, 3},
  { -6, -5, -4, -3, -2, -2, -3, -3, -3, -3, -3, -2, -1, 0, 1},
  { -5, -4, -3, -2, -3, -3, -4, -4, -4, -4,  -4,  -4,  -3,  2, -1},
  { -5, -4, -3, -3, -4, -4, -5, -5, -5, -5,  -5,  -5,  -5,  4, -3},
  { -4, -3, -4, -4, -5, -5, -6, -6, -6, -6,  -6,  -6,  -6,  -6,  -5},
  { -4, -4, -5, -5, -6, -6, -7, -7, -7, -7,  -7,  -7,  -7,  -7,  -7}
  };

  int controlMapX[15][15] =
  {
  { -2, -2, -1, 0, 1, 2, 3, 7, 4, 4, 4, 4, 4, 4, 4},
  { -2, -2, -1, 0, 0, 1, 2, 4, 3, 3, 3, 3, 3, 3, 4},
  { -3, -2, -1, -1, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 4},
  { -3, -2, -1, -1, -1, 0, 1, 2, 2, 2, 2, 2, 3, 3, 4},
  { -3, -2, -2, -1, -1, 0, 1, 2, 2, 2, 2, 2, 3, 3, 4},
  { -3, -2, -2, -2, -1, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4},
  { -3, -2, -2, -2, -2, -1, 0, 1, 1, 1, 2, 2, 3, 3, 4},
  { -7, -4, -2, -2, -2, -1, -1, 0, 1, 1, 2, 2, 2, 4, 7},
  { -4, -4, -2, -2, -2, -1, -1, -1, 0, 1, 2, 2, 2, 2, 3},
  { -4, -4, -2, -2, -2, -1, -1, -1, 0, 0, 1, 2, 2, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 2, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 1, 2, 3},
  { -4, -4, -2, -2, -2, -2, -2, -2, -2, -1, 0, 1, 1, 2, 3},
  { -4, -4, -4, -4, -4, -4, -4, -4, -2, -1, 0, 0, 1, 2, 2},
  { -4, -4, -4, -4, -4, -4, -4, -7, -3, -2, -1, 0, 1, 2, 2}
  };

  int controlMapY[15][15] =
  {
  {4, 4, 4, 4, 4, 4, 4, 7, 3, 2, 1, 0, -1, -2, -2} ,
  {4, 3, 3, 3, 3, 3, 3, 4, 2, 1, 0, 0, -1, -2, -2} ,
  {4, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, -1, -1, -2, -3},
  {4, 3, 3, 2, 2, 2, 2, 2, 1, 0, -1, -1, -1, -2, -3},
  {4, 3, 3, 2, 2, 2, 2, 2, 1, 0, -1, -1, -2, -2, -3} ,
  {4, 3, 3, 2, 2, 2, 1, 1, 0, 0, -1, -2, -2, -2, -3} ,
  {4, 3, 3, 2, 2, 1, 1, 1, 0, -1, -2, -2, -2, -2, -3} ,
  {7, 4, 2, 2, 2, 1, 1, 0, -1, -1, -2, -2, -2, -4, -7} ,
  {3, 2, 2, 2, 2, 1, 0, -1, -1, -1, -2, -2, -2, -4, -4} ,
  {3, 2, 2, 2, 1, 0, 0, -1, -1, -1, -2, -2, -2, -4, -4} ,
  {3, 2, 2, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {3, 2, 1, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {3, 2, 1, 1, 0, -1, -2, -2, -2, -2, -2, -2, -2, -4, -4} ,
  {2, 2, 1, 0, 0, -1, -2, -4, -4, -4, -4, -4, -4, -4, -4} ,
  {2, 1, 0, 0, -1, -2, -3, -7, -4, -4, -4, -4, -4, -4, -4}
  };
};


#endif
