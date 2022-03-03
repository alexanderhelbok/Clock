#include <Arduino.h>
#include "A4988.h"
// #include <Ticker.h>
#include <Wire.h>
#include "Wifi.h"
#include "NTP.h"
#include "Math.h"


// Setup Wifi and NTP Server +++++++++++++++++
char ssid[] = "Wirtschaftshilfe fuer Studenten";  //  your network SSID (name)
char pass[] = "9ter12ter1949";       // your network password

unsigned int localPort = 2390;      // local port to listen for UDP packets

const char* ntpServerName = "0.at.pool.ntp.org";

const int UTCTimeoffset = 1;    // Timezone offset compared to UTC at Greenwich Meridian (GMT)


// Pindefinitions Stepperdriver
#define Stepper_dir 13  // direction
#define Stepper_step 12 // step
#define Stepper_en 14   // enable

// constants 
const int CW = 1, CCW = -1;
const int Clock_direction = CW;   // Set direction: CW is clockwise, CCW ist anti clockwise 


// Setup Stepperdriver ++++++++++++
#define Stepper_Speed_Initial 17          // initial motor speed (when setting time)
#define Stepper_Speed 0.25          // motor speed
#define MICROSTEPS 16    // microsteps of the stepper driver
#define MOTOR_STEPS (2038 / MICROSTEPS) // steps and gearing of steper motor

// initialize stepper class (no, of steps per revolution, direction and step pin
A4988 stepper(MOTOR_STEPS, Stepper_dir, Stepper_step);

// Set the number of steps to move minute handle by one minute
const unsigned long stepper_oneMin = 512; // measured in practice
const unsigned long stepper_halfMin = 5124.0; //10 min 
const unsigned long stepper_quarterMin = 2048; //4 min

// unsigned long moving_threshold = 15000;  // move minute handle every 15 seconds (15000 ms)

// unsigned long previousTime = 0;


void setup() {
  Serial.begin(115200);
  Serial.println();

  // Start stepper driver 
  stepper.begin(Stepper_Speed_Initial, MICROSTEPS);
  pinMode(Stepper_en, OUTPUT);
  digitalWrite(Stepper_en, LOW);

  // Connect to WiFi
  WifiConnect(ssid, pass);

  // Get current Time from NTP Server and correct time
  int time = getTime(localPort, UTCTimeoffset, ntpServerName);
  Serial.println(time);
  int now = TimeCorrection(stepper_halfMin/600.0, time, 3.96*Stepper_Speed_Initial);
  Serial.println(now);

  // Disconnect Wifi and enter modem sleep
  WifiDisconnect();

  // move to desired time  
  if (time <= 21276){
    stepper.move(Clock_direction*now);
  }
  else{
    stepper.move(-Clock_direction*now);
  }
  stepper.setRPM(Stepper_Speed);
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
  // unsigned long currentTime = millis();
  stepper.move(Clock_direction*stepper_halfMin);

  // check if 15 seconds passed
  // if ((unsigned long)(currentTime - previousTime) >= moving_threshold) {
  //   previousTime = currentTime;
  //   incrementQuarterMinute(); // this takes some time
    // incrementHalfMinute();
    // incrementOneMinute();
  // }
}
