void checkResponse(){
  char c = client.read();
  //Serial.print(c);
  buff[pointer] = c;
  if (pointer < 64){ 
    pointer++;
  }else{
   //Serial.print("-"); 
  }
  
  if (c == ',' || c == '}'){ 
    Serial.println(buff);

    // just check this for connection
    found = strstr(buff, "\"status\":200");
    if (found != 0){
      Serial.println(F("."));
      last200Millis = millis();
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


