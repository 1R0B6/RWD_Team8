//ch3 = manual offensive command while ch4<1200
//SWA = trigger for arm to raise to 15%
//SWB = self-right robot

void offensiveValue(){
  off_in = ch3_in;  
  
  //Protects motors from drastic changes. Change rate controlled by change_debounce on home page. (larger = slower)
  if(off_in < off_out){//Going down
    int diff = (off_out - off_in)/change_debounce;
    off_out = off_out - diff;
  }
  else{//Going Up
    int diff = (off_in - off_out)/change_debounce;
    off_out = off_out + diff;
  }

  //check limit switches
  if(digitalRead(lowSwitch) == false && off_out < 1500){
    off_out = 1500;
    #if MOTORS_ON == 0
      lowerLimit = true;
      upperLimit = false;
    #endif
  }
  else if(digitalRead(upSwitch) == false && off_out > 1500){
    off_out = 1500;
    #if MOTORS_ON == 0
      lowerLimit = false;
      upperLimit = true;
    #endif
  }
  #if MOTORS_ON == 0
    else{
      lowerLimit = false;
      upperLimit = false;
    }
  #endif

  off_out = constrain(off_out, 1425, 2000);

  if(armActive==false){
    off_out = 1500;
  }
  #if MOTORS_ON == 1
    if(armActive==true){
      offensive.writeMicroseconds(off_out);
    }
  #else
    if(dFlag == true){
      Serial.print(",Off I/O=");
      Serial.print(off_in);
      Serial.print(",");
      Serial.print(off_out);
      Serial.print(",Limit L/H=");
      Serial.print(lowerLimit);
      Serial.print(",");
      Serial.print(upperLimit);
    }
  #endif
}

void triggerArm(bool up){//Need to add DEBUG Prints. Note need a , at start & no comma at end
  if(up == true){
    //Ensure arm is less than "up" position
    if(armLocation < armAtHigh){
      prev = millis();
     //drive arm to up position 
     off_out = 2000;  //DEBUG CHANGE 2000 TO 1570
     #if MOTORS_ON == 1
      if(armActive == true){
        offensive.writeMicroseconds(off_out);
      }
     #endif
     while((analogRead(armFeedback) < armAtLifted) && digitalRead(upSwitch) == true){ //DEBUG ADD VRA TO armAtLifted
      armLocation = analogRead(armFeedback);      
//      armLocation = armAtLifted - armLocation;
//      off_out = map(armLocation, 0, armAtLifted, 1500, 2000);
//      off_out = constrain(off_out, 1500, 2000);
      #if DEBUG == 2
        Serial.print("Location:Speed ");
        Serial.print(armLocation);
        Serial.print(":");
        Serial.print(off_out);
      #endif
      delay(delayInLoop);
      cur = millis();
      if (cur - prev >= loopTimeouts) {
        break;
      }
     }
     #if MOTORS_ON == 1
      offensive.writeMicroseconds(1500);
     #endif
    }
    armIsUp = true;
    #if DEBUG == 2
      Serial.println();
      Serial.print("ARM is: ");
      Serial.print(armIsUp);
    #endif    
  }
  else if(up == false){
    //Ensure arm is greater than "down" + VRA shift
    if(armLocation > armAtLow){
      //drive arm to down position  
      off_out = 1425;
      #if MOTORS_ON == 1
        if(armActive == true){
          offensive.writeMicroseconds(off_out);
        }
      #endif
      prev = millis();
     //drive arm to up position 
     while((analogRead(armFeedback) > (armAtLow)) && (digitalRead(lowSwitch) == true)){
      armLocation = analogRead(armFeedback);
//      armLocation = armLocation - armAtLow;
//      off_out = map(armLocation, 0, armAtLifted, 1500, 1000);
//      off_out = constrain(off_out, 1000, 1500);
      #if DEBUG == 2
        Serial.print("Location:Speed ");
        Serial.print(armLocation);
        Serial.print(":");
        Serial.print(off_out);
      #endif
//      #if MOTORS_ON == 1
//        offensive.writeMicroseconds(off_out);
//      #endif
      delay(delayInLoop);
      cur = millis();
      if (cur - prev >= loopTimeouts) {
        break;
      }
     }
     #if MOTORS_ON == 1
      offensive.writeMicroseconds(1500);
     #endif
    }
    armIsUp = false;
    #if DEBUG == 2
      Serial.println();
      Serial.print("ARM is: ");
      Serial.print(armIsUp);
    #endif
  }
}

