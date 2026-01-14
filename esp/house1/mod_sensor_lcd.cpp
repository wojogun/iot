#include <LiquidCrystal_I2C.h>
#include "mod_sensor_lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool lcdBlinkActive = false;
bool lcdTempActive = false;
bool lcdVisible = true;
unsigned long lcdBlinkInterval = 500;
unsigned long lcdTempDuration = 0;
unsigned long lcdLastToggle = 0;
String lcdOben = "";
String lcdUnten = "";

void initLcd()
{
	lcd.init();
	lcd.backlight();
	lcd.display();             // sicherstellen, dass Display an ist
	lcd.clear();
	lcdLastToggle = millis();
}

void print(const String &oben, const String &unten)
{
	lcdBlinkActive = false;
	lcdTempActive = false;

	lcdVisible = true;
	lcd.display();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(oben);
	lcd.setCursor(0, 1);
	lcd.print(unten);
}

void printLcd(const String &oben, const String &unten)
{
  printLcd(oben, unten, false);
}

void printLcd(const String &oben, const String &unten, bool flash)
{
	lcdBlinkActive = flash;
	lcdOben = oben;
	lcdUnten = unten;
	print(oben, unten);
	lcdLastToggle = millis();
}

void printTempLcd(const String &oben, const String &unten, unsigned long dur)
{
	lcdTempActive = true;
	lcdTempDuration = dur;
	print(oben, unten);
	lcdLastToggle = millis();
}

void lcdUpdate()
{
	unsigned long now = millis();
	if (lcdBlinkActive)
	{
		if (now - lcdLastToggle >= lcdBlinkInterval)
		{
			lcdLastToggle = now;
			lcdVisible = !lcdVisible;
			lcdVisible ? lcd.display() : lcd.noDisplay();
		}
	}
	if (lcdTempActive)
	{
		if (now - lcdLastToggle >= lcdTempDuration)
		{
			lcdLastToggle = now;
			print(lcdOben, lcdUnten);
		}
	}
}
