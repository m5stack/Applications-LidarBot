#include "accessport.h"

AccessPort::AccessPort(){
  SendFlag = true;
}

void AccessPort::MapSend(const uint8_t *data,int len){ 
     if(!SendFlag){  
       Serial.write(data,len);
       SendFlag = true;
      }
}

void AccessPort::AnalzyCommand(void){
  uint8_t buffer[4];
  if(Serial.available() == 3){
   Serial.readBytes(buffer,3);
   if(!strcmp((char *)buffer,"ack")){
    SendFlag = false;   
   }
   Serial.flush();  
  }else{
    while(Serial.available() > 0){
      Serial.read();
    }
  }
}
