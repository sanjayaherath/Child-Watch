#include <SoftwareSerial.h>


String Arsp, Grsp;
SoftwareSerial gsm(4,3); //  RX, TX

void setup() {

  Serial.begin(9600);
  Serial.println("Setup");
  Serial.println("Testing GSM SIM800L");

  pinMode(13, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  gsm.begin(9600);
}

void loop() {
  // turn on the led
  

  if(gsm.available())
  {
    //delay(10);
    Serial.println("gsm available");
    delay(10);
    Grsp = gsm.readString();
   //delay(10);
    Serial.print(Grsp);
    
    //delay(10);
    Serial.println("gsm available end");
  }

  if(Serial.available())
  {
    
    Arsp = Serial.readString();

    Serial.println("Serial available");
    //delay(10);
    Serial.println(Arsp);
    //delay(10);
    gsm.println(Arsp);
    //delay(10);
    Serial.println("Serial available end");
  }

  // turn off the led
  //digitalWrite(LED_BUILTIN, LOW);

  
}
