#include <SPI.h>
#include <WiFly.h>

int toasterID = 0;
char ssid[] = "CEL2"; // network name
char passphrase[] = "j3llyf1sh"; // password
#define REMOTE_FEED_DATASTREAMS 4
long toasterFeed[5] = {91258, 91259, 91260, 91261, 91262}; 

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

  checkConnection();
  
  
  while (client.available()) {
    //char c = client.read();
    //Serial.print(c); 
    checkResponse();
  }
}





