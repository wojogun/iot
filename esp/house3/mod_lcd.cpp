#include <LiquidCrystal_I2C.h>
#include "mod_lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Permanenter Text
String lcdOben = "";
String lcdUnten = "";

// Blink-Status
bool lcdBlinkActive = false;
bool lcdVisible  = true;
unsigned long lcdBlinkInterval = 500;
unsigned long lcdLastBlink = 0;

// Temporärer Text
bool lcdTempActive = false;
unsigned long lcdTempDuration = 0;
unsigned long lcdTempEnde = 0;

void initLcd() {
  lcd.init();
  lcd.backlight();
  lcd.display();   // sicherstellen, dass Display an ist
  lcd.clear();
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
	lcdOben        = oben;
	lcdUnten       = unten;
	if (flash) lcdLastBlink  = millis();
	print(oben, unten);
}

void printTempLcd(const String& oben, const String& unten, unsigned long dur) {
	lcdTempDuration = dur;
	lcdTempActive   = true;
  lcdTempEnde     = millis() + dur;
	print(oben, unten);
}

void loopLcd() {
	unsigned long now = millis();
  if (lcdBlinkActive && !lcdTempActive) {
		if (now - lcdLastBlink >= lcdBlinkInterval) {
			lcdLastBlink = now;
			lcdVisible = !lcdVisible;
			lcdVisible ? lcd.display() : lcd.noDisplay();
		}
	}
	if (lcdTempActive && now >= lcdTempEnde) {
    lcdTempActive = false;
    lcdVisible = true;
    lcd.display();
    print(lcdOben, lcdUnten);
  }
}
