#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include <Sim800l.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

RTC_DS3231 rtc;
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Variable assignment
char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char optionKey = 'M', commandKey = '0', preferenceKey;
// Variables for button press
char mode[4] = {'M', 'S', 'C', 'P'}, buttonKeyArray [4] = {'8', '5', '2', '4'};
int modeIndex = 0, centerPin = 7, upPin = 9, leftPin = 6, rightPin = 8;
long pressedTime = 0;
// Variables for watch quantities
int hourHand, minuteHand, secondHand;
int preHour, preMinute, preSecond;
int durationOff = 100;
// Variable for stopwatch quantites
int stopHour = 0, stopMinute = 0, stopSecond = 0;
int saveHour = 0, saveMinute = 0, saveSecond = 0, saveMilliSecond = 0;
int startHour, startMinute, startSecond;
int stopMilliSecond = 0, inProgress = 0;
int preStopMilliSecond, preStopSecond, preStopMinute;
int stopMinuteX = 0, stopSecondX = 54, stopMilliSecondX = 108, stopwatchY = 5;
int /*durationSW,*/ initiateStopwatch, loopDelayCount = 20;
// Variable for clock setting
int resetUnit, preChangeUnit = 0, modulusArray[] = {2100, 12, 31, 24, 60};;
// Variables for animation
int lineLength = 0, blackOn = 1;
// Cursor positions
// DateTimeX = {minuteX, hourX, dateX, monthX, yearX}
int DateTimeX[] = {70, 20, 30, 55, 80}, dotX = 56, slashX1 = 45, slashX2 = 70;
// DateTimeY = {timeY, timeY, dateY, dateY, dateY}
int DateTimeY[] = {12, 12, 2, 2, 2}, dotY = 12, slashY = 2;
// Variables for the GSM module
Sim800l Sim800l;  //to declare the library
String numberSms, textSms;
uint8_t index, msgIndex = 1;   // to indicate the message to read.
bool error;       // to store the error output
//bool match;       // to check whether numbers match
String number = "+94719688884";
int motPin = 3, checkSmsDelay = 1, checkSmsTimes = 3;
// Variables for the Pin enter window
int pinArray[4], pinIndex, disableMode = 0;
int pinCursorX[] = {25, 45, 65, 85}, pinCursorY = 12;
// Variables for the Pin/ Contact Info change window
int settingsMode, savedContacts[2][10], EntryX;
int contactCursorX[] = {10, 21, 32, 43, 54, 65, 76, 87, 98, 109}, contactCursorY = 15;
int blinkItCount = 0;
String Number1, Number2;
char contactInfo[13];

// Declaring methods
void mainWindowTimeAndDate();
void mainWindowAnime();
void runStopwatch();
void runClockSetting();

void displayColan();
void printHand();
void printDate();
void dotsTicking();
void printDateAndTime();
void drawHoriLine();
void showStopwatch();
void setResetUnitCursor();

void setup()   {

  //Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  DateTime now = rtc.now();
  printDateAndTime(now.day(), now.month(), daysOfTheWeek[now.dayOfTheWeek()]);

  //gsm
  Sim800l.begin(); // initializate the library.
  //Sim800l.reset();
  pinMode(motPin, OUTPUT);
  index = 1; // first position in the prefered memory storage.
  contactInfo[0] = '+', contactInfo[1] = '9', contactInfo[2] = '4', contactInfo[12] = '\0';
  for (int address = 5; address < 14; address++) {
    contactInfo[address - 2] = EEPROM.read(address) + '0';
  }
  Number1 = (String)(contactInfo);
  for (int address = 15; address < 24; address++) {
    contactInfo[address - 12] = EEPROM.read(address) + '0';
  }
  Number2 = (String)(contactInfo);
}

void loop() {


  //  Serial.println(Number1);
  //  Serial.println(Number2);
  commandKey = '0'; // reset command key to read only once
  DateTime now = rtc.now();
  checkButtonPress(); // Check any buttons are pressed

  // Run main window section
  if (optionKey == 'M') {
    mainWindowTimeAndDate();
    mainWindowAnime();
  }
  //  // Run stopwatch section
  //  if (optionKey == 'S' || inProgress == 1) {
  //    runStopwatch();
  //  }
  // Run setting section
  if (optionKey != 'C')resetUnit = 0;
  if (optionKey == 'C') {
    runClockSetting();
  }

  // Run pin enter section
  if (optionKey == 'P' && disableMode == 0) {
    display.setCursor(pinCursorX[pinIndex], pinCursorY);
    display.print(pinArray[pinIndex]);
    display.display();
    switch (commandKey) {
      case '2':
      case '8':
        changePinValue();
        break;
      case '4' :
        changeIndexOrCheckPin();
        break;
    }
  }

  //gsm


  if (lineLength % 33 == 0 && blackOn == 1) checkSmsDelay++;
  if ((checkSmsDelay % checkSmsTimes) == 0) {
    checkSmsDelay = 1;
    checkSms();
  }


  // Run pin / contact info change section
  if (disableMode == 1) {
    //Serial.println(EntryX);
    displayInfo();
    switch (commandKey) {
      case '4':
        increaseEntryX();
        break;
      case '2':
      case '8':
        changeEntryValue();
        break;
    }
  }
}

