void blink(int d) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(d);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW        
  delay(d);
}

void attachInterrupts() {
  // nothing connected right now
  // so not actually calling this function
  // will make fake calls to ISR
  // handled im simulator.ino
  
  pinMode(indexPulsePin, INPUT);
  attachInterrupt(digitalPinToInterrupt(indexPulsePin),zeroAnglePulse,HIGH);

  pinMode(incrementPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(incrementPin),incrementAngleEdge,CHANGE);

  pinMode(distributorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(distributorPin),distributorFired,HIGH);  
}

void initializeHW() {
  pinMode(LED_BUILTIN, OUTPUT);
  //attachInterrupts();
  blink(250);
}


