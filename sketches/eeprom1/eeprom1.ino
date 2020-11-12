/*
EEPROM  sketch
memery locations
pin:11-14
no1:21-32
no2:41-52
*/
#include <EEPROM.h>

String no1="+94719688884";
String getno1;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

getno1=readEEPROM(21,32);
Serial.println(getno1);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void writeEEPROM(int startAddr,int endAddr,String val ){
  for(int eindex=startAddr;eindex<=endAddr;eindex++){
    EEPROM.write(eindex,val[eindex-startAddr]);
    delay(10);
    }
  
  
  }

String readEEPROM(int startAddr,int endAddr){
  char val[endAddr+1-startAddr];
  for(int eindex=startAddr;eindex<=endAddr;eindex++){
    val[eindex-startAddr]=EEPROM.read(eindex);
    delay(10);
    }
    
  return String(val);
  }
