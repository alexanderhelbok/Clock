#include <Arduino.h>
#include "BasicStepperDriver.h"
#include <Wire.h>
#include "Wifi.h"
#include "NTP.h"
#include "Calc.h"
#include "Sleep.h"


// Setup Wifi and NTP Server +++++++++++++++++
char ssid[] = "Wirtschaftshilfe fuer Studenten";  //  your network SSID (name)
char pass[] = "9ter12ter1949";       // your network password

unsigned int localPort = 2390;      // local port to listen for UDP packets

const char* ntpServerName = "0.at.pool.ntp.org";

const int UTCTimeoffset = 2;    // Timezone offset compared to UTC at Greenwich Meridian (GMT)


// Pindefinitions Stepperdriver
#define Stepper_dir 13  // direction
#define Stepper_step 12 // step
#define Stepper_en 14   // enable

// constants 
const int CW = -1, CCW = 1;
const int Clock_direction = CW;   // Set direction: CW is clockwise, CCW ist anti clockwise 


// Setup Stepperdriver ++++++++++++
#define Stepper_Speed_Initial 8          // initial motor speed (when setting time)
#define Stepper_Speed 2         // motor speed
#define MICROSTEPS 16    // microsteps of the stepper driver
#define MOTOR_STEPS 2038 // steps and gearing of steper motor

// initialize stepper class (no, of steps per revolution, direction and step pin
BasicStepperDriver stepper(MOTOR_STEPS, Stepper_dir, Stepper_step);

// Set the number of steps to move minute handle by one minute
const unsigned long stepper_oneMin = 4096.0; // measured in practice
const unsigned long stepper_halfMin = 40965.0; //10 min 
const unsigned long stepper_quarterMin = 2048; //4 min

unsigned long moving_threshold = 60;  // move minute handle every 60 seconds 

unsigned long previousTime = 0;
unsigned long currentTime = 0;

// void incrementHalfMinute();
// void incrementOneMinute();

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Start stepper driver 
  stepper.begin(Stepper_Speed_Initial, MICROSTEPS);
  pinMode(Stepper_en, OUTPUT);
  digitalWrite(Stepper_en, HIGH);

  // Connect to WiFi
  WifiConnect(ssid, pass);

  // Get current Time from NTP Server and correct time
  int time = getTime(localPort, UTCTimeoffset, ntpServerName);
  Serial.println(time);
  int now = TimeCorrection(stepper_halfMin/600.0, time, 62.65);
  Serial.println(now);

  // Disconnect Wifi and enter modem sleep, drops 75 mA
  WifiDisconnect();

  // move to desired time  
  digitalWrite(Stepper_en, LOW);
  if (time <= 21276){
    stepper.move(Clock_direction*now);
  }
  else{
    stepper.move(-Clock_direction*now);
  }
  stepper.setRPM(Stepper_Speed);
  digitalWrite(Stepper_en, HIGH);
  // previousTime = millis();
}


void incrementQuarterMinute() {
  stepper.move(Clock_direction*stepper_quarterMin);
}

void incrementHalfMinute() {
  digitalWrite(Stepper_en, LOW);
  stepper.move(Clock_direction*stepper_halfMin);
  digitalWrite(Stepper_en, HIGH);
}

void incrementOneMinute() {
  digitalWrite(Stepper_en, LOW);
  stepper.move(Clock_direction*stepper_oneMin);
  digitalWrite(Stepper_en, HIGH);
}

void loop() {
  // currentTime = RTCmillis();
  // Serial.println(RTCmillis() - previousTime);
  // previousTime = currentTime;
  incrementOneMinute();
  // Serial.print(RTCmillis() - currentTime);
  // Serial.print("\t");
  // currentTime = RTCmillis();
  light_sleep(66170);
  // Serial.print(RTCmillis() - currentTime);
  // Serial.print("\t");
  // ESP.deepSleep(10e6);
}
