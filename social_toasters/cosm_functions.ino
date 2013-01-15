//
//
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

void cosmNormalGet(long feed, int stream){
  Serial.print(F("<"));

  client.print(F("GET /v2/feeds/"));  // APIV2
  client.print(feed);
  client.print(F("/datastreams/"));
  client.print(stream);
  client.println(F(".json HTTP/1.1"));
  client.println(F("Host: api.cosm.com"));
  client.print(F("X-ApiKey: "));
  client.println(API);
  client.println();
  //pachubeReading = true;
  //wdt_reset();
}

void cosmNormalPut3(long feed, int stream1, int data1, int stream2, int data2, int stream3, int data3){

  Serial.print(F(">"));
  sprintf(buff,"%d,%d\n%d,%d\n%d,%d", 
  stream1, data1, stream2, data2, stream3, data3
    );
  //Serial.print(buff);

  //Serial.println(buff);
  client.print(F("PUT /v2/feeds/"));  // APIV2
  client.print(feed);
  client.println(F(".csv HTTP/1.1"));
  client.println(F("Host: api.cosm.com"));
  client.print(F("X-ApiKey: "));
  client.println(API);
  //client.println("User-Agent: Arduino (WiFly RN-XV)");
  //client.println("Content-Type: text/csv");
  client.print(F("Content-Length: "));
  client.println(strlen(buff));
  client.println(F("Connection: close"));
  client.println();
  client.print(buff);
  client.println();





}

void cosmSocketGet(long feed, int stream, String token, String method){
  //Serial.println(F("cosmSocketGet"));
  Serial.print(F("<"));
  client.println(F("{"));
  client.print(F("\"method\" : \""));
  client.print(method);
  client.println(F("\","));
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


void cosmGet(long FEED, int stream, String token, String method){
  if(socketServer){
    cosmSocketGet(FEED, stream, token, method);
  }
  else{
    client.stop();
    isReading = true;
    if (client.connect(server, port)) { //connect first then send  
      cosmNormalGet(FEED, stream);
    } 
  }
}


void cosmPut3(long feed, int stream1, int data1, int stream2, int data2, int stream3, int data3, String token){
  if(socketServer){
    cosmSocketPut3(feed, stream1, data1, stream2, data2, stream3, data3, token);
  }
  else{
    client.stop();
    isReading = true;
    if (client.connect(server, port)) { //connect first then send  
      cosmNormalPut3(feed, stream1, data1, stream2, data2, stream3, data3);;
    } 
  }
  
  
}









