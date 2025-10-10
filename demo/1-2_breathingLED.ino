#include <Arduino.h>
#define led_y 12
void setup() {
  pinMode(led_y, OUTPUT);
}

void loop() {
  for(int i = 0; i < 255; i++) {
    analogWrite(led_y, i);
    delay(3);
  }
  for(int i = 255; i > 0; i--) {
    analogWrite(led_y, i);
    delay(3);
  }
}