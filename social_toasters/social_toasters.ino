#include <SPI.h>
#include <WiFly.h>
#include "EmonLib.h"                   // Include Emon Library

int toasterID = 0;
char ssid[] = "CEL2"; // network name
char passphrase[] = "j3llyf1sh"; // password
#define REMOTE_FEED_DATASTREAMS 4
long toasterFeed[5] = {91258, 91259, 91260, 91261, 91262}; 

long serverFeedID = 91254;
long localFeedID = toasterFeed[toasterID];

String API = "o9DJZaSWEcrSlqJjuwrJLCVpcN2SAKxXcmkrVUc1Q2c0TT0g";

// Instantiations
WiFlyClient client;
EnergyMonitor emon1;

// Current monitoring variables
const int numReadings = 10;
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
boolean counting = false;
double currentSense[numReadings];

// More variables
float value;
int streamID; 
int totalUsage = 0;

// Pin assignments
int resetPin = 3;
int coilAnalogInputPin = A1;

// decoding stuffs
char buff[64]; // incoming data, maximum of each line
boolean foundCurrentV = false;
boolean clientConnected = false;
boolean found200 = false;
char *found;
int pointer = 0;
float remoteData[REMOTE_FEED_DATASTREAMS];



int wiflyFailure = 0; // to count how many faliure of connecting to network
int maxWiflyFailure  = 5;

long timeLastUpdated = 0; //millis of last attemp to connect to cosm
long lastPutMillis = millis();

long lastAttempMillis = millis();
long last200Millis = millis();
long check200Interval = 15*1000;
boolean gotLastTotalUsage = false; 
int state = 0; 
// 0 = just started, 
// 1 = sent request for last total usage, 
// 2 = got last total usage, sent subscription, 
// 3 = got 200 ok from the subscription, ready for main loop

void setup(){
  Serial.begin(9600);
  Serial.println(F("starting up"));
  delay(5000);
  WiFlyStartup(); //start wifLy and connecting to network, also check for error and force reset
  
  pinMode(coilAnalogInputPin, INPUT); 
  
  // initialize all the readings to 0: 
  for (int a = 0; a < numReadings; a++) {
    currentSense[a] = 0;  
  }
  
  emon1.current(1, 111.1);             // Current: input pin, calibration.

  Serial.print(F("connecting..."));
  if (client.connect("beta.pachube.com", 8081)) {
    Serial.print(F("success.."));
    delay(1000);
    cosmSocketSub(serverFeedID,-1,"getCommand"); // subscribe to stream 10

    //pachubeSubscribe(ROOMBASCHEDULE, "getSchedule"); // stream -1 means follow the whole feed
    Serial.println(F("wait 2 sec for feedback"));

    delay(2000);
    timeLastUpdated = millis();
    last200Millis = millis();
  } 
  else {
    Serial.println("failed");
  }
  
  
}

void loop(){
  
  
  
//  if(millis() - lastPutMillis > 5000){
//    lastPutMillis = millis();
//    totalUsage ++;
//    Serial.println(F("putting"));
//    
//   //cosmSocketPut(FEEDID, 1, random(5)); 
//   //cosmSocketGet(FEEDID);
//   //cosmToasterPut(totalUsage, totalUsage*2, totalUsage*3, totalUsage*4, totalUsage*5);
//   cosmSocketPut(4, int(random(20)));
//  }


// Add an element to the current sample array and calculate the average
  currentSense[index] = emon1.calcIrms(1480);
  total = 0;  
  for (int a = 0; a < numReadings; a++) { total += currentSense[a]; }                
  average = total / numReadings;
 
  if (average >= 20) {
    if (counting == false) {
      counting = true;
      Serial.println("/nBEGINNING TO TOAST/n");
    }
  }
    
  if (average < 20) {
   if (counting == true) {
    counting = false;
    Serial.println("/nTOAST ENDED/n");
   }
  }
 
  index++;
  if (index >= numReadings) {
    index = 0;
  }   




  checkConnection();
  
  
  while (client.available()) {
    //char c = client.read();
    //Serial.print(c); 
    checkResponse();
  }
}





