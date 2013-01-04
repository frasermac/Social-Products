int lastYY, lastMM, lastDD, lastHH, lastMN, lastSS;
int currentYY, currentMM, currentDD, currentHH, currentMN, currentSS; 


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
