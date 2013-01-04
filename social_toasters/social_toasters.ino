#include <SPI.h>
#include <WiFly.h>
#include "EmonLib.h"                   // Include Emon Library

int toasterID = 0;
boolean report = false;        // debugging

long toasterFeed[5] = {
  91258, 91259, 91260, 91261, 91262}; 

long localFeedID = toasterFeed[toasterID];

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

// More variables
int streamID; 
int totalUsage = 0;
int localTotalUsage = 0;
int remoteTotalUsage = 0;

// Pin assignments
int arduinoResetPin = 3;
int coilAnalogInputPin = A1;

// JSON Socket Server decoding variables/stuff
float tempRemoteValue;
char buff[64]; // incoming data, maximum of each line
boolean foundCurrentV = false;
boolean clientConnected = false;
boolean found200 = false;
char *found;
int pointer = 0;

int wiflyFailure = 0; // to count how many faliure of connecting to network
int maxWiflyFailure  = 5;

long lastAttempMillis = millis();
long last200Millis = millis();
long check200Interval = 15*1000;

long fakeToastMillis = millis();
long fakeToastInterval = 5*1000;

int state = 0; 
int failure = 0;
int happiness = 0;
int prevHappiness = 0;



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

  switch(state){
  case 0: // open connection to cosm

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
    // ask for last total usage (stream 0)
    cosmSocketGet(localFeedID,0); 
    Serial.println(F("getting last total usage.."));
    delay(1000);
    lastAttempMillis = millis();
    state ++;
    break;

  case 2: // and wait for result (check in decoder)

    if(millis() - lastAttempMillis > 5000){
      Serial.println(F("waiting too long, asking again"));
      state --;
      failure ++;
      if(failure > 10){
        forceReset();
      }
    }

    break;

  case 3: // sent subscription
    cosmSocketSub(localFeedID,4,"happynessSub");
    Serial.println(F("sent subscription to cosm.."));
    lastAttempMillis = millis();
    state ++;
    break;

  case 4: // and wait for 200 ok (check in decoder)
    if(millis() - lastAttempMillis > 5000){
      Serial.println(F("waiting too long, asking again"));
      state --;
      failure ++;
      if(failure > 10){
        forceReset();
      }
    }
    break;

  case 5: // compare and adjust local and online value
    adjustValue();
    Serial.println(F("adjust var completed, starting main loop"));
    state ++;
    break;

  case 6: // main loop

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




  currentSense[currentCoilIndex] = emon1.calcIrms(1480);    // Add an element to the current sample array
  isToasting();
 
  currentCoilIndex++;
  if (index >= numReadings) {
    currentCoilIndex = 0;
  }   




  checkConnection();
  fakeToast();
  moveServo();
  
  if(prevHappiness != happiness){
    if(happiness < prevHappiness){
      startServo(2);
    }
    prevHappiness = happiness; 
  }



}