void selfRight(){
  #if MOTORS_ON == 1
    if(analogRead(armFeedback)< armAtFlip){
      if(armActive == true){
        offensive.writeMicroseconds(2000);
      }
    }
  #endif
  prev = millis();
  while(analogRead(armFeedback)< armAtFlip){
    #if MOTORS_ON == 0
      Serial.print("up ");
    #endif

    if(digitalRead(upSwitch) == false){
      #if MOTORS_ON == 1
        offensive.writeMicroseconds(1500);
      #else
        Serial.println();
        Serial.println("Impact at Upper Switch");
      #endif
      break;
    }
    cur = millis();
    if (cur - prev >= loopTimeouts) {
      #if MOTORS_ON == 0
        Serial.println();
        Serial.println("Timeout in arm going up!!");
      #endif
      break;
    }
  }
  #if MOTORS_ON == 1
    offensive.writeMicroseconds(1500);
  #else
    Serial.println();
    Serial.println("Arm at Flip Point");
  #endif
  delay(delayAtFlip);
  #if MOTORS_ON == 1
//    if(analogRead(armFeedback) > (armAtLow)){
      if(armActive == true){
        offensive.writeMicroseconds(1425);
//      }
    }
  #endif
  prev = millis();
  while(analogRead(armFeedback) > (armAtLow)){
    #if MOTORS_ON == 0
      Serial.print("down ");
    #endif
    
    if(digitalRead(lowSwitch) == false){
      #if MOTORS_ON == 1
        if(enableServos == true){
          offensive.writeMicroseconds(1500);
        }
      #else
        Serial.println();
        Serial.println("Impact at Lower Switch");
      #endif
      break;
    }
    cur = millis();
    if (cur - prev >= loopTimeouts) {
      #if MOTORS_ON == 0
        Serial.println();
        Serial.println("Timeout in arm going down!!");
      #endif
      break;
    }
  }
  #if MOTORS_ON == 1
    if(enableServos == true){
      offensive.writeMicroseconds(1500);
    }
  #else
    Serial.println();
    Serial.println("Flip complete");
  #endif
  selfRightDisable = true;
}

void calibrateArm(){
  while(true){
    IBus.loop();
    channel_update();
    #if MOTORS_ON == 0
      Serial.println("Awaiting operator readiness to calibrate");
    #endif
    delay(100);
    if(SWA == 2000){
      #if MOTORS_ON == 1
        offensive.writeMicroseconds(calUpSpeed);
      #else
        Serial.println("Begin Calibration");
      #endif
      prev = millis();
      while(digitalRead(upSwitch) == true){        //while upSwitch is not depressed
        #if MOTORS_ON == 0
          Serial.print("up, ");
          delay(10);
        #endif
        armAtHigh = analogRead(armFeedback);
        cur = millis();
        if (cur - prev >= 5000) {
          calFailed = true;
          break;
        }
      }
      #if MOTORS_ON == 1
        offensive.writeMicroseconds(1500);
      #else
        Serial.println();
        Serial.print("Finding Upper Limit. Upper Limit is ");
        Serial.println(armAtHigh);
      #endif
      delay(100);

      #if MOTORS_ON == 1
        offensive.writeMicroseconds(calDownSpeed);
      #endif
      prev = millis();
      while(digitalRead(lowSwitch) == true){
        #if MOTORS_ON == 0
          Serial.print("down, ");
          delay(10);
        #endif
        armAtLow = analogRead(armFeedback);
        armAtLowPerm = armAtLow;
        cur = millis();
        if (cur - prev >= 5000) {
          calFailed = true;
          break;
        }
      }
      #if MOTORS_ON == 1
        offensive.writeMicroseconds(1500);
      #else
        Serial.println();
        Serial.print("Finding Lower Limit. Lower Limit is ");
        Serial.println(armAtLow);
      #endif
      //calculate arm at flip
      armAtFlip = (armAtLow + (((armAtHigh - armAtLow)*2)/3));
      //calculate arm at lift
      armAtLifted = (armAtLow + (((armAtHigh - armAtLow)*2)/16)); //Was 2/8
      #if MOTORS_ON == 0
        Serial.print("Position for flipping bot is ");
        Serial.println(armAtFlip);
      #endif

      break;
    }
  }
}

void armAdjustment(){
  armOffset = (VRA - 1000);
  armOffset = map(armOffset, 0, 1000, 0, 150); //was 0 -> 100
  armAtLow = armAtLowPerm + armOffset;
  #if MOTORS_ON == 0
    Serial.print(",ArmOffset=");
    Serial.print(armOffset);
  #endif
}

