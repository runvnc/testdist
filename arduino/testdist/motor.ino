// http://playground.arduino.cc/Code/PIDLibrary MIT license
#include <PID_v1.h>

// There is also a PID autotune library I might use

int motorDutyPin = 0; // ??

PID motorPID(&rpm, &motorDuty, &targetRPM, 2, 5, 1, DIRECT);

void adjustMotor() {
  motorPID.Compute();
  analogWrite(motorDutyPin, motorDuty);
  setSimulatedMotorDuty(motorDuty);
}

void updateRPM() {
  int nowMS = millis();
  if (lastRPMUpdate == NULL) {
    lastRPMUpdate = nowMS;
    return;
  }
  int elapsedMS = nowMS - lastRPMUpdate;
  if (elapsedMS < RPM_UPDATE_INTERVAL_MS) return;

  float rotations = encoderEdges/(float)EDGES_PER_REVOLUTION;
  float rotPerMS = rotations/elapsedMS;
  float rotPerSecond = rotPerMS / 1000;
  rpm = rotPerSecond / 60.0;
  encoderEdges = 0;
  adjustMotor();
  lastRPMUpdate = nowMS;
}

void initializeMotor() {
  motorPID.SetMode(AUTOMATIC);
}

