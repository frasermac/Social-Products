void adjustValue(){

  totalUsage = max(localTotalUsage, remoteTotalUsage); // use the larger one
  if(localTotalUsage != remoteTotalUsage){
    Serial.println(F("difference in totalUsage, use the larger one")); 
  }

}


void expressEmotion(){

  if(millis() - lastEmotionMillis > emotionInterval){
    lastEmotionMillis = millis();
    lastActionMillis = millis();
    // express emotion depends on happiness
    // startServo(2);
  } 


}

int resistCal(){

  if(happiness < angryTres){
    return (7 - happiness); 
  }
  else{
    return 0; 
  }

}


void checkSleep(){
  if(millis() - lastActionMillis > sleepTimer){
    Serial.println(F("I'm sleeping now"));
    if(!sleeping){
      sleepRad = PI/2;
    }
    sleeping = true;
    ledMode = 4;
  }
  else{
    sleeping = false;
    ledMode = 2;
  } 



}



