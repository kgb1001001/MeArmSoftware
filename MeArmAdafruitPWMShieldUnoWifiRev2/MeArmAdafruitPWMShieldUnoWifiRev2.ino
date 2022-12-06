/*
  MeArm robot arm example using the aREST library with the WifiNINA library (for the Arduino Uno Wifi Rev 2)
  This example has been tested on an Arduino Uno Wifi Rev 2 upgraded to the 1.4.8 firmware. 

  Contributed by Kyle Brown
  Written and tested in December 2021
  
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <aREST.h>

#include "arduino_secrets.h" 

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define BASE_SERVOMIN  100 // This is the 'minimum' pulse length count (out of 4096)
#define BASE_SERVOMAX  400 // This is the 'maximum' pulse length count (out of 4096)
#define GRIPPER_SERVOMIN  40 // This is the 'minimum' pulse length count (out of 4096)
#define GRIPPER_SERVOMAX  180 // This is the 'maximum' pulse length count (out of 4096)
#define XSERVOMIN 200
#define XSERVOMAX 400
#define YSERVOMIN 95
#define YSERVOMAX 190
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

/** 
 *  I plugged the servos in in the following order to ports 0-3
 *  
 */

#define BASESERVO 0
#define GRIPPERSERVO 1
#define XSERVO 2
#define YSERVO 3

// our servo # counter
int firsttime = 0;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Create aREST instance
aREST rest = aREST();

// Initialize the WiFi server library
WiFiServer server(80);

int baseAngle = 0;
int xAngle = 0;
int yAngle = 0;
int gripperAngle = 0;

int openGripper(String command);
int closeGripper(String command);
int baseRotate(String command);
int extendX(String command);
int extendY(String command);
int homePosition(String command);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  rest.variable("xAngle",&xAngle);
  rest.variable("yAngle",&yAngle);
  rest.variable("gripperAngle",&gripperAngle);
  rest.variable("baseAngle",&baseAngle);

  rest.function("openGripper",openGripper);
  rest.function("closeGripper",closeGripper);
  rest.function("baseRotate",baseRotate);
  rest.function("xExtend",xExtend);
  rest.function("yExtend",yExtend);
  rest.function("homePosition",goHome);

  // Give name and ID to device (ID should be 6 characters long)
  rest.set_id("100100");
  rest.set_name("futzy_hedgehog");

  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}

void driveServo(int servonum, int angle, int servomin, int servomax) {
  if (angle < 0 || angle > 180)
    return;
  int pulselen = 0;
  Serial.print("driving servo ");
  Serial.print(servonum);
  Serial.print(" to angle");
  Serial.print(angle);
  Serial.print(" with pulselength ");
  pulselen=map(angle, 0, 180, servomin, servomax);
  Serial.println(pulselen);
  pwm.setPWM(servonum, 0, pulselen);
}

void gripperOpen() {
    // Open the gripper by driving the gripper servo to the 0 position
  driveServo(GRIPPERSERVO, 0, GRIPPER_SERVOMIN, GRIPPER_SERVOMAX);
  gripperAngle = 0;
}


void gripperClose() {
    // Close the gripper by driving the gripper servo to the 180 position
  driveServo(GRIPPERSERVO, 180, GRIPPER_SERVOMIN, GRIPPER_SERVOMAX);
  gripperAngle = 180;
}


void rotateBase(int angle) {
  // rotate the base to angle
  driveServo(BASESERVO, angle, BASE_SERVOMIN, BASE_SERVOMAX);
  baseAngle = angle;
}


void xExtend(int angle) {
    // Extend the arm along X by angle degrees
  driveServo(XSERVO, angle, XSERVOMIN, XSERVOMAX);
  xAngle = angle;
}

void yExtend(int angle) {
    // Extend the arm along Y by X degrees
  driveServo(YSERVO, angle, YSERVOMIN, YSERVOMAX);
  yAngle = angle;
}

void goHome() {
  // this rotates each to a halfway "home" postion"
  rotateBase(90);
  delay(1000);
  xExtend(90);
  delay(1000);
  yExtend(90);
}

void loop() {

  if (firsttime == 0) {
    Serial.println("starting from home position");
    goHome();
    firsttime=1;
    delay(5000);
    Serial.println("awaiting REST commands");
  } else {
      // listen for incoming clients
    WiFiClient client = server.available();
    rest.handle(client);
  }
 
}

int openGripper(String command) {
  // Get state from command
  int ignored = command.toInt();
  gripperOpen();
  return 1;
}

int closeGripper(String command) {
  // Get state from command
  int ignored = command.toInt();
  gripperClose();
  return 1;
}

int baseRotate(String command) {
  // Get state from command
  int state = command.toInt();
  rotateBase(state);
  return 1;
}

int extendX(String command) {
  // Get state from command
  int state = command.toInt();
  xExtend(state);
  return 1;
}

int extendY(String command) {
  // Get state from command
  int state = command.toInt();
  yExtend(state);
  return 1;
}

int homePosition(String command) {
  // Get state from command
  int ignored = command.toInt();
  goHome();
  return 1;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
