enum State { IDLE, COUNT_CYLINDERS, GET_ZERO_ANGLE, RUN_AUTO, RUN_MANUAL, UPLOAD };
enum Event { START, LOOP, ENCODER_PULSE, INDEX_PULSE, IGNITION_PULSE, 
             SWITCH_SETUP, SWITCH_RUN, SWITCH_MANUAL };

State state = IDLE;

// interrupt (encoder index)
void zeroAnglePulse() { handleEvent(INDEX_PULSE); }

// interrupt (encoder A or B wave edge)
void incrementAngleEdge() { handleEvent(ENCODER_PULSE); }

// interrupt
void distributorFired() { handleEvent(IGNITION_PULSE); }

void setup() { 
  initializeHW();
  initializeComm();
  initializeMotor();
  
  randomSeed(analogRead(0));

  blink(250);

  setState(IDLE);
}

int numCylinders = 0;
bool cylinderIndex = false;

void countCylindersState(Event event) {
  switch (event) {
    case INDEX_PULSE:
      if (!cylinderIndex) {
        numCylinders = 0;
        cylinderIndex = true;
      } else {
        cylinderIndex = false;
        reportCylinderCount(numCylinders);
        setState(GET_ZERO_ANGLE);
      }
      break;
    case IGNITION_PULSE:
      if (cylinderIndex) numCylinders++;
      break;
  }
}

bool zeroIndex = false;
bool zeroIgnition = false;
float zeroAngle = 0;
int zeroEncoderEdges = 0;

void getZeroAngleState(Event event) {
  switch event {
    case INDEX_PULSE:
      if (!zeroIndex)
        zeroIndex = true;
      else
        zeroIndex = false;
      break;
    case ENCODER_PULSE:
      zeroEncoderEdges++;
      break;
    case IGNITION_PULSE:
      zeroAngle = calculateAngle(zeroEncoderEdgges);
      break;
  }
}

int lastRecordedRPM = NULL;
float lastRecordedAngle = NULL;

void checkNewData() {
  if (lastRecordedRPM != rpm ||
      lastRecordedAngle != advanceAngle) {
    addDataPoint(rpm,advanceAngle);
    lastRecordedRPM = rpm;
    lastRecordedAngle = advanceAngle;
  }
}

float calculateAngle(int encoderEdges) {
  angleDegrees = (encoderEdges / EDGES_PER_REVOLUTION) * 360;
  return angleDegrees;
}

bool runIndex = false;
int runEncoderEdges = 0;
int encoderEdges = 0;

void runState(Event event) {
  switch (event) {
    case INDEX_PULSE:
      if (!runIndex) runIndex = true;
      else {
        runEncoderEdges = 0;
        runIndex = false;
      }
      break;
    case ENCODER_PULSE:
      if (runIndex) runEncoderEdges++;
      break;
    case IGNITION_PULSE:
      if (runIndex) {
        runIndex = false;
        float angle = calculateAngle(runEncoderEdges);
        advanceAngle = angle - zeroAngle;
      }
      break;
    case LOOP:
      updateRPM();
      checkNewData();
      break;
  }
}

void allStates(Event event) {
  switch (event) {
    case ENCODER_PULSE:
      // This gets reset in updateRPM in motor.ino
      // after the RPM is calculated
      encoderEdges++;
      break;
  }
}

void runAutoState(Event event) {
  runState(event);
} 

void runManualState(Event event) {
  runState(event);
}

void uploadState(Event event) {
  upload();
  setState(IDLE);
} 

void idleState(Event event) {
  switch (event) {
    case LOOP:
     displayRPM(0);
     displayAngle(0);
     break;
  }
}

void setState(String newState) {
  state = newState();
  // log state to SerialUSB maybe
}

void handleEvent(Event event) {
  eventHandler[state](event);
  allStates(event);
}

void initEventHandlers() {
  eventHandler[IDLE] = idleState;
  eventHandler[COUNT_CYLINDERS] = countCylindersState;
  eventHandler[GET_ZERO_ANGLE] = getZeroAngleState;
  eventHandler[RUN_AUTO] = runAutoState;
  eventHandler[RUN_MANUAL] = runManualState;
  eventHandler[UPLOAD] = uploadState;
}

void loop() {
  checkCommand();
  handleEvent(LOOP);
}
 
