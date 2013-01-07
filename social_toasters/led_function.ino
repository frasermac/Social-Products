
long lastLedOnMillis = 0;
long lastLedOffMillis = 0;
boolean ledOn = false;
long ledOnInterval[] = {0,500,5000,100};
long ledOffInterval[] = {5000,500,0,100};



void setupLed(){
  
  pinMode(ledPin, OUTPUT);  
  
  
}


void ledControl(){
  if(ledOn == true){
    
    if(millis() - lastLedOnMillis > ledOnInterval[ledMode] ){
      ledOn = false;
      digitalWrite(ledPin, LOW);
      lastLedOffMillis = millis();
      //Serial.println("off");
    }
    
  }else{
    
    if(millis() - lastLedOffMillis > ledOffInterval[ledMode] ){
      ledOn = true;
      digitalWrite(ledPin, HIGH);
      lastLedOnMillis = millis();
      //Serial.println("on");
    }
  }  
  
  
}
