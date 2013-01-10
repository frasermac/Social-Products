#include <SPI.h>
#include <WiFly.h>
#include "EmonLib.h"                   // Include Emon Library

int toasterID = 5; // start from 1
boolean servoLeft = true;
boolean report = false; // debugging
boolean servoPrint = false; // debugging

int happinessStream = 2 + toasterID; 


long toasterFeed[5] = {
  96617, 96618, 96620, 96621, 96622}; 

long localFeedID = toasterFeed[toasterID-1];
long avgFeed = 96616;

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
int relayPin = 2;

// JSON Socket Server decoding variables/stuff
float tempRemoteValue; // to store temp value
int streamID;  // to store temp stream ID
char buff[64]; // incoming data, maximum of each line
boolean foundCurrentV = false;
boolean clientConnected = false;
boolean found200 = false;
char *found;
int pointer = 0;

int wiflyFailure = 0; // to count how many faliure of connecting to network
int maxWiflyFailure  = 5;

unsigned long lastAttempMillis = millis();
unsigned long last200Millis = millis();
unsigned long check200Interval = 5000;

unsigned long lastEmotionMillis = millis();
unsigned long emotionInterval = 900000; // 15 mins

unsigned long lastActionMillis = millis();
unsigned long sleepTimer = 450000; // 5mins

long lastRelayMillis = millis(); 
long relayInterval = 1000; // stay off for 1 sec
boolean powerAllow = true;

int state = 0; 
int failure = 0;
float happiness = 0;
float prevHappiness = -1;
int angryTres = 4;

int maxResist = 5;
int resistCount = 0;

int ledMode = 1; // 0-3 0=off, 1=getting network, 2= normal, 3=problemo
boolean servoRunning = false;
int emotionServoMode = 0;
boolean firstExpress = 0;

boolean toggle2 = 0;
int ledCounter = 0;
float sleepRad = 0;
int ledBrightness = 0;
boolean sleeping = false;

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
  Serial.print(F("TOASTER: "));
  Serial.print(toasterID);
  Serial.print(F("  FEEDID: "));
  Serial.print(localFeedID);
  Serial.println(F("  starting up.. "));
 
  setupLed();   
  state = -1;
}


ISR(TIMER2_COMPA_vect){ // interupt timer at 8kHz
  ledControl();
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

  switch(state){

  case -1:
    delay(5000);
    setupCurrentCoil();        // Initialize the current sensing coil and its averaging array
    emon1.current(1, 111.1);   // Current: input pin, calibration.
    readSD();                  // Read the network config from the the SD card
    setupServo();              // Designate the servo pin and move it to home position
    WiFlyStartup();            // Start wiFly and connect to the network, also check for error and force reset
    state ++;
    break; 

  case 0: // open connection to cosm
    ledMode = 1;
    Serial.print(F("connecting Cosm.. "));
    if (client.connect("api.cosm.com", 8081)) {
      Serial.println(F("success.."));
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
    cosmSocketGet(localFeedID,0,"getLastUsage"); 
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
    cosmSocketGet(avgFeed,happinessStream,"getHappiness"); 
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
    cosmSocketSub(avgFeed, happinessStream,"happinessSub");
    Serial.println(F("sent subscription to cosm.."));
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
    Serial.println(F("////////////////////////////////////////"));
    Serial.println();
    state ++;
    ledMode = 2;
    break;

  case 8: // main loop

    mainLoop();

    break;


  }





  while (client.available()) {
    if(state == 8){
      ledMode = 3;
    }
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
  
  if(!servoRunning){
    checkCurrent();   //current sensing 
  }
  
  checkConnection();
  checkSleep();
  powerControl();
  
  if(!toastInProgress){
    moveServo();
    expressEmotion();
  }
  
  
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
        cosmSocketPut3(localFeedID, 0, totalUsage, 5, 1, -1, 0, "leverPressed");

      }

    }
    else{
      Serial.println(F("lever is released"));
      cosmSocketPut3(localFeedID, 5, 0, -1, -1, -1, -1, "leverReleased");
      lastAttempMillis = millis();
    }

    prevToastInProgress = toastInProgress;
  }





  if(prevHappiness != happiness){

    maxResist = resistCal();
    emotionCal();    
    if(happiness < prevHappiness){
      startServo(0);
      lastEmotionMillis = millis();
    }
    prevHappiness = happiness; 

  }



}

















