void adjustValue(){

  totalUsage = max(localTotalUsage, remoteTotalUsage); // use the larger one
  if(localTotalUsage != remoteTotalUsage){
    Serial.print(F("difference in totalUsage, use the larger one: ")); 
    Serial.println(totalUsage);
  }
  saveLocalUsage(totalUsage);
}


void expressEmotion(){

  if(firstExpress == 1){
    if(happiness < 7){
      Serial.println(F("expressing first emotion...")); 
      startServo(emotionServoMode);
    }
    firstExpress ++; 
  }


  if(millis() - lastEmotionMillis > emotionInterval){
    Serial.println(F("expressing emotion...")); 
    lastEmotionMillis = millis();
    lastActionMillis = millis();
    if(happiness < 7){
      startServo(emotionServoMode);
    }
  } 

}

void emotionCal(){


  if(happiness < 3){ //super pissed
    emotionInterval =  600000; // 10 mins
    emotionServoMode = 1;
  }
  else if(happiness >= 3 && happiness < 5){ //moderately pissed
    emotionInterval = 900000; // 15 mins
    emotionServoMode = 2;

  }
  else if(happiness >= 5 && happiness < 7){ //slightly pissed
    emotionInterval = 1200000; // 20 mins
    emotionServoMode = 3;

  }
  else{
    emotionInterval = 1200000; // 20 mins
    emotionServoMode = 2;

  }
  if(firstExpress == 0){
    firstExpress ++;
  }
  Serial.print(F("new emotionServoMode = ")); 
  Serial.print(emotionServoMode);
  Serial.print(F(",  new interval = ")); 
  Serial.println(emotionInterval);

}

int resistCal(){

  if(happiness < angryTres){
    int r = map(happiness, 0, angryTres, 5, 1);
    Serial.print(F("max resist = "));
    Serial.println(r);
    return r;
  }
  else{
    return 0; 
  }

}

void checkSleep(){
  if(millis() - lastActionMillis > sleepTimer){

    if(!sleeping){
      Serial.println(F("Going to sleep now"));
      sleepRad = PI/2;
    }
    sleeping = true;
    ledMode = 4;
  }
  else{
    if(sleeping){
      Serial.println(F("Woke up"));
    }
    sleeping = false;
    ledMode = 2;
  } 

}
boolean getSwitch = false;

// checkConnection() sends a fake update to the toaster's feed every 'check200Interval' milliseconds
void checkConnection(){
  int a = 0;
  if(millis() - lastAttempMillis > check200Interval){
    if(isToasting()){
      a = 1;
    }
    else{
      a = 0; 
    }
    
    //if (client.connect(server, 80)) {
    
      if(getSwitch){
        cosmPut3(localFeedID, 0, totalUsage, 4, happiness, 5, a, "routineUpdate");
        getSwitch = false;
      }
      else{
        cosmGet(avgFeed,happinessStream,"getHappiness", "get"); 
        getSwitch = true;
     }
        
    lastAttempMillis = millis();
  }
  
  if(millis() - lastAttempMillis > waitingLimit && isReading){
    Serial.print("?");
    finishReading();
  }
  
  // if didn't get 200 in 5 tries
  if(millis() - last200Millis > check200Interval*5){
    state = 0;
    wiflyFailure ++;
    if(wiflyFailure > maxWiflyFailure){
      Serial.println(F("can't connect to router, try reseting...."));
      forceReset(); 
    }
  }
}





