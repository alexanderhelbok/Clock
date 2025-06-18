#include <Arduino.h>
#include "BasicStepperDriver.h"
#include <Wire.h>
#include "Wifi.h"
#include "NTP.h"
#include "Calc.h"
#include "Sleep.h"


// Setup Wifi and NTP Server +++++++++++++++++
// char ssid[] = "Wirtschaftshilfe fuer Studenten";  //  your network SSID (name)
// char pass[] = "9ter12ter1949";       // your network password
char ssid[] = "Free VBucks";  //  your network SSID (name)
char pass[] = "Hallo123";       // your network password

unsigned int localPort = 2390;      // local port to listen for UDP packets

const char* ntpServerName = "0.at.pool.ntp.org";

const int UTCTimeoffset = 2;    // Timezone offset compared to UTC at Greenwich Meridian (GMT)


// Pindefinitions Stepperdriver
#define Stepper_dir 13  // direction
#define Stepper_step 12 // step
#define Stepper_en 14   // enable

#define Hall_Hour_pin 5 // Hall sensor pin
#define Hall_Minute_pin 4 // Hall sensor pin

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
const unsigned long stepper_oneMin = 4096; // measured in practice
const unsigned long stepper_halfMin = 2048; // half min
const unsigned long stepper_quarterMin = 1024; //4 min

const unsigned long stepper_oneSec = stepper_oneMin/60; // steps per second for one minute 

unsigned int currentHour = 0;
unsigned int currentMinute = 0;

unsigned long sleep_time = 66170;

void incrementOneMinute();
void incrementHalfMinute();

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Start stepper driver 
  stepper.begin(Stepper_Speed_Initial, MICROSTEPS);
  pinMode(Stepper_en, OUTPUT);
  digitalWrite(Stepper_en, LOW);

  // turn the stepper until both are at 12
  while ((digitalRead(Hall_Minute_pin) || digitalRead(Hall_Hour_pin))) {
    stepper.move(Clock_direction*stepper_quarterMin);
  }
  // compensate for Hall sensor offset
  incrementHalfMinute(); 

  // Connect to WiFi
  WifiConnect(ssid, pass);

  // Get current Time from NTP Server and correct time
  int time = getTime(localPort, UTCTimeoffset, ntpServerName);
  Serial.println("Current time from NTP server: ");
  // print time in HH:MM format
  Serial.print(time / 3600);
  Serial.print(":");
  Serial.println((time / 60) % 60);

  int now = TimeCorrection(time, 62.65);

  Serial.println("Corrected time: ");
  // print corrected time in HH:MM format
  Serial.print(now / 3600);
  Serial.print(":");
  Serial.println((now / 60) % 60);


  // Disconnect Wifi and enter modem sleep, drops 75 mA
  WifiDisconnect();

  // move to desired time  
  digitalWrite(Stepper_en, LOW);
  if (time <= 21276){
    stepper.move(Clock_direction*now*stepper_oneSec);
  }
  else{
    stepper.move(-Clock_direction*now*stepper_oneSec);
  }
  stepper.setRPM(Stepper_Speed);
  digitalWrite(Stepper_en, HIGH);
  currentHour = (now / 3600) % 24; // calculate current hour
  currentMinute = (now / 60) % 60; // calculate current minute

  light_sleep(sleep_time);
}


void incrementQuarterMinute() {
  digitalWrite(Stepper_en, LOW);
  stepper.move(Clock_direction*stepper_quarterMin);
  digitalWrite(Stepper_en, HIGH);
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

  currentMinute += 1;
  if (currentMinute >= 60) {
    currentMinute = 0;
    currentHour += 1;
    if (currentHour >= 24) {
      currentHour = 0;
    }
  }
}

void stepperMoveFast(unsigned long steps) {
  digitalWrite(Stepper_en, LOW);
  stepper.setRPM(Stepper_Speed_Initial);
  stepper.move(Clock_direction * steps);
  stepper.setRPM(Stepper_Speed);
  digitalWrite(Stepper_en, HIGH);
}

void loop() {
  incrementOneMinute();
  // Serial.print("Current time: ");
  // Serial.print(currentHour);
  // Serial.print(":");
  // Serial.println(currentMinute);

  // Serial.print("Hall Hour: ");
  // Serial.println(digitalRead(Hall_Hour_pin));
  // Serial.print("Hall Minute: ");
  // Serial.println(digitalRead(Hall_Minute_pin));
  
  if (!digitalRead(Hall_Hour_pin) && !digitalRead(Hall_Minute_pin)) {
    // compare time with NTP server
    WifiConnect(ssid, pass);
    int time = getTime(localPort, UTCTimeoffset, ntpServerName);
    WifiDisconnect();
    // Serial.print("Current time: ");
    // Serial.println(time);
    int hour = (time / 3600) % 24; // calculate current hour
    int minute = (time / 60) % 60; // calculate current minute

    // update current time
    currentHour = hour;
    currentMinute = minute;

    int timeDiff = (hour % 12) * 60 + minute;

    // Serial.print("Time difference: ");
    // Serial.println(timeDiff);

    stepperMoveFast(stepper_oneMin * timeDiff);
  }

  light_sleep(sleep_time);
  // ESP.deepSleep(10e6);
}
