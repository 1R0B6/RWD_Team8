//Version = string "0.7"
//Use Ard Leonardo for Pro Micro
/*
0.6 Release for July Competition
  Need to reduce timeout from loops from 10 seconds to lower
    700 ms
  Fix mapping of user controlled lowering speed value
    Changed from 1400 to 1425
  Should hard code speed going up as 2000 for both trigger & selfright()
  Hard code arm going down speed to 1425 for the trigger function
  Need to fix the "going up" function to allow overshoot or tolerance
    Decreased timeout see if this is enough
      Determine if trim of arm is working
    Fix reverse drive function
      Attempting Karl's version
//        or implement spin function
//        and/or spin function if SWD == 2000 && SWA == 2000
  Make it so if calibration fails it exits the loop and disables selfright & trigger??
    Fix the stuck in loop while arm is disabled
      Changed drive values

Channel Mapping
Ch1->Drive Left/Right
Ch2->Drive Fwd/Bck
Ch3->Manual Arm control
Ch4->Activator for manual arm control
Ch5->VRA (safe shutdown to raspberry pi) in combination with other switches
Ch6->VRB (Speed control)
Ch7->SWA Trigger for Arm
Ch8->SWB Self-righting routine
Ch9->SWC unit disable (3 levels)
Ch10->SWD inverse drive commands
*/

#include "FlySkyIBus.h"
#include <Servo.h>
/*Global Variables*/
//#define DEBUG 2 //Recommend using 3-5 during competition
#define MOTORS_ON 1 //Must be 1 for Servo output and 0 for bench debug
  unsigned long prev = 0;
  unsigned long cur = 0;
  boolean dFlag = false;
  const long interval = 500; //Debug print interval/mode update


/*
 * DEBUG 5=Critical
 * DEBUG 4=Error
 * DEBUG 3=Warning
 * DEBUG 2=Info
 * DEBUG 1=Debug
*/
//RX Variables
int  ch1_in;
int  ch2_in;
int  ch3_in;
int  ch4_in;
int  VRA;
int  VRB;
int  SWA;
int  SWB;
int  SWC;
int  SWD;

//Mode
boolean driveActive = false;
boolean armActive = false;
//boolean enableServos = false;
boolean enableServos = true;
byte rLED = 7;
byte gLED = 8;
byte bLED = 9;
int loopTimeouts = 1000;//500;

//Powertrain
byte limiter = 100;
Servo leftDrive;
Servo rightDrive;
short left_in;
short right_in;
int left_out = 1500;
int right_out = 1500;
short change_debounce = 4;
bool reverseDrive = false;
bool l_FWD = true;
bool r_FWD = true;

//Offensive
Servo offensive;
short off_in;
short off_out = 1500;
bool lowerLimit = false;
bool upperLimit = false;
bool selfRightDisable = true;
bool armIsUp = false;
byte lowSwitch = 10;
byte upSwitch = 16;
byte armFeedback = A0;
int armLocation = 0;
int armAtLow = 0;
int armAtLowPerm = 0;
int armOffset = 0;
int armAtHigh = 0;
int armAtLifted = 0;  //What gives??
int armAtFlip = 0;
int calUpSpeed = 1570;
int calDownSpeed = 1430;
int delayAtFlip = 500; //in milliseconds
int delayInLoop = 50;
int hardBuffer = 50;
bool calFailed = false;

void setup() {
  pinMode(lowSwitch, INPUT_PULLUP);
  pinMode(upSwitch, INPUT_PULLUP);
  pinMode(armFeedback, INPUT); 
  pinMode(rLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(bLED, OUTPUT);
  digitalWrite(rLED, HIGH);   //Make LED WHITE
  digitalWrite(gLED, HIGH);
  digitalWrite(bLED, HIGH);
  IBus.begin(Serial1);
  #if MOTORS_ON == 1
    rightDrive.attach(6);
    leftDrive.attach(3);
    offensive.attach(5);
    rightDrive.writeMicroseconds(right_out);
    leftDrive.writeMicroseconds(left_out);
    offensive.writeMicroseconds(off_out);
  #else
    Serial.begin(115200);
    Serial.println("test");
  #endif
  while(true){
    IBus.loop();
    channel_update();
    delay(100);
    if(SWC == 1000){
      #if MOTORS_ON == 0
        Serial.println("Transmitter data received, waiting to calibrate arm");
      #endif
      digitalWrite(rLED, LOW);    //Make LED BLUE
      digitalWrite(gLED, LOW);
      digitalWrite(bLED, HIGH);
      calibrateArm();
      break;
    }
      #if MOTORS_ON == 0
        Serial.println("RX conditions not met");
      #endif
  }
}

void loop() {
  IBus.loop();                                        //Read in UART
  channel_update();                                   //Set local variables
  cur = millis();
  if (cur - prev >= interval) {
    prev = cur;
    #if MOTORS_ON == 0
      dFlag = true;
      printCH();
    #endif
    orientation();
    modeSelect();
    armAdjustment();
    operationPerc();
    
  }
    
  driveValues();
  armLocation = analogRead(armFeedback);
//  if(armLocation > armAtLow){
//    armIsUp = true;
//  }
////  else if(armLocation < (armAtLow-50)){
//  else{
//    armIsUp = false;
//  }
  #if MOTORS_ON == 0
    if(dFlag == true){
      Serial.print(",ArmLoc=");
      Serial.print(armLocation);
    }
  #endif
  
  if(ch4_in < 1200){ //determine if manual control of arm
    offensiveValue();
  }
  else{ //determine if self righting of arm
    #if MOTORS_ON == 1
      offensive.writeMicroseconds(1500);
    #endif
    if(SWB == 2000 && selfRightDisable == false && calFailed == false){
      #if MOTORS_ON == 0
        Serial.println("");
        Serial.println("SELF RIGHTING");
      #endif
      selfRight();  //Test timeout
    }
    else if(SWB == 1000){
      selfRightDisable = false;
    }
    if(SWA == 1000 && armIsUp == false && calFailed == false){
      Serial.println();
      Serial.println("Calling Up Sequence");
      triggerArm(true);
      //add a timeout measurement
    }
    else if (SWA == 2000 && armIsUp == true && calFailed == false){
      Serial.println();
      Serial.println("Calling Down Sequence");
      triggerArm(false);
      //add a timeout measurement
    }
  }

  #if MOTORS_ON == 0
    if(dFlag == true){
      Serial.println(",");
      dFlag = false;
    }
  #endif
}
