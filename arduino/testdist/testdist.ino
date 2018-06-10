String mode = "IDLE";
const int DATAPOINTS = 10000;
float data[DATAPOINTS][2];
int p = 0;


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
 
