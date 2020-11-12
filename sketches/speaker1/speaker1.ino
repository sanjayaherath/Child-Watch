int motPin = 3; //speakerPin to arduino pin 9


void setup(){
 
  pinMode(motPin, OUTPUT); // Set speakerPin - pin 9 as an output
  playAlarm();

}

void loop(){
 
 
}


void playAlarm(){
  for(int i=0;i<5;i++){
  digitalWrite(motPin,HIGH);
  delay(600);
  digitalWrite(motPin,LOW);
  delay(1000);  
  
      
  
  }    
  
  
  
  }

void playSmsAlarm(){
  for(int i=0;i<5;i++){
  digitalWrite(motPin,HIGH);
  delay(400);
  digitalWrite(motPin,LOW);
  delay(200);  
  digitalWrite(motPin,HIGH);
  delay(400);
  digitalWrite(motPin,LOW);
  delay(1000);  
      
  
  }
  
  }  
