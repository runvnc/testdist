enum Led { DIPLAY_RPM, DISPLAY_ANGLE };

int MAX_RPM = 10000;
int currentSimRPM = 0;

// assume it takes simulated motor 1 second to go from 0
// RPM to 3000 etc.
float RPM_CHANGE_PER_SECOND = 3000;

byte simulatedMotorDuty = 0;
float simulatedRPM = 0;

void setSimulatedMotorDuty(double duty) {
  simulatedMotorDuty = (byte)duty; 
}

float displayAngle = 0;
float displayRPM = 0;

void setSimulatedLED(Led which, float value) {
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
  
}

void encoderInterruptIfTime() {
  
}

void simulatorStepRun() {
  updateSimulatedRPM();
  encoderInterruptIfTime();
  indexPulseInterruptIfTime();
  distributorInterruptIfTime();
}

