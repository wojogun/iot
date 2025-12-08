#include <ESP32Servo.h>

#define servoPin 13
Servo myservodoor;


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
    delay(200);
}

void closeDoor(){  
    myservodoor.write(0);
    delay(200);
}