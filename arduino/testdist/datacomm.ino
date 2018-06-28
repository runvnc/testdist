const int DATAPOINTS = 2000;
float data[DATAPOINTS][2];
int p = 0; // index into data

char line[1024];

void initializeComm() {
  SerialUSB.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

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

void addDataPoint(int rpm, float angle) {
  data[p][0] = rpm;
  data[p][1] = angle;
}

void checkCommand() {
  String cmd = readLine();
  // maybe control simulator with buttons on UI
}

void upload() {
  int max_ = p;
  p = 0;
  SerialUSB.println("UPLOAD");
  for (int i=0; i<max_; i++) {
    SerialUSB.println("DATA " + String(data[i][0])+" "+String(data[i][1]));
  }
  SerialUSB.println("END");
}

void sendStatus() {
  SerialUSB.println("STATUS  RPM: " + String(rpm) + "  degrees: " + String(angleDegrees));  
}

void sendLine(String s) {
  SerialUSB.println(s);
}

void reportCylinderCount(int cylinders) {
  sendLine("CYLINDERS: " + String(cylinders));
}

