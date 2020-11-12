//#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include <Sim800l.h>
#include <SoftwareSerial.h> //is necesary for the library!! 
#include "U8glib.h"

RTC_DS3231 rtc;
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Variable assignment
char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char optionKey = 'M', commandKey = '0';
char preferenceKey;
// Variables for button press
char mode[5] = {'M', 'S', 'C', 'P'};
int modeIndex = 0, centerPin = 7, upPin = 9, leftPin = 6, rightPin = 8;
long pressedTime = 0;
// Variables for time quantities 
int hourHand, minuteHand, secondHand;
int preHour, preMinute, preSecond;
// Variable for stopwatch quantites
int stopMinute = 0, stopSecond = 0, stopMilliSecond = 0, inProgress = 0, preUnit, resetSW = 0;
int stopMinuteX = 0, stopSecondX = 54, stopMilliSecondX = 108, stopwatchY = 5;
int durationSW, resetUnit, loopDelayCount = 20;
// Variables for animation
int lineLength = 0, blackOn = 1;
// Cursor positions
int hourX = 20, minuteX = 70, dotX = 54, timeY = 12;
int dateX = 30, dateY = 2,durationOff;
//gsm
Sim800l Sim800l;  //to declare the library
String numberSms;
uint8_t index;   // to indicate the message to read.
bool error;       // to store the error output
bool match;       // to check whether numbers match
String number ="+94719688884";
int LEDpin=5, checkSmsDelay = 1, checkSmsTimes = 10;

// Declaring methods
void mainWindowTimeAndDate();
void mainWindowAnime();
void runStopwatch();
void runClockSetting();

void displayColan();
void printHand();
void dotsTicking();
void printDateAndTime();
void drawHoriLine();
void reserStopwatch();
void setResetUnitCursor();

void setup()   {               
  Serial.begin(9600);
 // rtc.adjust(DateTime(2018, 11, 8, 10, 53, 00));
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  DateTime now = rtc.now();
  printDateAndTime(now.day(),now.month(),daysOfTheWeek[now.dayOfTheWeek()]);
//gsm
  
  Sim800l.begin(); // initializate the library. 
  index=1; // first position in the prefered memory storage. 
  
}
int times = 0;
void loop(){
  // Set duration for pulsating 
  if(inProgress == 0)durationOff = 80;
  if(inProgress == 1)durationOff = 10;
  commandKey = '0';
  
  DateTime now = rtc.now();
  // Check button press
    checkButtonPress();
  /* 
  // Read from the serial 
  if(Serial.available()){
    preferenceKey = (char)Serial.read();
    while(Serial.available()){
    Serial.read();
    }
    commandKey = preferenceKey;
  }
  // Serial read is over
  */  
  // Run main window section
  //if(checkSms == 0 && optionKey == 'M') Serial.println("SHAPE");
  if(optionKey == 'M'){  
    mainWindowTimeAndDate();
    mainWindowAnime();
  }
  // Run stopwatch section
  if(optionKey == 'S' || inProgress == 1){
    runStopwatch();
    }
  // Run setting section  
  if(optionKey != 'C')resetUnit = 0;
  if(optionKey == 'C'){    
     runClockSetting();
    }  
  // Run pin enter section
//  if()
  
 //gsm 
  if(lineLength%33 == 0 && blackOn == 1) checkSmsDelay++;
  if ((checkSmsDelay % checkSmsTimes) == 0){
    checkSmsDelay = 1;
  getNumber();
  checkMatch(numberSms);
 bool error=Sim800l.delAllSms();
  }
} 

/////////////////////////////////////// MAIN METHODS ///////////////////////////////////////////// 
// Main window of the clock
void mainWindowTimeAndDate(){
    displayColan(':');
    display.display();
    DateTime now = rtc.now();
    preSecond = secondHand;
    secondHand = now.second();
      if (secondHand == 0){   
        preMinute = minuteHand;
        minuteHand = now.minute();      
        // Display the change in minutes
        printHand(preMinute,minuteHand,minuteX,timeY,2);
      
        // Trigger hour change       
          if (preMinute > minuteHand){          
            preHour = hourHand;
            hourHand = now.hour();          
            // Display the change in hours
            printHand(preHour,hourHand,hourX,timeY,2);
          
            // Trigger date change          
              if (preHour > hourHand){                    
                printDateAndTime(now.day(),now.month(),daysOfTheWeek[now.dayOfTheWeek()]);                                       
                }                               
              }        
            }
    if (preSecond != secondHand){
    display.setCursor(dotX,timeY);
    display.setTextColor(BLACK);
    displayColan(':');
    display.display();    
    delay(durationOff);
    display.setTextColor(WHITE);                       
        }
 }
      
