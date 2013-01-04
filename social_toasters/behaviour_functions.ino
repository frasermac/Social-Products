void adjustValue(){
  
  totalUsage = max(localTotalUsage, remoteTotalUsage); // use the larger one
  if(localTotalUsage != remoteTotalUsage){
   Serial.println(F("difference in totalUsage, use the larger one")); 
  }
  
}
