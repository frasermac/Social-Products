#include <SPI.h>
#include <WiFly.h>

int toasterID = 0;
int happinessStream = 3 + toasterID; 
boolean report = false;



long toasterFeed[5] = {
  91258, 91259, 91260, 91261, 91262}; 

long localFeedID = toasterFeed[toasterID];
long avgFeed = 91254;

String API = "o9DJZaSWEcrSlqJjuwrJLCVpcN2SAKxXcmkrVUc1Q2c0TT0g";

WiFlyClient client;

float tempRemotevalue;
int streamID; 
int totalUsage = 0;
int localTotalUsage = 0;
int remoteTotalUsage = 0;
#define SIZE 300
int usageTrack[SIZE];

//pin assigning
int resetPin = 3;
int ledPin = 5;

// decoding stuffs
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
unsigned long check200Interval = 15000;

unsigned long fakeToastMillis = millis();
unsigned long fakeToastInterval = 15000;

unsigned long lastEmotionMillis = millis();
unsigned long emotionInterval = 900000; // 15 mins

int state = 0; 
int failure = 0;
int happiness = 0;
int prevHappiness = 0;
int angryTres = 3;

int maxResist = 5;
int resistCount = 0;

boolean prevBeingUsed = false;
boolean beingUsed = false;
int ledMode = 1; // 0-3 0=off, 1=getting network, 2= normal, 3=problemo

void setup(){
  Serial.begin(9600);
  Serial.println(F("starting up"));
  delay(5000);
  setupLed(); 
  readSD(); // read netwrok config from SD card
  setupServo();
  WiFlyStartup(); //start wiFly and connecting to network, also check for error and force reset

}

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
    
  case 3: // sent subscription
    ledMode = 1;
    cosmSocketSub(91254, happinessStream,"happynessSub");
    Serial.println(F("sent subscription to cosm.."));
    lastAttempMillis = millis();
    state ++;
    break;

  case 4: // and wait for 200 ok (check in decoder)
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

  case 5: // compare and adjust local and online value
    ledMode = 1;
    adjustValue();
    Serial.println(F("adjust var completed, starting main loop"));
    state ++;
    ledMode = 2;
    break;

  case 6: // main loop
    
    mainLoop();


    break;


  }

  
  


  while (client.available()) {
    checkResponse();
  }

}


void mainLoop(){


  checkConnection();
  fakeToast();
  
  if(!beingUsed){
    moveServo();
    expressEmotion();
  }
  

  beingUsed = toasterIsBeingUsed();
  
  if(!prevBeingUsed && beingUsed){

    if(resistCount < maxResist){
      resistCount ++;
      cutPower();
    }
    else{
      resistCount = 0;
      totalUsage ++;
      cosmSocketPut(localFeedID, 0, totalUsage);
      lastAttempMillis = millis();

    }
    
  } 
  
  prevBeingUsed = beingUsed;


  if(prevHappiness != happiness){

    maxResist = resistCal();

    if(happiness < prevHappiness){
      startServo(2);
      lastEmotionMillis = millis();
    }
    prevHappiness = happiness; 

  }



}















