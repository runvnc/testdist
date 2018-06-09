String mode = "IDLE";
const int DATAPOINTS = 600;
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

void clearData() {
  for (int i=0; i<DATAPOINTS; i++) {
    data[i][0] = -1000;
    data[i][1] = -1000;
  }
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

void run_() {
  blink(100);
  data[p][0] = p;
  data[p][1]= random(0,50);
  p++;
  if (p >= DATAPOINTS) {
    mode = "UPLOAD";
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

void loop() {
  checkCommand();
  if (mode == "RUN") {
    run_();    
  } else if (mode == "UPLOAD") {
    upload();
  }
}
 
