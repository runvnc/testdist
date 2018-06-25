enum State { IDLE, COUNT_CYLINDERS, GET_ZERO_ANGLE, RUN_AUTO, RUN_MANUAL, UPLOAD };
enum Event { START, LOOP, ENCODER_PULSE, INDEX_PULSE, IGNITION_PULSE, SWITCH_SETUP, SWITCH_RUN, SWITCH_MANUAL };

int EDGES_PER_REVOLUTION = 2048;
String state = "IDLE";

volatile float angleDegrees = 0;
volatile float initialAngle = 0;
volatile float advanceAngle = 0;
volatile int rotations = 0;
volatile int startMS = NULL;
volatile int startmicros = NULL;
volatile int lastIndexMS = NULL;
volatile int rotateMS = 0;
volatile int rpm = 0;
volatile int angleIncrements = 0;
int targetRPM = 100;
int rpmUpdateIntervalMS = 50;
int lastRPMUpdate = NULL;
float fakeSpeed = 1.0;

int p = 0; // index into data[]

// interrupt (encoder index)
void zeroAnglePulse() {
  handleEvent(INDEX_PULSE);
  
  //angleIncrements = 0;
  //rotations++;
}

// interrupt (encoder A or B wave edge)
void incrementAngleEdge() {
  handleEvent(ENCODER_PULSE);
  //angleIncrements++;
}

void calculateAngle() {
  angleDegrees = (angleIncrements / EDGES_PER_REVOLUTION) * 360;
  if (state == "SETUP")
    initialAngle = angleDegrees;
  else
    advanceAngle = angleDegrees - initialAngle;
}

// interrupt
void distributorFired() {
  handleEvent(IGNITION_PULSE);
  //calculateAngle();
}

int motorDutyByte = 0;
int motorDutyPin = 8;

void updateRPM() {
  int elapsedMS = 0;
  if (lastRPMUpdate != -100) {
    elapsedMS = millis()-lastRPMUpdate;
    lastRPMUpdate = millis();
  }
  float rotations_ = (float)rotations + (angleDegrees/360.0);
  float rotationsPerSecond = rotations / ((float)rotateMS/1000.0);
  //SerialUSB.println("rotateS: " + String((float)rotateMS/1000.0) + " rotations: " + String(rotations));
  rpm = rotationsPerSecond * 60;
  int error = rpm - targetRPM;
  if (abs(error)>20) adjustMotor(error, elapsedMS);
  rotations = 0;
  lastRPMUpdate = millis();
}

void setup() { 
  initializeHW();
  initializeComm();
  
  randomSeed(analogRead(0));

  blink(250);

  setState(IDLE);
}

float lastAngle = -1000;

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
int zeroEncoderEdges = 0;

void getZeroAngleState(Event event) {
  switch event {
    case INDEX_PULSE:
      break;
    case ENCODER_PULSE:

      break;

    case IGNITION_PULSE:
   
      break;
  }
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
  
}

void runManualState(Event event) {
  switch (event) {
    case 
  }
}

void uploadState(Event event) {
  
}

void idleState(Event event) {

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
  
}

void loop() {
  checkCommand();
  handleEvent(LOOP);
}
 
