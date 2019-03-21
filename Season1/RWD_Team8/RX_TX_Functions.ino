//Function to read in data from IBus
void channel_update(){
  ch1_in = IBus.readChannel(0), DEC;
  ch2_in = IBus.readChannel(1), DEC;
  ch3_in = IBus.readChannel(2), DEC;
  ch4_in = IBus.readChannel(3), DEC;
  VRA = IBus.readChannel(4), DEC;
  VRB = IBus.readChannel(5), DEC;
  SWA = IBus.readChannel(6), DEC;
  SWB = IBus.readChannel(7), DEC;
  SWC = IBus.readChannel(8), DEC;
  SWD = IBus.readChannel(9), DEC;
}

#if MOTORS_ON == 0
void printCH() {
    Serial.print(IBus.readChannel(0), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(1), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(2), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(3), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(4), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(5), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(6), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(7), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(8), DEC);Serial.print(",");
    Serial.print(IBus.readChannel(9), DEC);Serial.print(",");
}
#endif

void modeSelect(){
  switch(SWC){
    case 2000:
      driveActive = true;
      armActive = true;
      break;
    case 1500:
      driveActive = true;
      armActive = false;    
      break;
    default:
      driveActive = false;
      armActive = false;
  }

  if(driveActive == false && armActive == false){   //Make LED GREEN
    digitalWrite(rLED, LOW);
    digitalWrite(gLED, HIGH);
    digitalWrite(bLED, LOW);
  }
  else{
    digitalWrite(rLED, HIGH); //Make LED RED
    digitalWrite(gLED, LOW);
    digitalWrite(bLED, LOW);
  }
  #if MOTORS_ON == 0
    if(dFlag == true){  
      Serial.print("Drive=");
      Serial.print(driveActive);
      Serial.print(",Arm=");
      Serial.print(armActive);  
    }
  #endif
}
