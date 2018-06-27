int indexPulsePin = 0;  // encoder index pin ?
int incrementPin = 1;   // encoder pin ?
int distributorPin = 2; // ignition pin ?

void blink(int d) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(d);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW        
  delay(d);
}

DISPLAY_UPDATE_MS = 50;
int lastAngleUpdate = NULL;

void displayAngle(float angle) {
  if (lastAngleUpdate == NULL ||
      millis() - lastAngleUpdate > DISPLAY_UPDATE_MS) {
    setSimulatedLED(DISPLAY_ANGLE, angle);
    lastAngleUpdate = millis();
  }
}

int lastRPMUpdate = NULL;

void displayRPM(int rpm) {
  if (lastRPMUpdatae == NULL ||
      millis() - lastRPMUpdate > DISPLAY_UPDATE_MS) {
    setSimulatedLED(DISPLAY_RPM, rpm);
    lastRPMUpdate = millis();      
  }
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

  // attach interrupts for switches
}

void initializeHW() {
  pinMode(LED_BUILTIN, OUTPUT);
  //attachInterrupts();
  blink(250);
}


