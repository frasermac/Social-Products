void checkResponse(){
  char c = client.read();
  if(report)Serial.print(c);

  buff[pointer] = c;
  if (pointer < 64){ 
    pointer++;
  }
  else{
    //Serial.print("-"); 
  }

  if (c == ',' || c == '}' || c == '\n'){ 
    //if(report)Serial.println(buff);

    // just check this for connection
    if(socketServer){
      found = strstr(buff, "\"status\":200");
    }
    else{
      found = strstr(buff, "200 OK");
    }

    if (found != 0){
      Serial.print(F("."));
      found200 = true;
      last200Millis = millis();
      if(state == WAIT_HAPPINESS){
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

    found = strstr(buff, "\"status\":401");
    if (found != 0){
      Serial.print(F("_"));
      found200 = false;
    }

    // since we now only request for 1 stream each time (depends on which state, we can just get the current value straight away
    found = strstr(buff, "current_value");
    if (found200 && found != 0){
      foundCurrentV = true; 
      tempRemoteValue = buffToFloat(17, (pointer-2) - 17 );

      if(state == MAIN_LOOP){       
        happiness = tempRemoteValue;
        //        Serial.println(buff);
        //        Serial.print(F("happiness = "));
        Serial.print(tempRemoteValue);
        foundCurrentV = false; 

      }
      else if(state == READ_HAPPINESS){

        happiness = tempRemoteValue;
        Serial.print(F("received last happiness = "));
        Serial.println(tempRemoteValue);
        foundCurrentV = false; 
        state = COMPARE_VAR;
        failure = 0;

      }
      else if(state == READ_USAGE){

        remoteTotalUsage = int(tempRemoteValue);
        Serial.print(F("received last total usage:"));
        Serial.println(remoteTotalUsage);
        state ++; 
        failure = 0;
      }
      //found200 = false;

    }



    found = strstr(buff, "Content-Length");
    if (found200 && found != 0){
      contentLength = int(buffToFloat(16, (pointer-1) - 16 ));
      if(report)Serial.print(F("content lenght: "));
      if(report)Serial.println(contentLength);
      //found200 = false;
    }

    if(contentLength == 1){
      finishReading();
      if(report)Serial.println(F("done!!!!"));
      Serial.print(",");
    }
    else if( contentLength > 1 ){
      found = strstr(buff, "}");
      if( found != 0 ){
        finishReading();
         if(report)Serial.println(F("done!!!!"));
         Serial.print(",");
      }
    }



    clean_buffer();

  }


}


void finishReading(){

  found200 = false;
  contentLength  = 0;
  isReading = false;
  if(!socketServer)client.stop();
  //Serial.print(",");
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



















