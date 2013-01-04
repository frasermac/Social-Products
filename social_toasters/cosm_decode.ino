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
      value = buffToInt(17, (pointer-2) - 17 );
    }


    if(foundCurrentV == true){ // look for id
      found = strstr(buff, "\"id\":\"");
      if (found != 0){
        streamID = buffToInt(6, (pointer-2) - 6 );
        foundCurrentV = false; 
        
        if(report){
          Serial.print(F("id = ")); 
          Serial.print(streamID); 
          Serial.print(F("  currentValue = "));  
          Serial.println(value);
        }
        
        if(streamID == 4){
          happiness = value;
          Serial.print(F("happiness = "));
          Serial.println(happiness);
        }

        if(state == 2 && streamID == 0){
          remoteTotalUsage = value;
          Serial.print(F("received last total usage:"));
          Serial.println(remoteTotalUsage);
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
}



int buffToInt(int s, int l){
  char r[l+1] ;
  for(int i=0; i<l; i++){
    r[i] = buff[s+i];
  }
  r[l] = '\0';
  return int(atof(r));  
}