/////////////////////////////////////// MAIN METHODS /////////////////////////////////////////////
// Check for the pressed button
void checkButtonPress() {
  // Check for the pressed button
  for (int pinVal = 6; pinVal < 10; pinVal++) {
    if (digitalRead(pinVal) == LOW && (millis() - pressedTime) > 150) {
      // Get the corresponding keypad button
      pressedTime = millis();
      commandKey = buttonKeyArray[pinVal - 6];
      if (commandKey == '5' && disableMode == 0) modeIndex = (++modeIndex) % 4;
      if (disableMode == 1 && commandKey == '5') {
        settingsMode = (++settingsMode) % 4;
        display.clearDisplay(); EntryX = 0;
      }
    }
  }
  preferenceKey = mode[modeIndex];
  // Current window is the main window
  if (preferenceKey == 'M' && optionKey != 'M') {
    //durationSW = 52;
    optionKey = 'M';
    DateTime now = rtc.now();
    printDateAndTime(now.day(), now.month(), daysOfTheWeek[now.dayOfTheWeek()]);
    lineLength = 0; blackOn = 1;
  }
  /*  // Current window is the stopwatch window
    if (preferenceKey == 'S' && optionKey != 'S') {
      optionKey = 'S';
      //durationSW = 77;
      showStopwatch();
      if (inProgress == 0) {
        initiateStopwatch = 0;
      }

    }*/
  // Current window is the settings window
  if (preferenceKey == 'C' && optionKey != 'C') {
    display.clearDisplay();
    //durationSW = 50;
    optionKey = 'C';
  }
  // Current window is the enter pin window
  if (preferenceKey == 'P' && optionKey != 'P') {
    //durationSW = 50;
    optionKey = 'P';
    displayEnterPin();
    pinIndex = 0;
    for (int index = 0; index < 4; index++) {
      pinArray[index] = 0;
    }
  }
}
// Main window of the clock
void mainWindowTimeAndDate() {
  displayColan(':');
  display.display();
  DateTime now = rtc.now();
  preSecond = secondHand;
  secondHand = now.second();
  if (secondHand == 0) {
    preMinute = minuteHand;
    minuteHand = now.minute();
    // Display the change in minutes
    printHand(preMinute, minuteHand, DateTimeX[0], DateTimeY[0], 2);

    // Trigger hour change
    if (preMinute > minuteHand) {
      preHour = hourHand;
      hourHand = now.hour();
      // Display the change in hours
      printHand(preHour, hourHand, DateTimeX[1], DateTimeY[1], 2);

      // Trigger date change
      if (preHour > hourHand) {
        printDateAndTime(now.day(), now.month(), daysOfTheWeek[now.dayOfTheWeek()]);
      }
    }
  }
  if (preSecond != secondHand) {
    display.setCursor(dotX, dotY);
    display.setTextColor(BLACK);
    displayColan(':');
    display.display();
    delay(durationOff);
    display.setTextColor(WHITE);
  }
}

