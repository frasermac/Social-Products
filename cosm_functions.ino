void cosmSocketSub(long feed, int stream, String token){

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

void cosmToasterPut(int input0, int input1, int input2, int input3, int input4){

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

void cosmSocketPut(int stream, float data){

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

void cosmSocketGet(long feed){

  client.println("{");
  client.println("\"method\" : \"get\",");
  client.print("\"resource\" : \"/feeds/");
  client.print(feed);
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

void checkConnection(){
  //send fake update every check200Interval
  if(millis() - lastAttempMillis > check200Interval){
    lastAttempMillis = millis();
    cosmSocketPut(4, int(random(20)));
  }
  
  // if didn't get 200 in 5 tries
  if(millis() - last200Millis > check200Interval*5){
    forceReset();
  }
}


void startUp(){
  
  
  
  
}

