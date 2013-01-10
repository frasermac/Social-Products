void cosmSocketSub(long feed, int stream, String token){

  //Serial.println("cosmSocketSub");
  client.println(F("{"));
  client.println(F("\"method\" : \"subscribe\","));
  client.print(F("\"resource\" : \"/feeds/"));
  client.print(feed);
  if(stream > -1){
    client.print(F("/datastreams/"));
    client.print(stream);
  }
  client.println(F("\","));
  client.println(F("\"headers\" :"));
  client.println(F("{"));
  client.print(F("\"X-ApiKey\" : \""));
  client.print(API);
  client.println(F("\""));
  client.println(F("},"));
  client.print(F("\"token\" : \""));
  client.print(token);
  client.print(toasterID);
  client.println(F("\""));
  client.println(F("}"));
  client.println();

}



void cosmSocketPut3(long feed, int stream1, int data1, int stream2, int data2, int stream3, int data3, String token){
  Serial.print(F("+"));
  //Serial.println("cosmSocketPut");
  client.println(F("{"));
  client.println(F("\"method\" : \"put\","));
  client.print(F("\"resource\" : \"/feeds/"));
  client.print(feed);
  client.println(F("\","));
  client.println(F("\"params\" : {},"));
  client.print(F("\"headers\" : {\"X-ApiKey\":\""));       
  client.print(API);
  client.println(F("\"},"));
  client.println(F("\"body\" :"));  
  client.println(F("{"));
  client.println(F("\"version\" : \"1.0.0\","));
  client.println(F("\"datastreams\" : ["));

  // each stream
  client.print(F("{\n\"id\" : \"")); 
  client.print(stream1);
  client.print(F("\",\n\"current_value\" : \""));
  client.print(data1);
  client.print(F("\"\n}"));
  
  if(stream2 > -1){
    
    client.println(F(",")); // sep 

    client.print(F("{\n\"id\" : \"")); 
    client.print(stream2);
    client.print(F("\",\n\"current_value\" : \""));
    client.print(data2);
    client.print(F("\"\n}"));
    
  }
  
  if(stream3 > -1){
    
    client.println(F(",")); // sep 
  
    client.print(F("{\n\"id\" : \"")); 
    client.print(stream3);
    client.print(F("\",\n\"current_value\" : \""));
    client.print(data3);
    client.print(F("\"\n}"));
  
  }
  client.println(F("]"));
  client.println(F("},"));
  client.print(F("\"token\" : \""));
  client.print(token);
  client.print(toasterID);
  client.println(F("\""));
  client.println(F("}"));
  client.println();

}


void cosmSocketGet(long feed, int stream, String token){
  Serial.println(F("cosmSocketGet"));
  client.println(F("{"));
  client.println(F("\"method\" : \"get\","));
  client.print(F("\"resource\" : \"/feeds/"));
  client.print(feed);
  if(stream > -1){
    client.print(F("/datastreams/"));
    client.print(stream);
  }
  client.println(F("\","));
  client.println(F("\"headers\" :"));
  client.println(F("{"));
  client.print(F("\"X-ApiKey\" : \""));
  client.print(API); 
  client.println(F("\""));
  client.println(F("},"));
  client.print(F("\"token\" : \""));
  client.print(token);
  client.print(toasterID);
  client.println(F("\""));
  client.println(F("}"));

}










