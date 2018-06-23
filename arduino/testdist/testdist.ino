float EDGES_PER_REVOLUTION = 2048;
String mode = "IDLE";
const int DATAPOINTS = 10000;
float data[DATAPOINTS][2];

// Arduino reference says make them volatile
// if they are used in interrupt handlers
volatile float angleDegrees = 0;
volatile float initialAngle = 0;
volatile float advanceAngle = 0;
volatile int rotations = 0;
volatile int startMS = 0;
volatile int angleIncrements = 0;
int targetRPM = 100;
int rpmUpdateIntervalMS = 50;

int p = 0; // index into data[]

void start() {
  startMS = millis();
}

void zeroAnglePulse() {
  angleIncrements = 0;
  rotations++;
}

void incrementAngleEdge() {
  angleIncrements++;
}

void calculateAngle() {
  angleDegrees = (angleIncrements / EDGES_PER_REVOLUTION) * 360;
  if (mode == "SETUP")
    initialAngle = angleDegrees;
  else
    advanceAngle = angleDegrees - initialAngle;
}

void distributorFired() {
  calculateAngle();
}

void adjustMotor() {
  
}

void updateRPM() {
  int elapsedMS = millis()-startMS;
  rotationsPerSecond = rotations / (elapsedMS/1000.0);
  rpm = rotationsPerSecond * 60;
  if (abs(rpm-targetRPM)>100) adjustMotor();
  startMS = millis();
  rotations = 0;
}

void blink(int d) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(d);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW        
  delay(d);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  blink(250);

  randomSeed(analogRead(0));
  SerialUSB.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //pinMode(indexPulsePin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(indexPulsePin),zeroAnglePulse,CHANGE);

  //pinMode(incrementPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(incrementPin),incrementAngleEdge,CHANGE);

  blink(250);
}

char line[1024];

String readLine() {
  if (SerialUSB.available() > 0) {
    size_t bytes = SerialUSB.readBytesUntil('\n', line, 1024);  
    line[bytes] = 0;
  } else {
    line[0] = 0;
  }
  return String(line);
}

//float shift = random(0,250);
float shift = 100;
float factor = 300;
//float factor = random(230,350);

void clearData() {
  for (int i=0; i<DATAPOINTS; i++) {
    data[i][0] = -1000;
    data[i][1] = -1000;
  }
  shift = random(50,200);
  factor = random(280,320);
}

void checkCommand() {
  String cmd = readLine();
  if (cmd[0] != 0) {
    if (cmd == "START") {
      clearData();
      mode = "RUN";
    } else if (cmd == "STOP") {
      mode = "UPLOAD";
    }
  }
}

float lastAngle = -1000;

float angleFromRPM(float rpm) {
  float angle = 15.0*atan((rpm-shift)/factor)+1.0;
  if (angle < 0 && rpm > 500) return lastAngle;
  if (angle < 0) angle = 0;
  lastAngle = angle;
  return angle;
  //return 15.0*atan((rpm-100.0)/300.0)+1.0;
}

/*
void run_() {
  digitalWrite(LED_BUILTIN, HIGH);
  data[p][0] = p*1.0;
  data[p][1]= angleFromRPM(p);
  p++;
  if (p >= 4000) {
    mode = "UPLOAD";
  }
  if ((p)%100==0) SerialUSB.println("STATUS "+String(p));
  digitalWrite(LED_BUILTIN, LOW);
} */

int lastRPMUpdate = -100;
int startMS = -100;
float fakeSpeed = 1.0;

void run_() {
  int ms = millis();
  int elapsedRPM = 0;
  if (lastRPMUpdate != -100) elapsedRPM = ms - lastRPMUpdate;
  if (elapsedRPM >= rpmUpdateIntervalMS) {
    updateRPM();
    lastRPMUpdate = ms;
  }
  int elapsedMS = 0;
  if (startMS != -100)
    elapsedMS = ms - startMS;
  if (elapsedMS > 8.0/fakeSpeed) {
    incrementAngleEdge(); // FAKE INTERRUPT FIRE
  }
  if (elapsedMS > 200/fakeSpeed) {
    zeroAnglePulse(); // FAKE INTERRUPT FIRE
  }
  if (elapsedMS > 275/fakeSpeed) {
    distributorFired(); // FAKE INTERRUPT
  }
}

void upload() {
  int max_ = p;
  p = 0;
  SerialUSB.println("UPLOAD");
  mode = "IDLE";
  for (int i=0; i<max_; i++) {
    SerialUSB.println("DATA " + String(data[i][0])+" "+String(data[i][1]));
  }
  SerialUSB.println("END");
}

void idle() {
  SerialUSB.println("IDLE");
  delay(500);
}

void loop() {
  checkCommand();
  if (mode == "RUN") {
    run_();    
  } else if (mode == "UPLOAD") {
    upload();
  } else {
    idle();
  }
}
 
