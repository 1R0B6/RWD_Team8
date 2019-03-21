//ch1 = left to right
//ch2 = forward/reverse

void driveValues(){
  left_in = ch1_in;
  right_in = ch2_in;
  left_out = ch1_in;
  right_out = ch2_in;
   
//  //Protects motors from drastic changes in normal mode.
//  if(left_in < left_out){
//    int diff = (left_out - left_in)/change_debounce;
//    left_out = left_out - diff;
//  }
//  else{
//    int diff = (left_in - left_out)/change_debounce;
//    left_out = left_out + diff;
//  }
//
//  if(right_in < right_out){
//    int diff = (right_out - right_in)/change_debounce;
//    right_out = right_out - diff;
//  }
//  else{
//    int diff = (right_in - right_out)/change_debounce;
//    right_out = right_out + diff;
//  }

  if(reverseDrive == true){
    int temp_RO = right_out;
    right_out = (1500 + ((left_out - 1500) * -1));
    left_out = (1500 + ((temp_RO - 1500) * -1));
  }

  switch(limiter){
    case 25:
      right_out = constrain(right_out, 1375, 1625);
      left_out = constrain(left_out, 1375, 1625);
      break;
    case 50:
      right_out = constrain(right_out, 1250, 1750);
      left_out = constrain(left_out, 1250, 1750);
      break;
    case 75:
      right_out = constrain(right_out, 1125, 1875);
      left_out = constrain(left_out, 1125, 1875);
      break;
  }

  #if MOTORS_ON == 1
    if(driveActive==true){
      rightDrive.writeMicroseconds(right_out);
      leftDrive.writeMicroseconds(left_out);
    }
    else{
      rightDrive.writeMicroseconds(1500);
      leftDrive.writeMicroseconds(1500);    
    }
  #else
    if(dFlag == true){
      Serial.print(",Left I/O,");
      Serial.print(left_in);
      Serial.print(",");
      Serial.print(left_out);
      Serial.print(",Right I/O,");
      Serial.print(right_in);
      Serial.print(",");
      Serial.print(right_out);
    }
  #endif
}

void orientation(){
  if(SWD == 2000){
    reverseDrive = true;
  }
  else{
    reverseDrive = false;
  }
  #if MOTORS_ON == 0
    if(dFlag == true){
      Serial.print("Rev=");
      Serial.print(reverseDrive);
      Serial.print(",");
    }
  #endif
}

void operationPerc(){
  if(VRB <= 1250){//25%
    limiter = 25;
  }
  else if ((VRB >= 1250) && (VRB <= 1500)){
    limiter = 50;
  }
  else if((VRB >= 1500) && (VRB <= 1750)){
    limiter = 75;
  }
  else{
    limiter = 100;
  }
  #if MOTORS_ON == 0
    if(dFlag == true){
      Serial.print(",Op%=");
      Serial.print(limiter);  
    }
  #endif
}
