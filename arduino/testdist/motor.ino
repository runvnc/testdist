void adjustMotor(int error, int msSinceLast) {
  // just increment or decrement duty cycle for now
  // probably will be adequate to start testing prototype
  // need more motivation to dig into PID
  // will be motivated if can't get anywhere with simple control
  // or move on from prototype
  // or done testing other things on prototype
  if (error > 0) 
    motorDutyByte--;
  else
    motorDutyByte++;
  motorDutyByte = min(motorDutyByte, 255);
  motorDutyByte = max(0, motorDutyByte);
  // nothing connected yet so commenting out
  // Arduino handles PWM with a timer interrupt
  // at 500 or 1000 Hz
  // 255 is 100%
  // analogWrite(motorDutyPin, motorDutyByte);
}

