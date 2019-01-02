#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include<M5Stack.h>

class KeyBoard {
 public:
   KeyBoard();
   void Init();
   void GetValue(void);
 public:
   uint8_t keyData[5];
   uint16_t adX, adY;
 private:
  void Led(int i, int r, int g, int b);
  void LedControl(void);
 private:
  uint8_t adXL, adYL,adXH,adYH;
  uint16_t oldAdX, oldAdY;
  uint16_t medianAdX, medianAdY;
  uint8_t oldKey;
  int8_t consoleX, consoleY;

};
#endif
