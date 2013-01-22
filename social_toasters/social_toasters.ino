#include <SPI.h>    
#include <WiFly.h>
#include "EmonLib.h"                   // Include Emon Library
#include <EEPROM.h>

const int toasterID = 5; // start from 1
const boolean servoLeft = true; // servo position facing front of the toaster
const boolean report = false; // debugging network
const boolean servoPrint = false; // debugging servo
const boolean socketServer = false;  // use socket server or not

const int happinessStream = 2 + toasterID; 


const long toasterFeed[5] = {
  98446, 98447, 98448, 98449, 98450}; 

const long localFeedID = toasterFeed[toasterID-1];
//const long localFeedID = 98425;
const long avgFeed = 98451;

String API = "o9DJZaSWEcrSlqJjuwrJLCVpcN2SAKxXcmkrVUc1Q2c0TT0g";

// Instantiations
WiFlyClient client;
EnergyMonitor emon1;

char ssid[26]; // network name
char passphrase[26]; // password

// Current monitoring variables
const int numReadings = 10;
int currentCoilIndex = 0;                  // the index of the current reading
int average = 0;                // the average
boolean counting = false;
double currentSense[numReadings];
boolean prevToastInProgress = false; // prev state of being used, to compare with the current state

// More variables
unsigned int totalUsage = 0;  // actual total usage that is being used
unsigned int localTotalUsage = 0; // total usage read from EEPROM
unsigned int remoteTotalUsage = 0; // total usage read from Cosm

// Pin assignments
const int ledPin = 5;
const int arduinoResetPin = 3;
const int coilAnalogInputPin = A1;
const int servoPin = 9;
const int relayPin = 2;

// JSON Socket Server decoding variables/stuff
float tempRemoteValue; // to store temp value
int streamID;  // to store temp stream ID
char buff[64]; // incoming data, maximum of each line
boolean found200 = false;
boolean isReading = false; // only use this when not using socket server
int contentLength = 0;
char *found;
int pointer = 0;

int wiflyFailure = 0; // to count how many faliure of connecting to network
const int maxWiflyFailure  = 5;

unsigned long lastAttempMillis = millis();
unsigned long last200Millis = millis();
const unsigned long check200Interval = 15000;
const unsigned long waitingLimit = 5000; // if doesn't get the feedback in 3 secs, close socket and finish reading


unsigned long lastEmotionMillis = millis();
unsigned long emotionInterval = 900000; // 15 mins

unsigned long lastActionMillis = millis();
const unsigned long sleepTimer = 450000; // 5mins

long lastRelayMillis = millis(); 
const long relayInterval = 1000; // stay off for 1 sec
boolean powerAllow = true;

long startToastMillis = millis();
const long minToastMillis = 30000; //at least 30 seconds, in order to count as a toast

int state = 0; 
int failure = 0;
float happiness = 0;
float prevHappiness = -1;
const int angryTres = 4;

int maxResist = 5;
int resistCount = 0;

int ledMode = 1; // 0-3 0=off, 1=getting network, 2= normal, 3=problemo
boolean servoRunning = false;
int emotionServoMode = 0;
int firstExpress = 0;

boolean toggle2 = 0;
int ledCounter = 0;
float sleepRad = 0;
int ledBrightness = 0;
boolean sleeping = false;
boolean networkConfig = false;
int networkConfigState = 0;
IPAddress server(64,94,18,121);
int port;

