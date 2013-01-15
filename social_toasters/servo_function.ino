#include <Servo.h> 
Servo myservo;


int svHighest = 180; // same highest point for both side
int svLowest = 0; // these numbers are for left sided one, the right sided one need to be flipped
// so actually the range will be from lowest to highest, // percentage? then we re-map on the 
int servoMode;
const int movementModeNo = 4;
const int movementStepNo = 12;
int movement[movementModeNo][movementStepNo] = { // first digit is number of step, time to, destination(0-100), time to, destination(0-100), so on..


  {
    4,  1,  300,  80,  300,  100,  300,  80,  300,  100,  999,  99        } // wiggle
  , 
  {
    2,  9,  150,  80,  150,  40,  150,  50,  150,  60,  150,  40         } // 0,1,2 super pissed
  ,
  {  
    4,  2,  1000,  30,  300,  50,  300,  30,  500,  100,  999,  99         } // 3,4 moderately pissed
  ,
  {  
    3,  1,  1000,  75,  1000,  50,  1000,  100,  999,  99,  999,  99        }// 5,6 slightly pissed

};

int currentServoStep = 0; //to keep track where are we
float currentServoAngle = svHighest; // to keep track of current angle
int servoInterval = 50; // move every 50 millis
long lastServoMillis = millis();
int totalServoFrame = 0; // to be calculated
int servoFrameCount = 0;
int servoRoundCount = 0;
float servoMoveStep = 0; // to be calculated


void setupServo(){
  mapServo(); // re calculate pattern depends on servo is on the left or right
  currentServoAngle = svHighest;
  myservo.attach(servoPin);
  myservo.write(currentServoAngle);
  delay(2000);
  myservo.detach();
  
}


void mapServo(){
  if(!servoLeft){
    svLowest = 180;
    svHighest = 0;
  }else{
    svLowest = 0;
    svHighest = 180;
  }

  for(int i=0; i< movementModeNo; i++){
    
    if(servoPrint){
      Serial.print(F("servoPattern:"));
      Serial.print(i);
       Serial.print(F("  "));
    }

    for(int j=3; j< movementStepNo; j+=2){
      movement[i][j] = map(movement[i][j], 0, 100, svLowest, svHighest); 
      if(servoPrint){
        Serial.print(movement[i][j]);
        Serial.print(F(", "));
      }
    }
    
    if(servoPrint)Serial.println();
  }
}



void moveServo(){

  // if reach destination, change stage and recalculate, else keep moving  

  if(servoRunning && millis() - lastServoMillis > servoInterval){
    lastServoMillis = millis();

    if( reachDestAngle() ){

      currentServoStep ++;


      if(currentServoStep == movement[servoMode][0]){
        servoRoundCount ++;
        if(servoRoundCount == movement[servoMode][1]){
          Serial.print(F("a"));
          stopServo();
        }else{
          currentServoStep = 0;
          calServo();
          Serial.print(F("r"));
        }
      }
      else{
        calServo();
      }

    }
    else{

      currentServoAngle += servoMoveStep;
      myservo.write(currentServoAngle);
      //Serial.print(currentServoAngle);
      servoFrameCount ++;
    }


  }


}

void startServo(int mode){
  Serial.print("start servo mode: ");
  Serial.println(mode);
  servoMode = mode;
  servoRunning = true; 
  currentServoStep = 0;
  servoRoundCount = 0;
  calServo();
  myservo.attach(servoPin);
}

void stopServo(){
  myservo.write(svHighest);
  delay(500);
  servoRunning = false; 
  servoRoundCount = 0;
  myservo.detach();
}


void calServo(){

  int destAngle = movement[servoMode][3 + currentServoStep*2];
  float diff = destAngle - currentServoAngle;
  totalServoFrame = movement[servoMode][2 + currentServoStep*2] / servoInterval;
  servoMoveStep = diff/totalServoFrame;
  servoFrameCount = 0;
  if(servoPrint){
    Serial.print(F("destAngle = "));
    Serial.print(destAngle);
    Serial.print(F(", diff = "));
    Serial.print(diff);
    Serial.print(F(", totalServoFrame = "));
    Serial.print(totalServoFrame);
    Serial.print(F(", servoMoveStep = "));
    Serial.print(servoMoveStep);
    Serial.println();
  }

}

boolean reachDestAngle(){

  if(servoFrameCount >= totalServoFrame){
    if(servoPrint) Serial.println("reach destination");
    return true; 
  }
  else{
    //Serial.print(",");
    return false; 
  }

}















