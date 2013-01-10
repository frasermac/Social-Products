

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

  if(ledMode == 1){
    ledCounter ++; 
    if(ledCounter == 300){
      ledCounter = 0;
      if (toggle2){
        analogWrite(ledPin,100);
        toggle2 = 0;
      }
      else{
        analogWrite(ledPin,0);
        toggle2 = 1;
      }
    }
  }
  else if(ledMode == 2){

    analogWrite(ledPin,80);

  }
  else if(ledMode == 3){

    analogWrite(ledPin,255);

  }
  else if(ledMode == 4){
    sleepRad += 0.002;
    ledBrightness = 40 + sin(sleepRad)*40;
    analogWrite(ledPin,ledBrightness);

  }

}

