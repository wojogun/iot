#include <ESP32Servo.h>
#include "mod_mqtt.h"

#define servoPin 13
Servo myservodoor;

constexpr auto TOPIC_DOOR_STATUS = "resort/house2/door/status";


void initDoor() {
  Serial.begin(9600);

	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservodoor.setPeriodHertz(50);    // standard 50 hz servo
	myservodoor.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep

  delay(200);
}

void openDoor() {
    myservodoor.write(180);
	publishMqtt(TOPIC_DOOR_STATUS, "OFFEN");
    delay(200);
}

void closeDoor(){  
    myservodoor.write(0);
	publishMqtt(TOPIC_DOOR_STATUS, "GESCHLOSSEN");
    delay(200);
}