// Main window animation
void mainWindowAnime(){
  if(preSecond == secondHand) delay(durationOff);
  lineLength = lineLength % 33;
  if(lineLength == 0 && blackOn == 1)blackOn = 0;
  else if (lineLength == 0)blackOn = 1;
  drawHoriLine(lineLength++); 
  }

// Stopwatch window
void runStopwatch(){
  if(stopMinute == 0 && stopSecond == 0 && stopMilliSecond == 0 && resetSW == 0){
    stopMinute = 0, stopSecond = 0, stopMilliSecond = 0;
    resetStopwatch();  
    }
  // Start the stopwatch
  if (commandKey == '2' || inProgress == 1){
  inProgress = 1;
  delay(durationSW);
  if(stopMilliSecond == 10 ){
    stopMilliSecond = stopMilliSecond % 10;  
    preUnit = stopSecond++;
    //
    if(optionKey == 'S')printHand(preUnit,stopSecond%60,stopSecondX,stopwatchY,2);
    if(stopSecond == 60){
      stopSecond = stopSecond % 60; 
      preUnit = stopMinute++;
      //
      if(optionKey == 'S')printHand(preUnit,stopMinute%100,stopMinuteX,stopwatchY,2);
      }
      stopMinute = stopMinute % 100;
    }     
  preUnit = stopMilliSecond++;
  // 
  if(optionKey == 'S')printHand(preUnit,stopMilliSecond%10,stopMilliSecondX,stopwatchY,1);
  display.display();
 }

// Pause the operation
  if(commandKey == '8' && optionKey == 'S'){
    if(inProgress == 0){
      stopMinute = 0, stopSecond = 0, stopMilliSecond = 0;
      resetStopwatch();
    }
    // Reset the stopwatch
    else inProgress = 0;      
      }   
   
  }

// Run clock setting window
void runClockSetting(){
  // preHour = hourHand; preMinute = minuteHand;
      switch (commandKey){
        case '4':
          resetUnit++;
          resetUnit = (2 + resetUnit % 2) % 2;
          break;
        case '6':
          resetUnit--;
          resetUnit = (2 + resetUnit % 2) % 2;
          break;
        }
    setResetUnitCursor();
  }
/////////////////////////////////////SUBORDINATE METHODS////////////////////////////////
// Display colans
void displayColan(char sign){
  switch (sign){
    case ':':
      display.setCursor(dotX,timeY);
      display.print(':');
      break;
    case '/':
      break;
    }
  }
// Print time quantites
void printHand(int preUnit,int timeUnit,int cursorX,int cursorY,int numDigit){    
    display.setCursor(cursorX,cursorY);
    display.setTextColor(BLACK);
    if(preUnit<10 && numDigit == 2)display.print(0);
    display.print(preUnit);
    display.setCursor(cursorX,cursorY);
    display.setTextColor(WHITE);
    if(timeUnit<10 && numDigit == 2)display.print(0);
    display.print(timeUnit);
    }
//Display date/month DAY 
void printDateAndTime(int dayInfo, int monthInfo, String dayName){
  display.clearDisplay();
  display.setCursor(dateX,dateY);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print(dayInfo);
  display.print(" / ");
  display.print(monthInfo);
  display.print("  ");
  display.print(dayName);
  display.setTextSize(3);
  DateTime now = rtc.now();
  hourHand = now.hour(); minuteHand = now.minute(); secondHand = now.second();  
  printHand(0,hourHand,hourX,timeY,2);
  printHand(0,minuteHand,minuteX,timeY,2);
}
// Display the animation
void drawHoriLine(int lineWidth){
  int cursorX = 0;
  double width = (double)lineWidth ;
  width++; 
  width = ceil((width/8)*(width/6));
  while(cursorX < width){
    if (blackOn == 0){   
  display.drawPixel(cursorX,31 - lineWidth,WHITE);  
  display.drawPixel(127-cursorX++,31 - lineWidth,WHITE);   
    }
    else {
        display.drawPixel(cursorX,31 - lineWidth,BLACK);  
        display.drawPixel(127-cursorX++,31 - lineWidth,BLACK); 
      }
   }
}  
// Reset stopwatch
void resetStopwatch(){  
  resetSW = 1;
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.clearDisplay();
  // Display the minute in stopwatch
  printHand(0,stopMinute,stopMinuteX,stopwatchY,2);
  display.print(":");
  // Display the second in stopwatch
  printHand(0,stopSecond,stopSecondX,stopwatchY,2);
  display.print(":");
  // Display the milliscond in stopwatch
  printHand(0,stopMilliSecond,stopMilliSecondX,stopwatchY,1);
  display.display();
  } 
