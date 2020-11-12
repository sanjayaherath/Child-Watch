#include <Sim800l.h>
#include <SoftwareSerial.h> //is necesary for the library!! 



Sim800l Sim800l;  //to declare the library
String numberSms;
uint8_t index;   // to indicate the message to read.
bool error;       // to store the error output
bool match;       // to check whether numbers match
String number ="+94719688884";
int LEDpin=5;

void setup(){
  Serial.begin(9600);
  Sim800l.begin(); // initializate the library. 
  index=1; // first position in the prefered memory storage. 
  

}

void loop(){
  getNumber();
  Serial.println(numberSms);
  checkMatch(numberSms);
  error=Sim800l.delAllSms();
  delay(1000);
   
    }
 


void getNumber(){                      // to get the number
  Serial.println("get number");
  numberSms=Sim800l.getNumberSms(index);
  
  
  }


void checkMatch(String smsNumber){      //check whether numbers are equal
  Serial.println("checkSms");
  if(smsNumber.equals(number)){
    //what to do when number is equal
    digitalWrite(LEDpin,HIGH);
    delay(1000);
    digitalWrite(LEDpin,LOW);
    Serial.println("checkSmstrue");
    match=true;
    }
  else{
    match=false;
    Serial.println("checkSmsfalse");
      }
    }
 void addNumber(String newNo){          // add a new no. 
    number=newNo;
  
  }

 void sendMessage(){
    Sim800l.sendSms(number,"come get me");
  
  }
  
  
