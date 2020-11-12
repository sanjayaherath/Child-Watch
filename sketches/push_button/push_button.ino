# include <EEPROM.h>

int addr = 0;
void setup(){
  Serial.begin(9600);

  } 

void loop()  {
  for(int i = 100;i < 1111; i++){
    EEPROM.write(addr,i);
  delay(10);
  Serial.println(EEPROM.read(addr), DEC);}}