// Main window animation
void mainWindowAnime() {
  if (preSecond == secondHand) delay(durationOff);
  lineLength = lineLength % 33;
  if (lineLength == 0 && blackOn == 1)blackOn = 0;
  else if (lineLength == 0)blackOn = 1;
  lineLength++;
  drawHoriLine(lineLength);
}
/*
  // Stopwatch window
  void runStopwatch() {
  // Start the stopwatch
  DateTime now = rtc.now();
  if (commandKey == '2' || inProgress == 1) {
    if (initiateStopwatch++ == 0) {
      startHour = now.hour(); startMinute = now.minute(); startSecond = now.second();
      stopMilliSecond = 0;
    }
    inProgress = 1;

    stopHour = now.hour() - startHour;
    stopMinute = now.minute() - startMinute;
    stopSecond = now.second() - startSecond;

    stopMinute = ((stopHour + saveHour) * 60) + stopMinute;
    stopSecond = ((stopMinute + saveMinute) * 60) + stopSecond + saveSecond;
    stopMinute = floor(stopSecond / 60); stopSecond = stopSecond % 60;
    stopMinute = (stopMinute) % 100;
    delay(25);
    stopMilliSecond = ++stopMilliSecond % 10;
    if (optionKey == 'S') {
      // Display millisecond unit
      printHand(preStopMilliSecond, (stopMilliSecond + saveMilliSecond) % 10, stopMilliSecondX, stopwatchY, 1);
      // Display second unit
      if (preStopSecond != stopSecond) {
        printHand(preStopSecond, stopSecond, stopSecondX, stopwatchY, 2);
      }
      // Display minute unit
      if (preStopMinute != stopMinute) {
        printHand(preStopMinute, stopMinute, stopMinuteX, stopwatchY, 2);
      }
      display.display();
    }
    preStopMilliSecond = (saveMilliSecond + stopMilliSecond) % 10;
    preStopSecond = stopSecond;
    preStopMinute = stopMinute;
  }

  // Pause and reset the stopwatch
  if (commandKey == '8' && optionKey == 'S') {
    // Reset the stopwatch
    if (inProgress == 0) {
      preStopMinute = 0, preStopSecond = 0, preStopMilliSecond = 0, stopMilliSecond = 0;
      saveHour = 0, saveMinute = 0, saveSecond = 0, saveMilliSecond = 0;
      showStopwatch();
    }
    // Pause the stopwatch
    else {
      inProgress = 0;
      initiateStopwatch = 0;
      saveHour = stopHour, saveMinute = preStopMinute, saveSecond = preStopSecond;
      saveMilliSecond = preStopMilliSecond;
    }
  }
  }*/
// Run clock setting window
void runClockSetting() {
  if (commandKey == '4') {
    resetUnit = ++resetUnit % 5;
  }
  setResetUnitCursor();
}

/////////////////////////////////////SUBORDINATE METHODS////////////////////////////////
// Print time quantites
void printHand(int preUnit, int timeUnit, int cursorX, int cursorY, int numDigit) {
  display.setCursor(cursorX, cursorY);
  display.setTextColor(BLACK);
  if (preUnit < 10 && numDigit == 2)display.print(0);
  display.print(preUnit);
  display.setCursor(cursorX, cursorY);
  display.setTextColor(WHITE);
  if (timeUnit < 10 && numDigit == 2)display.print(0);
  display.print(timeUnit);
}
// Print date quantites
void printDate(int timeUnit, int cursorX, int cursorY) {
  display.setCursor(cursorX, cursorY);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print(timeUnit);
  display.setTextSize(3);
}
//Display date/month DAY
void printDateAndTime(int dayInfo, int monthInfo, String dayName) {
  display.clearDisplay();
  display.setCursor(DateTimeX[2], DateTimeY[2]);
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
  printHand(0, hourHand, DateTimeX[1], DateTimeY[1], 2);
  printHand(0, minuteHand, DateTimeX[0], DateTimeY[0], 2);
}
// Display the animation
void drawHoriLine(int lineWidth) {
  int cursorX = 0;
  double width = (double)lineWidth ;
  width++;
  width = ceil((width / 8) * (width / 6));
  while (cursorX < width) {
    if (blackOn == 0) {
      display.drawPixel(cursorX, 31 - lineWidth, WHITE);
      display.drawPixel(127 - cursorX++, 31 - lineWidth, WHITE);
    }
    else {
      display.drawPixel(cursorX, 31 - lineWidth, BLACK);
      display.drawPixel(127 - cursorX++, 31 - lineWidth, BLACK);
    }
  }
}

// Set stopwatch screen and display
void showStopwatch() {
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.clearDisplay();
  // Display the minute in stopwatch
  printHand(0, preStopMinute, stopMinuteX, stopwatchY, 2);
  display.print(":");
  // Display the second in stopwatch
  printHand(0, preStopSecond, stopSecondX, stopwatchY, 2);
  display.print(":");
  // Display the milliscond in stopwatch
  printHand(0, preStopMilliSecond, stopMilliSecondX, stopwatchY, 1);
  display.display();
}

