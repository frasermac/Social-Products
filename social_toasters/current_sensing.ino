/*
// 
 ////////////////////////////////////////////////////////////////////////////
 //
 //  void setupCurrentCoil:
 //    Sets the necessary analog pin as an input, zeroes the array used for 
 //    averaging out consecutive coil readings, and calibrates the pin using
 //    the Energy Monitor library.
 //
 */

void setupCurrentCoil() {
  pinMode(coilAnalogInputPin, INPUT); 

  for (int a = 0; a < numReadings; a++) {  // zero the current coil's averaging array
    currentSense[a] = 0;  
  }
}



/*
// 
 ////////////////////////////////////////////////////////////////////////////
 //
 //  void isToasting:
 //    Reads the current coil's array of read values for an average, returns true/false
 //    depending on lever status, and activates the boolean toastInProgress during the 
 //    course of a toast   
 //
 */

void checkCurrent(){
  currentSense[currentCoilIndex] = emon1.calcIrms(1480);    // Add an element to the current sample array
  toastInProgress = isToasting();

  currentCoilIndex++;
  if (currentCoilIndex >= numReadings) {
    currentCoilIndex = 0;
  }  
}

boolean isToasting() {
  int arraySum = 0;
  int average = 0;

  for (int a = 0; a < numReadings; a++) { 
    arraySum += currentSense[a]; 
  }

  average = arraySum / numReadings;

//  Serial.print("Average: ");
//  Serial.println(average);
//  Serial.println();

  if (average >= 20) {
    if (toastInProgress == false) {
      toastInProgress = true;
    }
    return true;
  }
  else {
    if (toastInProgress == true) {
      toastInProgress = false;
    }
    return false;
  }
}