// Change clock settings
void setResetUnitCursor(){
  
    DateTime now = rtc.now();
    switch (resetUnit){
      case 0:      
          display.clearDisplay();
          printHand(0, minuteHand, minuteX, timeY, 2);
          displayColan(':');
          display.display();
        switch (commandKey){
          case '2':
          preMinute = minuteHand--; minuteHand = (60 + minuteHand % 60) % 60;
          rtc.adjust(DateTime(now.year(), now.month(), now.day(), hourHand, minuteHand, secondHand));
          printHand(preMinute, minuteHand, minuteX, timeY, 2);
          display.display();break;
          case '8':
          preMinute = minuteHand++; minuteHand = (60 + minuteHand % 60) % 60;
          rtc.adjust(DateTime(now.year(), now.month(), now.day(), hourHand, minuteHand, secondHand));
          printHand(preMinute, minuteHand, minuteX, timeY, 2);
          display.display();break;          
          }
        break;  
      case 1:
          display.clearDisplay();
          printHand(0, hourHand, hourX, timeY, 2);
          displayColan(':');
          display.display();
        switch (commandKey){
          case '2':
          preHour = hourHand--; hourHand = (24 + hourHand % 24) % 24;
          rtc.adjust(DateTime(now.year(), now.month(), now.day(), hourHand, minuteHand, secondHand));
          printHand(preHour, hourHand, hourX, timeY, 2);
          display.display();break;
          case '8':
          preHour = hourHand++; hourHand = (24 + hourHand % 24) % 24;
          rtc.adjust(DateTime(now.year(), now.month(), now.day(), hourHand, minuteHand, secondHand));
          printHand(preHour, hourHand, hourX, timeY, 2);
          display.display();  break;       
          }
        break; 
      }
}

//gsm methods
void getNumber(){                      // to get the number
  numberSms=Sim800l.getNumberSms(index);
  
  
  }


void checkMatch(String smsNumber){      //check whether numbers are equal
  if(smsNumber.equals(number)){
    //what to do when number is equal
    digitalWrite(LEDpin,HIGH);
    delay(1000);
    digitalWrite(LEDpin,LOW);
    match=true;
    }
  else{
    match=false;
      }
    }
 void addNumber(String newNo){          // add a new no. 
    number=newNo;
  
  }

 void sendMessage(){
    Sim800l.sendSms(number,"come get me");
  
  }
void checkButtonPress(){
  int centerVal = digitalRead(centerPin);
  int upVal = digitalRead(upPin); 
  int leftVal = digitalRead(leftPin);
  int rightVal = digitalRead(rightPin);
  if (centerVal == LOW){
    if((millis() - pressedTime) > 200){
    modeIndex = (++modeIndex) % 4;
    pressedTime = millis();
    }
    }
  if (upVal == LOW){
    if((millis() - pressedTime) > 200){
    commandKey = '4';
    pressedTime = millis();
    }    
  }
  if (leftVal == LOW){
    if((millis() - pressedTime) > 200){
    commandKey = '8';
    pressedTime = millis();
    }    
  }
  if (rightVal == LOW){
    if((millis() - pressedTime) > 200){
    commandKey = '2';
    pressedTime = millis();
    }    
  }
  
    preferenceKey = mode[modeIndex];
    // Current window is the main window
    if (preferenceKey == 'M' && optionKey != 'M'){
      durationSW = 52; 
      optionKey = 'M';
      DateTime now = rtc.now();
      printDateAndTime(now.day(),now.month(),daysOfTheWeek[now.dayOfTheWeek()]);
      lineLength = 0; blackOn = 1;
      }
    // Current window is the stopwatch window
    if(preferenceKey == 'S' && optionKey != 'S'){
      optionKey = 'S';
      durationSW = 77;
      if(inProgress == 1 || inProgress == 0){
        resetStopwatch();
        }
    }
    // Current window is the settings window
    if(preferenceKey == 'C' && optionKey != 'C'){
      durationSW = 50;
      optionKey = 'C';
      }
    // Current window is the enter pin window
    if(preferenceKey == 'P' && optionKey != 'P'){
      durationSW = 50;
      optionKey = 'P';
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(dateX,dateY);
      display.print("Enter Pin");
      display.display();
      }
  }