#define SETUP_HARDWARE 0 
#define SETUP_WIFLY 1 
#define CONNECTING_COSM 2 
#define GET_USAGE 3
#define READ_USAGE 4
#define GET_HAPPINESS 5
#define READ_HAPPINESS 6
#define COMPARE_VAR 9
#define MAIN_LOOP 10
#define NETWORK_CONFIG 11

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
  pinMode(relayPin, OUTPUT);
  setupLed();
  setupPort();
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

  case SETUP_HARDWARE:
    delay(5000);
    setupCurrentCoil();        // Initialize the current sensing coil and its averaging array
    emon1.current(1, 111.1);   // Current: input pin, calibration.
    readEEPROM();
    setupServo();              // Designate the servo pin and move it to home position               
    state ++;
    break; 

  case SETUP_WIFLY:

    WiFlyStartup(); // Start wiFly and connect to the network, also check for error and force reset

    break;

  case CONNECTING_COSM: // open connection to cosm
    ledMode = 1;


    Serial.print(F("connecting Cosm.. "));
    if (client.connect(server, port)) {
      Serial.println(F("success.."));
      //delay(1000);
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

  case GET_USAGE:
    ledMode = 1;
    // ask for last total usage (stream 0)
    cosmGet(localFeedID,0,"getLastUsage", "get"); 
    //cosmNormalGet(localFeedID,0);
    Serial.println(F("getting last total usage.."));
    lastAttempMillis = millis();
    state ++;
    break;

  case READ_USAGE: // and wait for result (check in decoder)
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

  case GET_HAPPINESS:
    ledMode = 1;
    // ask for last happiness from avg feed 
    cosmGet(avgFeed,happinessStream,"getHappiness", "get"); 

    Serial.println(F("getting last happiness.."));
    lastAttempMillis = millis();
    state ++;
    break;

  case READ_HAPPINESS: // and wait for result (check in decoder)
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

  case COMPARE_VAR: // compare and adjust local and online value
    ledMode = 1;
    adjustValue();
    Serial.println(F("adjust var completed, starting main loop"));
    Serial.print("\n\n");
    state ++;
    ledMode = 2;
    break;

  case MAIN_LOOP: // main loop

    mainLoop();

    break;

  case NETWORK_CONFIG:

    //doing nothing

    break;

  }




  while (client.available()) {
    if(state == 8){
      ledMode = 3;
    }

      checkResponse();

  }

  while (Serial.available() > 0) {
    serialFeedback();
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
  
  // If we aren't in the middle of a servo action or memory reading, update the current reading
  if(!servoRunning && !isReading){
    checkCurrent();   //current sensing 
  }
  
  // Then check connectivity, whether to put the toaster into sleep mode due to inactivity, and whether power should be on or off
  checkConnection();
  checkSleep();
  powerControl();

  // If we're not toasting, express any emotions waiting
  if(!isToasting()){
    moveServo();
    expressEmotion();
  }

  // If the toaster has just started or just finished toasting, update as necessary
  if(prevToastInProgress != isToasting()){

    if(isToasting()){
      
      Serial.print("Lever is pressed! ");
      
      if(resistCount < maxResist){
        resistCount ++;
        cutPower();
        Serial.print(F("But the toaster is still pissed, resisCount = "));
        Serial.println(resistCount);
      }
      else{
        resistCount = 0;
        cosmPut3(localFeedID, 0, totalUsage, 4, happiness, 5, 1, "leverPressed");
        startToastMillis = millis();
      }

    } 
    else {
      
      Serial.println(F("Lever is released"));
      if(millis() - startToastMillis > minToastMillis) {
        totalUsage++;
        saveLocalUsage(totalUsage);
        Serial.print(F("Total usage is now: "));
        Serial.println(totalUsage);
      } else {
        Serial.print(F("that was a fake toast!"));
      }
      
      cosmPut3(localFeedID, 0, totalUsage, 4, happiness, 5, 0, "leverReleased");
      lastAttempMillis = millis();
    }

    prevToastInProgress = isToasting();
  }


  // If our happiness has changed, calculate how many times to resist being used and wiggle your arm
  if(prevHappiness != happiness){

    maxResist = resistCal();
    emotionCal();    
    if(happiness < prevHappiness){
      if(isToasting()){
        startServo(0);
        lastEmotionMillis = millis();
      }
    }
    prevHappiness = happiness; 

  }



}
