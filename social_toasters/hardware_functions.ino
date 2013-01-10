// WiFlyStartup() initiates the WiFly modules at boot
void WiFlyStartup(){
  
  
  char ssid[] = "CEL2"; // network name
  char passphrase[] = "j3llyf1sh"; // password
  
  //wdt_enable(WDTO_8S); // start WDT to 8 secs
  Serial.print(F("WiFly begin.."));
  WiFly.begin();    // startup the WiFly

  Serial.print(F("joining "));
  Serial.println(ssid);
  // Join the WiFi network
  if (WiFly.join(ssid, passphrase)) {

    Serial.print(F("joined, WiFly's IP is "));
    String IP = WiFly.ip();
    Serial.print(IP);
    if(IP.length() < 9){
      Serial.println(F(" bad IP, restarting.."));
      WiFlyStartup();
    }
    else{
      Serial.println(F(" OK.."));
    }
    //wdt_disable(); //disable WDT

  }
  else{
    Serial.print(F(".. fail, try again "));
    wiflyFailure ++;
    if(wiflyFailure > maxWiflyFailure){
      Serial.println(F("can't connect to router, try reseting...."));
      forceReset(); 
    }
    WiFlyStartup();
  } 

}

// forceReset() brings the rest pin to ground, which restarts the Arduino
void forceReset(){
  
  Serial.println(F("lost connection to Cosm too long, try reseting...."));
  delay(500);
  pinMode(arduinoResetPin, OUTPUT);
  
}

void readSD(){
  Serial.print(F("reading from SD card.."));
  localTotalUsage = 0;
//  for(int k=0; k<SIZE; k++){
//   usageTrack[k] = 0; 
//  }
//  ssid[] = "CEL2";
//  passphrase[] = "j3llyf1sh";
  Serial.println(F("done"));
}



void cutPower(){
  Serial.println("cut power");
  powerAllow = false;
  lastRelayMillis = millis();
}

void powerControl(){
  if(!powerAllow){
    if(millis() - lastRelayMillis > relayInterval){
      powerAllow = true;
      Serial.println("back on");
      digitalWrite(relayPin, LOW);
    }
    else{
      digitalWrite(relayPin, HIGH);
    }
  }

}
