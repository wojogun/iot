#include <LiquidCrystal_I2C.h>
#include "mod_sensor_lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initial text
String lcdOben = "";
String lcdUnten = "";

bool lcdBlinkActive = false;
bool lcdTempActive = false;
bool lcdVisible  = true;
unsigned long lcdBlinkInterval = 500;
unsigned long lcdTempDuration = 0;
unsigned long lcdTempEnde = 0;
unsigned long lcdLastToggle = 0;


void initLcd() {
  lcd.init();
  lcd.backlight();
  lcd.display();   // sicherstellen, dass Display an ist
  lcd.clear();
	lcdLastToggle = millis();
}

void print(const String& oben, const String& unten) {
 	lcdVisible = true;
	lcd.display();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(oben);
	lcd.setCursor(0, 1);
	lcd.print(unten);
}

void printLcd(const String& oben, const String& unten, bool flash) {
	lcdBlinkActive = flash;
	lcdOben = oben;
	lcdUnten = unten;
	print(oben, unten);
	lcdLastToggle = millis();
}

// void printTempLcd(const String& oben, const String& unten, unsigned long dur) {
// 	lcdTempDuration = dur;
// 	lcdTempActive   = true;
// 	lcdTempEnde     = millis() + dur;
// 	print(oben, unten);
// 	// lcdLastToggle = millis();
// }

// void lcdUpdate() {
// 	unsigned long now = millis();
//     if (lcdBlinkActive && !lcdTempActive) {
// 		if (now - lcdLastToggle >= lcdBlinkInterval) {
// 			lcdLastToggle = now;
// 			lcdVisible = !lcdVisible;
// 			lcdVisible ? lcd.display() : lcd.noDisplay();
// 		}
// 	}
// 	if (lcdTempActive && now >= lcdTempEnde) {
// 	lcdTempActive = false;
//     lcdVisible = true;
//     lcd.display();
//     print(lcdOben, lcdUnten);
// 	}
// }

void printTempLcd(const String& oben, const String& unten, unsigned long dur) {
	lcdTempDuration = dur;
	print(oben, unten);
	lcdLastToggle = millis();
}

void lcdUpdate() {
	unsigned long now = millis();
    if (lcdBlinkActive) {
		if (now - lcdLastToggle >= lcdBlinkInterval) {
			lcdLastToggle = now;
			lcdVisible = !lcdVisible;
			lcdVisible ? lcd.display() : lcd.noDisplay();
		}
	}
	if (lcdTempActive) {
		if (now - lcdLastToggle >= lcdTempDuration) {
			lcdLastToggle = now;
			print(lcdOben, lcdUnten);
		}
	}
}
