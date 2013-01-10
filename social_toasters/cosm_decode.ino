void checkResponse(){
  char c = client.read();
  buff[pointer] = c;
  if (pointer < 64){ 
    pointer++;
  }
  else{
    //Serial.print("-"); 
  }

  if (c == ',' || c == '}'){ 
    if(report)Serial.println(buff);

    // just check this for connection
    found = strstr(buff, "\"status\":200");
    if (found != 0){
      Serial.print(F("."));
      found200 = true;
      last200Millis = millis();
      if(state == 6){
        Serial.println(F("completed"));
        state ++; 
        failure = 0;
      }
    }
    
    found = strstr(buff, "\"status\":400");
    if (found != 0){
      Serial.print(F("-"));
      found200 = false;
    }

    // since we now only request for 1 stream each time (depends on which state, we can just get the current value straight away
    found = strstr(buff, "current_value");
    if (found200 && found != 0){
      foundCurrentV = true; 
      tempRemoteValue = buffToFloat(17, (pointer-2) - 17 );

      if(state == 8){       
        happiness = tempRemoteValue;
        Serial.print(F("happiness = "));
        Serial.println(tempRemoteValue);
        foundCurrentV = false; 

      }
      else if(state == 4){

        happiness = tempRemoteValue;
        Serial.print(F("received last happiness = "));
        Serial.println(tempRemoteValue);
        foundCurrentV = false; 
        state ++;
        failure = 0;

      }
      else if(state == 2){

        remoteTotalUsage = int(tempRemoteValue);
        Serial.print(F("received last total usage:"));
        Serial.println(remoteTotalUsage);
        state ++; 
        failure = 0;
      }
      found200 = false;
    }

    clean_buffer();

  }


}



void clean_buffer() {
  pointer = 0;
  memset(buff,0,sizeof(buff)); 
}



int buffToInt(int s, int l){
  char r[l+1] ;
  for(int i=0; i<l; i++){
    r[i] = buff[s+i];
  }
  r[l] = '\0';
  return int(atof(r));  
}

float buffToFloat(int s, int l){
  char r[l+1] ;
  for(int i=0; i<l; i++){
    r[i] = buff[s+i];
  }
  r[l] = '\0';
  return atof(r);  
}













