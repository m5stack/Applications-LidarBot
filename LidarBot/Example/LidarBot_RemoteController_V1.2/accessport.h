#ifndef __ACCESSPORT_H__
#define __ACCESSPORT_H__
#include<M5Stack.h>

class AccessPort{
  public:
  
   AccessPort();
   void MapSend(const uint8_t *data,int len);
   void AnalzyCommand(void);
   
  public:
  
  private:
  
  private:
    bool SendFlag;
};

#endif
