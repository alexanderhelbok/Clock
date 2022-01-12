#include <Arduino.h>
#include "A4988.h"
// #include <Ticker.h>
#include <Wire.h>
#include "Wifi.h"
#include "NTP.h"


// Setup Wifi and NTP Server +++++++++++++++++
char ssid[] = "Wirtschaftshilfe fuer Studenten";  //  your network SSID (name)
char pass[] = "9ter12ter1949";       // your network password

unsigned int localPort = 2390;      // local port to listen for UDP packets

const char* ntpServerName = "0.at.pool.ntp.org";

const int UTCTimeoffset = 1;    // Timezone offset compared to UTC at Greenwich Meridian (GMT)
const int secoffset = UTCTimeoffset * 3600;   // Timezone offset in seconds


// Pindefinitions +++++++++++++++++

// Stepperdriver
#define Stepper_dir 13  // direction
#define Stepper_step 12 // step
#define Stepper_en 14   // enable

// constants 
const int CW = -1;
const int CCW = 1;
// Set direction: CW is clockwise, CCW ist anti clockwise 
const int Clock_direction = CW;


// Setup Stepperdriver ++++++++++++

#define MOTOR_STEPS 2038 // steps and gearing of steper motor
#define Stepper_Speed 1          // motor speed, 2.5 rpm should be inaudible
#define MICROSTEPS 16    // microsteps of the stepper driver

// initialize stepper class (no, of steps per revolution, direction and step pin
A4988 stepper(MOTOR_STEPS, Stepper_dir, Stepper_step);

// Set the number of steps to move minute handle by one minute
// const unsigned long stepper_oneMin = 8152; // calculated
const unsigned long stepper_oneMin = 8192; // measured in practice
const unsigned long stepper_halfMin = 4096;
const unsigned long stepper_quarterMin = 2048;
const unsigned long stepper_twoSec = 273;

unsigned long moving_threshold = 15000;  // move minute handle every 15 seconds (15000 ms)

unsigned long previousTime=0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Start stepper driver 
  stepper.begin(25, MICROSTEPS);
  pinMode(Stepper_en, OUTPUT);
  digitalWrite(Stepper_en,LOW);

  // Connect to WiFi
  WifiConnect(ssid, pass);

//   Get current Time from NTP Server#
  int now = getTime(localPort, secoffset, ntpServerName);
  Serial.println(now);
  int speed = 60/3600;  //? time required to turn hour handle by one (in seconds)
  if (now <= 43200){
    stepper.move(Clock_direction*stepper_twoSec*((now + now*speed) / 2));
  }
  else {
    stepper.move(-Clock_direction*stepper_twoSec*((now - 43200 + (now - 43200)*speed) / 2));
  }
}


void incrementQuarterMinute() {
  stepper.move(Clock_direction*stepper_quarterMin);
}

void incrementHalfMinute() {
  stepper.move(Clock_direction*stepper_halfMin);
}

void incrementOneMinute() {
  stepper.move(Clock_direction*stepper_oneMin);
}


void loop() {
  stepper.setRPM(Stepper_Speed);
  unsigned long currentTime = millis();

  // check if 15 seconds passed
  if ((unsigned long)(currentTime - previousTime) >= moving_threshold) {
    previousTime = currentTime;
    incrementQuarterMinute(); // this takes some time
    // incrementHalfMinute();
    // incrementOneMinute();
  }
}
