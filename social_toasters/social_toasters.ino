#include <SPI.h>
#include <WiFly.h>

int toasterID = 2;
char ssid[] = "CEL2"; // network name
char passphrase[] = "j3llyf1sh"; // password
#define REMOTE_FEED_DATASTREAMS 4
long toasterFeed[5] = {
  91258, 91259, 91260, 91261, 91262}; 

long serverFeedID = 91254;
long localFeedID = toasterFeed[toasterID];

String API = "o9DJZaSWEcrSlqJjuwrJLCVpcN2SAKxXcmkrVUc1Q2c0TT0g";

WiFlyClient client;

float value;
int streamID; 
int totalUsage = 0;
//pin assigning
int resetPin = 3;


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

long lastAttempMillis = millis();

long last200Millis = millis();
long check200Interval = 15*1000;
int state = 0; 
int failure = 0;
int lastYY, lastMM, lastDD, lastHH, lastMN, lastSS;
int currentYY, currentMM, currentDD, currentHH, currentMN, currentSS; 


void setup(){
  Serial.begin(9600);
  Serial.println(F("starting up"));
  delay(5000);

  readSD(); // read netwrok config from SD card

  WiFlyStartup(); //start wiFly and connecting to network, also check for error and force reset


}

void loop(){

  switch(state){
  case 0: // just started
    // open connection to cosm
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

  case 2:

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
    cosmSocketSub(serverFeedID,-1,"getEverything");
    Serial.println(F("sent subscription to cosm.."));
    lastAttempMillis = millis();
    state ++;
    break;

  case 4:
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
    
    //checkConnection();
    break;


  }





  while (client.available()) {
    //char c = client.read();
    //Serial.print(c); 
    checkResponse();
  }
}








