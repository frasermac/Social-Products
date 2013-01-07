void adjustValue(){

  totalUsage = max(localTotalUsage, remoteTotalUsage); // use the larger one
  if(localTotalUsage != remoteTotalUsage){
    Serial.println(F("difference in totalUsage, use the larger one")); 
  }

}


void expressEmotion(){

  if(millis() - lastEmotionMillis > emotionInterval){
    lastEmotionMillis = millis();
    // express emotion depends on happiness
    // startServo(2);
  } 


}

int resistCal(){
  
  if(happiness < angryTres){
   return (7 - happiness); 
  }else{
   return 0; 
  }
  
}
