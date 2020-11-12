#include <Sim800l.h>
#include <SoftwareSerial.h> //is necesary for the library!! 



Sim800l Sim800l;  //to declare the library

String numberSms, textSms;
bool error;
String number = "+94719688884";
int motPin = 3;

void setup() {
  
  Sim800l.begin(); // initializate the library.
  Serial.begin(9600);
  pinMode(motPin, OUTPUT);
  Sim800l.delAllSms(); //clean memory of sms;
}

void loop() {
  checkSms();
  delay(1000);
}

void checkSms() {
  textSms = Sim800l.readSms(1);
  if (textSms.indexOf("OK") != -1) {
    if (textSms.length() > 7) {
      numberSms = Sim800l.getNumberSms(1);
      Serial.println(numberSms);
      if (numberSms.equals(number)) {
      Serial.println("match");
      playSmsAlarm();
    }
    Sim800l.delAllSms();
  }

  
    
  }
}

void sendMessage() {          // send sms to contacts
  char* sendsmstext = "match";
  error = Sim800l.sendSms(number, sendsmstext);
  if (error) {
    digitalWrite(motPin, HIGH);
    delay(800);
    digitalWrite(motPin, LOW);
  }


}

void playAlarm() {            //alarm notification motor
  for (int i = 0; i < 5; i++) {
    digitalWrite(motPin, HIGH);
    delay(600);
    digitalWrite(motPin, LOW);
    delay(1000);

  }
}

void playSmsAlarm() {         //sms received notification motor
  for (int i = 0; i < 5; i++) {
    digitalWrite(motPin, HIGH);
    delay(400);
    digitalWrite(motPin, LOW);
    delay(200);
    digitalWrite(motPin, HIGH);
    delay(400);
    digitalWrite(motPin, LOW);
    delay(1000);
  }
}

