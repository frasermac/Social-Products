#include <Servo.h> 
Servo myservo;

boolean servoPrint = false;

int movement[3][11] = {

  {
    5,5000,45,500,90,2000,90,500,45,300,0          }
  , // first digit is number of step, time to, destination, time to, destination, so on..
  {
    4,1000,90,200,45,5000,90,1000,0,999,999          } // this case we dont use the last 2, because there r 4 steps
  ,
  {
    5,300,30,300,0,300,30,300,45,500,0          }

};

int currentServoStep = 0; //to keep track where are we
float currentServoAngle = 0; // to keep track of current angle
int servoInterval = 50; // move every 50 millis
long lastServoMillis = millis();
int totalServoFrame = 0; // to be calculated
int servoFrameCount = 0;
float servoMoveStep = 0; // to be calculated
boolean servoRunning = false;
int servoMode;

void setupServo(){

  myservo.attach(servoPin);
  myservo.write(currentServoAngle);
  delay(2000);
}



void moveServo(){

  // if reach destination, change stage and recalculate, else keep moving  

  if(servoRunning && millis() - lastServoMillis > servoInterval){
    lastServoMillis = millis();

    if( reachDestAngle() ){

      currentServoStep ++;


      if(currentServoStep == movement[servoMode][0]){
        servoRunning = false; 
        if(servoPrint)Serial.println(F("finished"));
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
  calServo();

}

void stopServo(){
  servoRunning = false; 
}

void calServo(){

  int destAngle = movement[servoMode][2 + currentServoStep*2];
  float diff = destAngle - currentServoAngle;
  totalServoFrame = movement[servoMode][1 + currentServoStep*2] / servoInterval;
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












