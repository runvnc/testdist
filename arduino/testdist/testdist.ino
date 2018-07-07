enum State { IDLE, COUNT_CYLINDERS, GET_ZERO_ANGLE, RUN_AUTO, RUN_MANUAL, UPLOAD };
enum Event { START, LOOP, ENCODER_PULSE, INDEX_PULSE, IGNITION_PULSE, 
             SWITCH_SETUP_ON, SWITCH_SETUP_OFF, SWITCH_RUN_ON, SWITCH_RUN_OFF, 
             SWITCH_MANUAL_ON, SWITCH_MANUAL_OFF };
enum Led { DISPLAY_RPM, DISPLAY_ANGLE };           

// Using interrupts to make it more practical to ensure that 
// pulses are detected while at the same time ensuring system
// stays responsive.

State state = IDLE;

int EDGES_PER_REVOLUTION = 2048;
int RPM_UPDATE_INTERVAL_MS = 10;
double targetRPM = 0;
double rpm = 0;
double motorDuty = 0;

float advanceAngle = 0, angleDegrees = 0;

// interrupt (encoder index)
void zeroAnglePulse() { handleEvent(INDEX_PULSE); }

// interrupt (encoder A or B wave edge)
void incrementAngleEdge() { handleEvent(ENCODER_PULSE); }

// interrupt
void distributorFired() { handleEvent(IGNITION_PULSE); }

// interrupt
void setupSwitchOn() { handleEvent(SWITCH_SETUP_ON); }

// interrupt
void runSwitchOn() { handleEvent(SWITCH_RUN_ON); }

void setup() { 
  initializeHW();
  initializeComm();
  initializeMotor();
  
  randomSeed(analogRead(0));

  blink(250);

  initEventHandlers();
  setState(IDLE);
}

// So with the event-based design, the state handlers 
// are the most straightforward approach.

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
  switch (event) {
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
      zeroAngle = calculateAngle(zeroEncoderEdges);
      break;
  }
}

int lastRecordedRPM = NULL;
float lastRecordedAngle = NULL;

void checkNewData() {
  // TODO: check that data changed by a certain
  // amount and some time has elapsed before
  // adding a data point
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
    case SWITCH_SETUP_ON:
      sendLine("COUNT_CYLINDERS state activated.");
      setState(COUNT_CYLINDERS);
      break;
  }
}

void setState(State newState) {
  state = newState;
  // log state to SerialUSB maybe
}

void (*eventHandler[5])(Event);

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
 
