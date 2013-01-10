void adjustValue(){

  totalUsage = max(localTotalUsage, remoteTotalUsage); // use the larger one
  if(localTotalUsage != remoteTotalUsage){
    Serial.println(F("difference in totalUsage, use the larger one")); 
  }

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
      Serial.println(F("I'm sleeping now"));
      sleepRad = PI/2;
    }
    sleeping = true;
    ledMode = 4;
  }
  else{
    if(sleeping){
      Serial.println(F("just woke up"));
    }
    sleeping = false;
    ledMode = 2;
  } 

}

// checkConnection() sends a fake update to the toaster's feed every 'check200Interval' milliseconds
void checkConnection(){
  int a = 0;
  if(millis() - lastAttempMillis > check200Interval){
    if(toastInProgress){
      a = 1;
    }
    else{
      a = 0; 
    }
    cosmSocketPut3(localFeedID, 0, totalUsage, 4, happiness, 5, a, "routineUpdate");
    lastAttempMillis = millis();
  }

  // if didn't get 200 in 5 tries
  if(millis() - last200Millis > check200Interval*5){
    forceReset();
  }
}




