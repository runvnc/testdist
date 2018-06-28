int MAX_RPM = 10000;
int currentSimRPM = 0;

// suppose it takes simulated motor 1 second to go from 0
// RPM to 3000 etc.
float RPM_CHANGE_PER_SECOND = 3000;

byte simulatedMotorDuty = 0;
float simulatedRPM = 0;

void setSimulatedMotorDuty(double duty) {
  simulatedMotorDuty = (byte)duty; 
}

float simDisplayAngle = 0;
float simDisplayRPM = 0;

int setSimulatedLED(Led which, float value) {
  switch (which) {
    case DISPLAY_RPM:
      displayRPM = value;
      break;
    case DISPLAY_ANGLE:
      displayAngle = value;
      break;
  }
  SerialUSB.println("RPM: " + String(displayRPM) + 
                    "Angle: " + String(displayAngle));
}

void updateSimulatedRPM() {
  // simulated speed is based on time that duty has been applied
  // suppose 100% duty = 100% max speed after enough time
}

void encoderInterruptIfTime() {
  // based on simulated speed
}

void distributorInterruptIfTime(){
  
}

void indexPulseInterruptIfTime() {
  
}

void simulatorStepRun() {
  updateSimulatedRPM();
  encoderInterruptIfTime();
  indexPulseInterruptIfTime();
  distributorInterruptIfTime();
}

