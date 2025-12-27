// WIND = SIMULATION
// FAN = AKTOR !! 
// Node-RED Slider “Wind” → simulation/wind → Storm Logic nutzt Wind als Trigger
// Node-RED Slider “Fan” → fan/cmd → steuert echten Fan (für Demo/Visual)
// Optional: In AUTO: fan automatisch proportional zum wind (wenn enabled), aber nur wenn du das willst.
// #include "hardware.h"
// #include "mod_sensor_fan.h"

// static int fanPct = 0;
// static const bool FAN_PWM_INVERTED = true;

// void initFan() {
//   pinMode(PIN_FAN_DIR, OUTPUT);
//   pinMode(PIN_FAN_PWM, OUTPUT);
//   digitalWrite(PIN_FAN_DIR, LOW);
//   analogWrite(PIN_FAN_PWM, 0);// OFF
//   fanPct = 0;
// }

// // static int pctToPwm(int pct) {
// //   if (pct <= 0) return 0;
// //   if (pct >= 100) return 255;
// //   return map(pct, 0, 100, 0, 255);
// // }
// static int pctToPwm(int pct) {
//   pct = constrain(pct, 0, 100);
// // map to 0..255 "normal"
//   int pwm = map(pct, 0, 100, 0, 255);

//   // invert if needed
//   if (FAN_PWM_INVERTED) pwm = 255 - pwm;

//   return pwm;
// }

// // void setFanPercent(int pct) {
// // pct = constrain(pct, 0, 100);

// //   int prev = fanPct;
// //   fanPct = pct;

// //   int pwm = pctToPwm(fanPct);

// //   // OFF condition depends on inversion:
// //   // - not inverted: pwm==0 is OFF
// //   // - inverted: pwm==255 is OFF
// //   bool isOff = FAN_PWM_INVERTED ? (pwm == 255) : (pwm == 0);

// //   if (isOff) {
// //     digitalWrite(PIN_FAN_DIR, LOW);
// //     analogWrite(PIN_FAN_PWM, FAN_PWM_INVERTED ? 255 : 0);
// //     return;
// //   }

// //   // enable/dir
// //   digitalWrite(PIN_FAN_DIR, HIGH);

// // // kickstart when previously OFF
// //   if (prev == 0) {
// //     analogWrite(PIN_FAN_PWM, FAN_PWM_INVERTED ? 0 : 255);
// //     delay(200);
// //   }

// //   analogWrite(PIN_FAN_PWM, pwm);
// // }

// // void setFanPercent(int pct) {
// //   pct = constrain(pct, 0, 100);
// //   int prev = fanPct;
// //   fanPct = pct;

// //   int pwm = pctToPwm(pct);

// //   // OFF (für beide Varianten korrekt)
// //   if (pct == 0) {
// //     digitalWrite(PIN_FAN_DIR, LOW);
// //     analogWrite(PIN_FAN_PWM, FAN_PWM_INVERTED ? 255 : 0);
// //     return;
// //   }

// //   digitalWrite(PIN_FAN_DIR, HIGH);

// //   // Kickstart nur wenn vorher 0%
// //   if (prev == 0) {
// //     analogWrite(PIN_FAN_PWM, FAN_PWM_INVERTED ? 0 : 255);
// //     delay(200);
// //   }

// //   analogWrite(PIN_FAN_PWM, pwm);
// // }
// void setFanPercent(int pct) {
//   pct = constrain(pct, 0, 100);
//   int prev = fanPct;
//   fanPct = pct;

//   int pwm = pctToPwm(fanPct);

//   // OFF level richtig behandeln:
//   int offPwm = FAN_PWM_INVERTED ? 255 : 0;
//   int kickPwm = FAN_PWM_INVERTED ? 0 : 255;

//   if (pwm == offPwm) {
//     digitalWrite(PIN_FAN_DIR, LOW);
//     analogWrite(PIN_FAN_PWM, offPwm);
//     return;
//   }

//   digitalWrite(PIN_FAN_DIR, HIGH);

//   if (prev == 0) {
//     analogWrite(PIN_FAN_PWM, kickPwm);
//     delay(200);
//   }

//   analogWrite(PIN_FAN_PWM, pwm);
// }

// int getFanPercent() {
//   return fanPct;
// }



#include "hardware.h" 
#include "mod_sensor_fan.h" 

static int fanPct = 0; 

void initFan() { 
  pinMode(PIN_FAN_DIR, OUTPUT); 
  pinMode(PIN_FAN_PWM, OUTPUT); 
  digitalWrite(PIN_FAN_DIR, LOW); 
  analogWrite(PIN_FAN_PWM, 0); 
  fanPct = 0; 
} 

static int pctToPwm(int pct) { 
  if (pct <= 0) return 0; 
  if (pct >= 100) return 255; 
  return map(pct, 0, 100, 0, 255); 
} 

void setFanPercent(int pct) { 
  if (pct < 0) pct = 0; 
  if (pct > 100) pct = 100; 
  
  int prev = fanPct; 
  fanPct = pct; 
  int pwm = pctToPwm(fanPct); 
  if (pwm == 0) { 
    digitalWrite(PIN_FAN_DIR, LOW); 
    analogWrite(PIN_FAN_PWM, 0); 
    return; 
  } 
  
  // enable/dir 
  digitalWrite(PIN_FAN_DIR, HIGH); 
  // kickstart wenn vorher aus 
  if (prev == 0) { 
    analogWrite(PIN_FAN_PWM, 255); 
    delay(200); 
  } 
  analogWrite(PIN_FAN_PWM, pwm); 
} 

int getFanPercent() { 
  return fanPct;
 }