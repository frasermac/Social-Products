
char tmpSSID[26];
char tmpPassphrase[26];
int endLineCode = 44; // ","
int startConfigCode = 47;
int tmpIndex = 0;
int tempReadVal;
int localUsageAddress = 53; // 53 and 54

void serialFeedback(){

  int inByte = Serial.read();
  Serial.print(char(inByte));
  switch (networkConfigState){

  case 0:
    // any caractor will trigger to 1
    networkConfigState++;
    state = NETWORK_CONFIG;
    //Serial.println(F("network config started, please input SSID"));
    break;

  case 1:
    if(inByte != endLineCode){
      tmpSSID[tmpIndex] = inByte;
      tmpIndex ++;
    }
    else{
      tmpSSID[tmpIndex] = '\0';
      tmpIndex = 0;
      Serial.print(F("received ssid: "));
      Serial.print(tmpSSID);
      for (int i = 0; i < 26; i++){
        EEPROM.write(i, tmpSSID[i]);
      }
      networkConfigState ++;
      //Serial.println(F("please input passphrase"));
    }
    break;


  case 2:

    if(inByte != endLineCode){
      tmpPassphrase[tmpIndex] = inByte;
      tmpIndex ++;

    }
    else{

      tmpPassphrase[tmpIndex] = '\0';
      tmpIndex = 0;
      Serial.print(F("received passphease: "));
      Serial.print(tmpPassphrase);
      for (int i = 0; i < 26; i++){
        EEPROM.write(i+26, tmpPassphrase[i]);
      }
      //Serial.println(F("config saved, restarting...."));
      forceReset(); 
    }

    break;

  }



}


void readEEPROM(){

  Serial.print(F("reading from EEPROM, "));
  for (int i = 0; i < 52; i++){
    tempReadVal = EEPROM.read(i);


    if(i<26){
      ssid[i] = char(tempReadVal);
    }
    else{
      passphrase[i-26] = char(tempReadVal);
    }

  }
  Serial.print(F("ssid:"));
  Serial.print(ssid);
  Serial.print(F(", passphrase:"));
  Serial.print(passphrase);


  int r1= EEPROM.read(localUsageAddress);
  int r2= EEPROM.read(localUsageAddress+1);
  if(r1 == 255 && r2 == 255){
    localTotalUsage = 0;
    totalUsage = 0;
    saveLocalUsage();
  }else{
    localTotalUsage = r1*256 + r2;
  }
  
  Serial.print(F(", local usage:"));
  Serial.println(localTotalUsage);
  
//  Serial.print(F("    "));
//  Serial.print(r1);
//  Serial.print(F(", "));
//  Serial.println(r2);

}


void saveLocalUsage(){

  int r1 = totalUsage/256;
  int r2 = totalUsage - (r1*256);
  EEPROM.write(localUsageAddress, r1);
  EEPROM.write(localUsageAddress+1, r2);
  Serial.println(F("totalUsage saved"));
  
}