// Change clock settings
void setResetUnitCursor() {
  DateTime now = rtc.now();
  int currentSettings[] = {(int)now.year(), now.month(), now.day(), now.hour(), now.minute()};
  int changeIndex = 4 - resetUnit, changeUnit = currentSettings[changeIndex];
  int timeBound = modulusArray[changeIndex];
  if (preChangeUnit != changeUnit) {
    display.clearDisplay();
    preChangeUnit = changeUnit;
  }
  display.setTextSize(3); display.setTextColor(WHITE);
  displayColan(':'); displayColan('/'); display.display();

  if (resetUnit < 2) printHand(0, changeUnit, DateTimeX[resetUnit], DateTimeY[resetUnit], 2);
  else printDate(changeUnit, DateTimeX[resetUnit], DateTimeY[resetUnit]);
  display.display();
  if (commandKey == '2' || commandKey == '8') {
    changeUnit = changeUnit + (commandKey == '2') - (commandKey == '8');
    changeUnit = (timeBound + (changeUnit % timeBound)) % timeBound;
    switch (changeIndex) {
      case 0:
      case 1:
      case 2:
        changeUnit = timeBound * (changeUnit == 0) + changeUnit;
        break;
    }
    currentSettings[changeIndex] = changeUnit;
    rtc.adjust(DateTime(currentSettings[0], currentSettings[1], currentSettings[2], currentSettings[3], currentSettings[4], 0));
  }

  // Save parameters for the stopwatch
  if (commandKey == '2' || commandKey == '8') {
    saveHour = stopHour, saveMinute = preStopMinute, saveSecond = preStopSecond;
    saveMilliSecond = preStopMilliSecond;
    initiateStopwatch = 0;
  }
}
// Display colans
void displayColan(char sign) {
  switch (sign) {
    case ':':
      display.setCursor(dotX, dotY);
      display.print(':');
      break;
    case '/':
      display.setTextSize(1);
      display.setCursor(slashX1, slashY);
      display.print('/');
      display.setCursor(slashX2, slashY);
      display.print('/');
      display.setTextSize(3);
      break;
  }
}
// Display 'Enter Pin'
void displayEnterPin() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(DateTimeX[2] - 6, DateTimeY[2]);
  display.print("**Enter Pin**");
  display.setTextSize(3);
}
// Entering the pin
void changePinValue() {
  int pinValue = pinArray[pinIndex];
  pinValue = pinValue + (commandKey == '2') - (commandKey == '8');
  pinArray[pinIndex] = (10 + pinValue) % 10;
  displayEnterPin();
}
// Change the digit of the 4-digit Pin and check for permission
void changeIndexOrCheckPin() {
  int misMatch = 0;
  if (++pinIndex > 3) {
    for (int address = 0; address < 4; address++) {
      if (EEPROM.read(address) != pinArray[address]) {
        optionKey = 'M';
        misMatch = 1;
      }
    }
    if (misMatch == 0) {
      disableMode = 1;
      display.clearDisplay();
      settingsMode = 0, EntryX = 0, commandKey = '0';
      loadValues();
    }
  }
  else {
    displayEnterPin();
  }
}
// Load values from the EEPROM to the program
void loadValues() {
  // Storing the contact information
  for (int infoRow = 0; infoRow < 2; infoRow++) {
    for (int infoCol = 0; infoCol < 10; infoCol++) {
      int address = 10 * (infoRow) + infoCol + 4;
      savedContacts[infoRow][infoCol] = EEPROM.read(address);
    }
  }
}
// Display information in settings window
void displayInfo() {
  switch (settingsMode) {
    case 0: // Display the pin number
      display.setTextSize(1);
      display.setCursor(DateTimeX[2] - 12, DateTimeY[2]);
      display.print("*Enter New Pin*");
      display.setTextSize(3);
      for (int index = 0; index < 4; index ++) {
        display.setCursor(pinCursorX[index], pinCursorY);
        display.print(pinArray[index]);
      }
      display.display();
      blinkItCount = (++blinkItCount) % 20;
      if (blinkItCount == 0)blinkIt(pinArray[EntryX], pinCursorX[EntryX], pinCursorY);
      break;
    case 1: // Display the contact info 1
      display.setTextSize(1);
      display.setCursor(DateTimeX[2] + 6, DateTimeY[2]);
      display.print("Contact 1");
      display.setTextSize(2);
      for (int index = 0; index < 10; index ++) {
        display.setCursor(contactCursorX[index], contactCursorY);
        display.print(savedContacts[0][index]);
      }
      display.display();
      blinkItCount = (++blinkItCount) % 20;
      if (blinkItCount == 0)blinkIt(savedContacts[0][EntryX], contactCursorX[EntryX], contactCursorY);
      display.setTextSize(3);
      break;
    case 2: // Display the contact info 2
      display.setTextSize(1);
      display.setCursor(DateTimeX[2] + 6, DateTimeY[2]);
      display.print("Contact 2");
      display.setTextSize(2);
      for (int index = 0; index < 10; index ++) {
        display.setCursor(contactCursorX[index], contactCursorY);
        display.print(savedContacts[1][index]);
      }
      display.display();
      blinkItCount = (++blinkItCount) % 20;
      if (blinkItCount == 0)blinkIt(savedContacts[1][EntryX], contactCursorX[EntryX], contactCursorY);
      display.setTextSize(3);
      break;
    case 3: // Display the save settings window
      display.setTextSize(1);
      display.setCursor(DateTimeX[2] - 6, DateTimeY[2]);
      display.print("Save settings?");
      //display.setCursor(DateTimeX[2] - 10, DateTimeY[2] + 14);
      display.setCursor(0, DateTimeY[2] + 10);
      display.print("Move, Forward to save");
      //display.setCursor(DateTimeX[2] - 10, DateTimeY[2] + 24);
      display.setCursor(10, DateTimeY[2] + 20);
      display.print("Backward to cancel");
      display.display();
      break;
  }
}
// Blink the current variable digit
void blinkIt(int value, int cursorX, int cursorY) {
  display.setCursor(cursorX, cursorY);
  display.setTextColor(BLACK);
  display.print(value);
  display.display();
  delay(100);
  display.setTextColor(WHITE);
}
// Change the value of the current variable digit
void changeEntryValue() {
  switch (settingsMode) {
    case 0:
      pinArray[EntryX] = pinArray[EntryX] + (commandKey == '2') - (commandKey == '8');
      pinArray[EntryX] = (10 + pinArray[EntryX] % 10) % 10;
      display.clearDisplay();
      break;
    case 1:
      savedContacts[0][EntryX] = savedContacts[0][EntryX] + (commandKey == '2') - (commandKey == '8');
      savedContacts[0][EntryX] = (10 + savedContacts[0][EntryX] % 10) % 10;
      display.clearDisplay();
      break;
    case 2:
      savedContacts[1][EntryX] = savedContacts[1][EntryX] + (commandKey == '2') - (commandKey == '8');
      savedContacts[1][EntryX] = (10 + savedContacts[1][EntryX] % 10) % 10;
      display.clearDisplay();
      break;
    case 3:
      switch (commandKey) {
        case '2':
          // Save the new pin and return to main window
          for (int addressI = 0; addressI < 2; addressI++ ) {
            for (int addressJ = 0; addressJ < 10; addressJ++ ) {
              int address = 10 * addressI + addressJ + 4;
              EEPROM.write(address, savedContacts[addressI][addressJ]);
              if (address > 4 && address < 14) {
                contactInfo[address - 2] = savedContacts[addressI][addressJ] + '0';
                if (address == 13) {
                  //contactInfo[12] = '\0';
                  Number1 = (String)(contactInfo);
                }
              }
              else if (address > 14 && address < 24) {
                contactInfo[address - 12] = savedContacts[addressI][addressJ] + '0';
                if (address == 23) {
                  //contactInfo[12] = '\0';
                  Number2 = (String)(contactInfo);
                }
              }
            }
          }
          for (int address; address < 4; address++) {
            EEPROM.write(address, pinArray[address]);
          }
          for (int index = 0; index < 4; index++) {
            pinArray[index] = 0;
          }
          disableMode = 0;
          display.clearDisplay();
          display.setCursor(30, 10);
          display.setTextSize(2);
          display.print("Saved!");
          display.display();
          delay(1000);
          modeIndex = (++modeIndex) % 4;
          break;
        case '8':
          // Discard changes and return to main window
          disableMode = 0;
          display.clearDisplay();
          display.display();
          modeIndex = (++modeIndex) % 4;
          break;
      }
  }
}
// Change the index of the variable digit
void increaseEntryX() {
  switch (settingsMode) {
    case 0:
      EntryX = (++EntryX) % 4;
      break;
    case 1:
    case 2:
      EntryX = (++EntryX) % 10;
      break;
  }
}

// GSM METHODS //////////////////////////////

void checkSms() {
  delay(100);
  numberSms = Sim800l.getNumberSms(msgIndex);
  delay(100);
  Serial.print(numberSms + "\t");
  Serial.println(msgIndex);
  delay(1000);

  if (numberSms.equals(number)) {
    // Serial.println("match");
    displayMesRec();
    playSmsAlarm();
    sendMessage();
  }

  if (numberSms.length() > 10) {
    msgIndex++;
  }
  if (msgIndex > 1) {
    msgIndex = 1;
    Sim800l.delAllSms();
  }
}


void displayMesRec() {
  display.clearDisplay();
  display.setCursor(20, 15);
  display.setTextSize(1);
  display.print("Message Received");
  display.display();
  display.setTextSize(3);
  display.clearDisplay();
  DateTime now = rtc.now();
  printDateAndTime(now.day(), now.month(), daysOfTheWeek[now.dayOfTheWeek()]);
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

