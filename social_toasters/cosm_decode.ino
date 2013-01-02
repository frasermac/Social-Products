void checkResponse(){
  char c = client.read();
  //Serial.print(c);
  buff[pointer] = c;
  if (pointer < 64){ 
    pointer++;
  }
  else{
    //Serial.print("-"); 
  }

  if (c == ',' || c == '}'){ 
    //Serial.println(buff);

    // check current time, just check this at start up (state == 2)
    if(state == 2){
      found = strstr(buff, " GMT\"");
      if (found != 0){
        Serial.println(buff);
        decodeCurrentTime();
      }
      found = strstr(buff, "at\":\"2");
      if (found != 0){
        Serial.println(buff);
        decodeLastUpdateTime();
      }
      
    }

    // just check this for connection
    found = strstr(buff, "\"status\":200");
    if (found != 0){
      Serial.print(F("."));
      last200Millis = millis();
      if(state == 4){
        Serial.println(F("completed"));
        state ++; 
        failure = 0;
      }
    }







    found = strstr(buff, "current_value");
    if (found != 0){
      foundCurrentV = true; 
      //Serial.println("found currentV");
      int s = 17; //always start at buff[17]
      int l = (pointer-2) - s;
      char r[l+1] ;
      for(int i=0; i<l; i++){
        r[i] = buff[s+i];
      }
      r[l] = '\0';
      //Serial.println(buff);
      value = atof(r); // value from the actuall feed
    }


    if(foundCurrentV == true){ // look for id
      found = strstr(buff, "\"id\":\"");
      if (found != 0){
        int s = 6; //always start at buff[17]
        int l = (pointer-2) - s;
        char r[l+1] ;
        for(int i=0; i<l; i++){
          r[i] = buff[s+i];
        }
        r[l] = '\0';
        //Serial.println(buff);
        //Serial.println("found ID");
        streamID = int(atof(r));
        Serial.print(F("id = ")); 
        Serial.print(streamID); 
        Serial.print(F("  currentValue = "));  
        Serial.println(value);
        remoteData[streamID] = value; 
        foundCurrentV = false; 

        if(state == 2 && streamID == 0){
          totalUsage = remoteData[streamID];
          Serial.print(F("received last total usage:"));
          Serial.println(totalUsage);
          state ++; 
          failure = 0;
        }
      }

    }


    clean_buffer();

  }


}

void clean_buffer() {
  pointer = 0;
  memset(buff,0,sizeof(buff)); 
  //foundCurrentV = false; 
}

void decodeCurrentTime(){

  // 02 Jan 2013 14:59:55 GMT

  currentDD = buffToInt(1, 2);
  currentMM = buffToMonth();
  currentYY = buffToInt(8, 4);
  currentHH = buffToInt(13, 2);
  currentMN = buffToInt(16, 2);
  currentSS = buffToInt(19, 2);

  Serial.print(F("current time is "));
  Serial.print(currentDD);
  Serial.print(F("/"));
  Serial.print(currentMM);
  Serial.print(F("/"));
  Serial.print(currentYY);
  Serial.print(F("  "));
  Serial.print(currentHH);
  Serial.print(F(":"));
  Serial.print(currentMN);
  Serial.print(F(":"));
  Serial.println(currentSS);
  

}

void decodeLastUpdateTime(){
  
  //"at":"2013-01-02T15:30:11.213622Z",
  lastDD = buffToInt(14, 2);
  lastMM = buffToInt(11, 2);
  lastYY = buffToInt(6, 4);
  lastHH = buffToInt(17, 2);
  lastMN = buffToInt(20, 2);
  lastSS = buffToInt(23, 2);
  
  Serial.print(F("last updated time is "));
  Serial.print(lastDD);
  Serial.print(F("/"));
  Serial.print(lastMM);
  Serial.print(F("/"));
  Serial.print(lastYY);
  Serial.print(F("  "));
  Serial.print(lastHH);
  Serial.print(F(":"));
  Serial.print(lastMN);
  Serial.print(F(":"));
  Serial.println(lastSS);

}

int buffToInt(int s, int l){
  char r[l+1] ;
  for(int i=0; i<l; i++){
    r[i] = buff[s+i];
  }
  r[l] = '\0';
  return int(atof(r));  
}

int buffToMonth(){
  char *found;

  found = strstr(buff, "Jan");
  if(found != 0 ){
    return 1; 
  }

  found = strstr(buff, "Feb");
  if(found != 0 ){
    return 2; 
  }

  found = strstr(buff, "Mar");
  if(found != 0 ){
    return 3; 
  }

  found = strstr(buff, "Apr");
  if(found != 0 ){
    return 4; 
  }

  found = strstr(buff, "May");
  if(found != 0 ){
    return 5; 
  }

  found = strstr(buff, "Jun");
  if(found != 0 ){
    return 6; 
  }

  found = strstr(buff, "Jul");
  if(found != 0 ){
    return 7; 
  }

  found = strstr(buff, "Aug");
  if(found != 0 ){
    return 8; 
  }

  found = strstr(buff, "Sep");
  if(found != 0 ){
    return 9; 
  }

  found = strstr(buff, "Oct");
  if(found != 0 ){
    return 10; 
  }

  found = strstr(buff, "Nov");
  if(found != 0 ){
    return 11; 
  }

  found = strstr(buff, "Dec");
  if(found != 0 ){
    return 12; 
  }

  return 0;


}






