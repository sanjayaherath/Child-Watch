#include <Sim800l.h>
#include <SoftwareSerial.h> //is necesary for the library!! 



Sim800l Sim800l;  //to declare the library
String text;     // to storage the text of the sms
String textSms,numberSms;
uint8_t index;   // to indicate the message to read.
bool error;
String number ="+94719688884";
int LEDpin=3;

void setup(){
	Serial.begin(9600); // only for debug the results . 
	Sim800l.begin(); // initializate the library. 
	index=1; // first position in the prefered memory storage. 
  pinMode(LEDpin,OUTPUT);

}

void loop(){
	
  text=Sim800l.readSms(index);    
  Serial.println(text);
  numberSms=Sim800l.getNumberSms(index);
  //numberSms.remove(0,1);
  Serial.println(numberSms);
  error=Sim800l.delAllSms();
  if(numberSms.equals(number)){
    Serial.println("match");
    digitalWrite(LEDpin,HIGH);
    delay(500);
    digitalWrite(LEDpin,LOW);
    }
  delay(1000);
}
