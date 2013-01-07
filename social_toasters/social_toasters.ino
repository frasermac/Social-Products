#include <SPI.h>
#include <WiFly.h>
#include "EmonLib.h"                   // Include Emon Library

int toasterID = 0;
//int happinessStream = 3 + toasterID; 
int happinessStream = 4; 
boolean report = false; // debugging

long toasterFeed[5] = {
  91258, 91259, 91260, 91261, 91262}; 

long localFeedID = toasterFeed[toasterID];
long avgFeed = 91254;

String API = "o9DJZaSWEcrSlqJjuwrJLCVpcN2SAKxXcmkrVUc1Q2c0TT0g";

// Instantiations
WiFlyClient client;
EnergyMonitor emon1;



// Current monitoring variables
const int numReadings = 10;
int currentCoilIndex = 0;                  // the index of the current reading
int average = 0;                // the average
boolean counting = false;
double currentSense[numReadings];
boolean toastInProgress = false;  // being used
boolean prevToastInProgress = false; // prev state of being used, to compare with the current state

// More variables
int totalUsage = 0;  // actual total usage that is being used
int localTotalUsage = 0; // total usage read from SD card
int remoteTotalUsage = 0; // total usage read from Cosm

// Pin assignments
int ledPin = 5;
int arduinoResetPin = 3;
int coilAnalogInputPin = A1;
int servoPin = 9;

// JSON Socket Server decoding variables/stuff
int tempRemoteValue; // to store temp value
int streamID;  // to store temp stream ID
char buff[64]; // incoming data, maximum of each line
boolean foundCurrentV = false;
boolean clientConnected = false;
boolean found200 = false;
boolean foundHappiness = false;
char *found;
int pointer = 0;

int wiflyFailure = 0; // to count how many faliure of connecting to network
int maxWiflyFailure  = 5;

unsigned long lastAttempMillis = millis();
unsigned long last200Millis = millis();
unsigned long check200Interval = 15000;

unsigned long lastEmotionMillis = millis();
unsigned long emotionInterval = 900000; // 15 mins

unsigned long lastTestMillis = millis();
unsigned long testInterval = 15000;

int state = 0; 
int failure = 0;
int happiness = 0;
int prevHappiness = 0;
int angryTres = 3;

int maxResist = 5;
int resistCount = 0;

int ledMode = 1; // 0-3 0=off, 1=getting network, 2= normal, 3=problemo
boolean servoRunning = false;



/*
// 
 ////////////////////////////////////////////////////////////////////////////
 //
 //  Setup loop:
 //    Initializes the serial, current sensing coil, SD card, servo, and
 //    wiFly module
 //
 */


void setup(){
  Serial.begin(9600);
  Serial.println(F("starting up"));
  delay(5000);

  setupLed();                // Initialize led pin
  setupCurrentCoil();        // Initialize the current sensing coil and its averaging array
  emon1.current(1, 111.1);   // Current: input pin, calibration.
  readSD();                  // Read the network config from the the SD card
  setupServo();              // Designate the servo pin and move it to home position
  WiFlyStartup();            // Start wiFly and connect to the network, also check for error and force reset

}



/*
// 
 ////////////////////////////////////////////////////////////////////////////
 //
 //  Constant loop:
 //    Switches between states depending on whether the toaster has lost power,
 //    is waiting for a reply, etc.
 //
 //
 */


void loop(){

  ledControl();

  switch(state){
  case 0: // open connection to cosm
    ledMode = 1;
    Serial.print(F("connecting..."));
    if (client.connect("beta.pachube.com", 8081)) {
      Serial.print(F("success.."));
      delay(1000);
      state ++;
      failure = 0;
      lastAttempMillis = millis();
      last200Millis = millis();
    } 
    else {
      failure ++;
      Serial.println(F("failed.."));
      Serial.println(failure);
      if(failure > 5){
        forceReset();
      }
      Serial.println(F("wait 5 secs, then try again"));
      delay(5000);

    }

    break;

  case 1:
    ledMode = 1;
    // ask for last total usage (stream 0)
    cosmSocketGet(localFeedID,0); 
    Serial.println(F("getting last total usage.."));
    delay(1000);
    lastAttempMillis = millis();
    state ++;
    break;

  case 2: // and wait for result (check in decoder)
    ledMode = 1;
    if(millis() - lastAttempMillis > 5000){
      Serial.println(F("waiting too long, asking again"));
      state --;
      failure ++;
      if(failure > 10){
        forceReset();
      }
    }

    break;

  case 3:
    ledMode = 1;
    // ask for last happiness from avg feed 
    cosmSocketGet(localFeedID,happinessStream); 
    Serial.println(F("getting last happiness.."));
    delay(1000);
    lastAttempMillis = millis();
    state ++;
    break;

  case 4: // and wait for result (check in decoder)
    ledMode = 1;
    if(millis() - lastAttempMillis > 5000){
      Serial.println(F("waiting too long, asking again"));
      state --;
      failure ++;
      if(failure > 10){
        forceReset();
      }
    }

    break;

  case 5: // sent subscription
    ledMode = 1;
    cosmSocketSub(localFeedID, happinessStream,"happinessSub1");
    Serial.println(F("sent subscription to cosm.."));
    //    cosmSocketSub(avgFeed, happinessStream,"happinessSub2");
    //    Serial.println(F("sent subscription to cosm.."));
    lastAttempMillis = millis();
    state ++;
    break;

  case 6: // and wait for 200 ok (check in decoder)
    ledMode = 1;
    if(millis() - lastAttempMillis > 5000){
      Serial.println(F("waiting too long, asking again"));
      state --;
      failure ++;
      if(failure > 10){
        forceReset();
      }
    }
    break;

  case 7: // compare and adjust local and online value
    ledMode = 1;
    adjustValue();
    Serial.println(F("adjust var completed, starting main loop"));
    Serial.println(F("----------------------------------------"));
    Serial.println();
    state ++;
    ledMode = 2;
    break;

  case 8: // main loop

    mainLoop();


    break;


  }





  while (client.available()) {
    checkResponse();
  }

}




/*
// 
 ////////////////////////////////////////////////////////////////////////////
 //
 //  void mainLoop():
 //    Switches between states depending on whether the toaster has lost power,
 //    is waiting for a reply, etc.
 //
 //
 */





void mainLoop(){
  if(!servoRunning){ // for some reason we can't run servo and check current at the same time
    checkCurrent();   //current sensing 
  }

  checkConnection();

  if(!toastInProgress){
    moveServo();
    expressEmotion();
  }

//  if(millis() - lastTestMillis > testInterval){
//    lastTestMillis = millis();
//    startServo(0);
//
//  }


  if(prevToastInProgress != toastInProgress){

    if(toastInProgress){
      Serial.print("lever is pressed..");
      if(resistCount < maxResist){
        resistCount ++;
        cutPower();
        Serial.print(F("but the toaster is still pissed, resisCount = "));
        Serial.println(resistCount);
      }
      else{
        resistCount = 0;
        totalUsage ++;
        Serial.print(F("total usage is "));
        Serial.println(totalUsage);
        cosmSocketPut2(localFeedID, 0, totalUsage, 5, 1);

      }

    }
    else{
      Serial.println(F("lever is released"));
      cosmSocketPut(localFeedID, 5, 0);
      lastAttempMillis = millis();
    }

    prevToastInProgress = toastInProgress;
  }





  if(prevHappiness != happiness){

    maxResist = resistCal();

    if(happiness < prevHappiness){
      if(!toastInProgress){
        startServo(2);
        lastEmotionMillis = millis();
      }
    }
    prevHappiness = happiness; 

  }



}


















