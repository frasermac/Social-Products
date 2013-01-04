void cosmSocketSub(long feed, int stream, String token){

  //Serial.println("cosmSocketSub");
  client.println("{");
  client.println("\"method\" : \"subscribe\",");
  client.print("\"resource\" : \"/feeds/");
  client.print(feed);
  if(stream > -1){
    client.print("/datastreams/");
    client.print(stream);
  }
  client.println("\",");
  client.println("\"headers\" :");
  client.println("{");
  client.print("\"X-PachubeApiKey\" : \"");
  client.print(API);
  client.println("\"");
  client.println("},");
  client.print("\"token\" : \"");
  client.print(token);
  client.println("\"");
  client.println("}");
  client.println();

}

void cosmHistGet(){

  Serial.println("cosmSocketGet");
  client.println("{");
  client.println("\"method\" : \"get\",");
  client.print("\"resource\" : \"/feeds/91258/datastreams/5.json?start=2013-01-01T10:30:48Z&duration=1seconds&find_previous=true");


  client.println("\",");
  client.println("\"headers\" :");
  client.println("{");
  client.print("\"X-PachubeApiKey\" : \"");
  client.print(API); 
  client.println("\"");
  client.println("},");
  client.print("\"token\" : \"");
  client.print("getSchedule");
  client.println("\"");
  client.println("}");

}

void cosmToasterPut(int input0, int input1, int input2, int input3, int input4){

  Serial.println("cosmToasterPut");  
  client.println("{");
  client.println("\"method\" : \"put\",");
  client.print("\"resource\" : \"/feeds/");
  client.print(localFeedID);
  client.println("\",");
  client.println("\"params\" : {},");
  client.print("\"headers\" : {\"X-PachubeApiKey\":\"");       
  client.print(API);
  client.println("\"},");
  client.println("\"body\" :");  
  client.println("{");
  client.println("\"version\" : \"1.0.0\",");
  client.println("\"datastreams\" : [");

  // each stream
  client.print("{\n\"id\" : \""); 
  client.print(0);
  client.print("\",\n\"current_value\" : \"");
  client.print(input0);
  client.print("\"\n}");

  client.println(","); // sep 

  client.print("{\n\"id\" : \""); 
  client.print(1);
  client.print("\",\n\"current_value\" : \"");
  client.print(input1);
  client.print("\"\n}");

  client.println(","); // sep 

  client.print("{\n\"id\" : \""); 
  client.print(2);
  client.print("\",\n\"current_value\" : \"");
  client.print(input2);
  client.print("\"\n}");

  client.println(","); // sep 

  client.print("{\n\"id\" : \""); 
  client.print(3);
  client.print("\",\n\"current_value\" : \"");
  client.print(input3);
  client.print("\"\n}");

  client.println(","); // sep 

  client.print("{\n\"id\" : \""); 
  client.print(4);
  client.print("\",\n\"current_value\" : \"");
  client.print(input4);
  client.print("\"\n}");


  client.println("]");
  client.println("},");
  client.println("\"token\" : \"updateToasterData\"");
  client.println("}");
  client.println();


}

// cosmSocketPut takes a datastream number and a float value and puts it to the toaster's feed
void cosmSocketPut(long feed, int stream, float data){

  //Serial.println("cosmSocketPut");
  client.println("{");
  client.println("\"method\" : \"put\",");
  client.print("\"resource\" : \"/feeds/");
  client.print(feed);
  client.println("\",");
  client.println("\"params\" : {},");
  client.print("\"headers\" : {\"X-PachubeApiKey\":\"");       
  client.print(API);
  client.println("\"},");
  client.println("\"body\" :");  
  client.println("{");
  client.println("\"version\" : \"1.0.0\",");
  client.println("\"datastreams\" : [");

  // each stream
  client.print("{\n\"id\" : \""); 
  client.print(stream);
  client.print("\",\n\"current_value\" : \"");
  client.print(data);
  client.print("\"\n}");

  client.println("]");
  client.println("},");
  client.println("\"token\" : \"0x12345\"");
  client.println("}");
  client.println();

}


void cosmSocketGet(long feed, int stream){

  Serial.println("cosmSocketGet");
  client.println("{");
  client.println("\"method\" : \"get\",");
  client.print("\"resource\" : \"/feeds/");
  client.print(feed);
  if(stream > -1){
    client.print("/datastreams/");
    client.print(stream);
  }
  client.println("\",");
  client.println("\"headers\" :");
  client.println("{");
  client.print("\"X-PachubeApiKey\" : \"");
  client.print(API); 
  client.println("\"");
  client.println("},");
  client.print("\"token\" : \"");
  client.print("getSchedule");
  client.println("\"");
  client.println("}");

}



// checkConnection() sends a fake update to the toaster's feed every 'check200Interval' milliseconds
void checkConnection(){

  if(millis() - lastAttempMillis > check200Interval){
    cosmSocketPut(localFeedID, 0, totalUsage);
    Serial.print(F("+"));
    lastAttempMillis = millis();
  }

  // if didn't get 200 in 5 tries
  if(millis() - last200Millis > check200Interval*5){
    forceReset();
  }
}


void fakeToast(){

  if(millis() - fakeToastMillis > fakeToastInterval){
    fakeToastMillis = millis();
    totalUsage ++;
    cosmSocketPut(localFeedID, 0, totalUsage);
    Serial.print(F("toasting, total usage = "));
    Serial.println(totalUsage);
    lastAttempMillis = millis();
  } 

}



