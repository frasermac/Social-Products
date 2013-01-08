
long lastLedOnMillis = 0;
long lastLedOffMillis = 0;
boolean ledOn = false;
long ledOnInterval[] = {0,5000,500,100};
long ledOffInterval[] = {5000,0,500,100};



void setupLed(){
  
  cli();//stop interrupts
 
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS11 bit for 8 prescaler
  TCCR2B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  
  sei();//allow interrupts  
  
}


void ledControl(){
  if(ledMode == 0){
    analogWrite(ledPin, 0);
  }else if(ledMode == 1){
    analogWrite(ledPin, 80);
    
  }else{
  
    if(ledOn == true){
      
      if(millis() - lastLedOnMillis > ledOnInterval[ledMode] ){
        ledOn = false;
        analogWrite(ledPin, 0);
        lastLedOffMillis = millis();
        //Serial.println("off");
      }
      
    }else{
      
      if(millis() - lastLedOffMillis > ledOffInterval[ledMode] ){
        ledOn = true;
        analogWrite(ledPin, 255);
        lastLedOnMillis = millis();
        //Serial.println("on");
      }
    }  
  
  }
  
}



