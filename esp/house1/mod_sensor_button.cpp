#include <Arduino.h>
#include "hardware.h"
#include "mod_logic_storm.h"
#include "mod_sensor_button.h"
#include "mod_sensor_lcd.h"
#include "mod_sensor_wind.h"

// BTN1 = Storm Mode On, Off, Auto
// - 1x short click  -> STORM FORCED ON
// - 2x short click  -> STORM FORCED OFF
// - long press      -> STORM AUTO
// Bleibt immer aktiv

// BTN2: Wind Simulation 1x = 30%, 2x = 80%, long = 0%
// - 1x short click  -> WIND 30%
// - 2x short click  -> WIND 80% (Über Schwellwert)
// - long press      -> WIND 0%  (Off)
// Ignoriert wenn Simulation aktiv

static const unsigned long DEBOUNCE_MS = 35;
static const unsigned long DOUBLECLICK_GAP_MS = 400;
static const unsigned long LONGPRESS_MS = 2200;

// ========= STORM ===========
static inline bool isDown(bool reading)
{
    return reading == LOW;
};

struct BtnCtx
{
    bool lastReading = HIGH;
    unsigned long lastChangeMs = 0;

    bool pressed = false;
    unsigned long pressStartMs = 0;
    bool longHandled = false;

    uint8_t clickCount = 0;
    unsigned long lastReleaseMs = 0;
};

static BtnCtx b1;
static BtnCtx b2;

static void handleBtn1Storm()
{
    bool reading = digitalRead(PIN_BTN1);

    // debounce
    if (reading != b1.lastReading)
    {
        b1.lastReading = reading;
        b1.lastChangeMs = millis();
    }
    if (millis() - b1.lastChangeMs < DEBOUNCE_MS)
        return;

    bool down = isDown(reading);

    if (down && !b1.pressed)
    {
        b1.pressed = true;
        b1.pressStartMs = millis();
        b1.longHandled = false;
    }

    // long press -> AUTO
    if (b1.pressed && down && !b1.longHandled)
    {
        if (millis() - b1.pressStartMs >= LONGPRESS_MS)
        {
            b1.longHandled = true;
            b1.clickCount = 0;

            setStormModeAuto();
            printTempLcd("STORM MODE", "AUTO", 1200);
            refreshStormUi();
            return;
        }
    }

    // release -> count clicks
    if (!down && b1.pressed)
    {
        b1.pressed = false;
        if (!b1.longHandled)
        {
            b1.clickCount++;
            b1.lastReleaseMs = millis();
        }
    }

    // finalizieren click
    if (b1.clickCount > 0 && (millis() - b1.lastReleaseMs) > DOUBLECLICK_GAP_MS)
    {
        if (b1.clickCount == 1)
        {
            setStormModeOn();
            printTempLcd("STORM MODE", "FORCED ON", 1200);
            refreshStormUi();
        }
        else if (b1.clickCount == 2)
        {
            setStormModeOff();
            printTempLcd("STORM MODE", "FORCED OFF", 1200);
            refreshStormUi();
        }
        else
        {
            // falls doch mal mehr -> AUTO als safe default // Haus Sensoren greifen wieder
            setStormModeAuto();
            printTempLcd("STORM MODE", "AUTO", 1200);
            refreshStormUi();
        }
        b1.clickCount = 0;
    }
}

static void handleBtn2Wind()
{
    // NUR BTN2 ignorieren wenn Wind-Simulation aktiv !!!!
    if (isWindSimulationEnabled())
        return;

    bool reading = digitalRead(PIN_BTN2);

    // debounce
    if (reading != b2.lastReading)
    {
        b2.lastReading = reading;
        b2.lastChangeMs = millis();
    }
    if (millis() - b2.lastChangeMs < DEBOUNCE_MS)
        return;

    bool down = isDown(reading);

    if (down && !b2.pressed)
    {
        b2.pressed = true;
        b2.pressStartMs = millis();
        b2.longHandled = false;
    }

    if (b2.pressed && down && !b2.longHandled)
    {
        if (millis() - b2.pressStartMs >= LONGPRESS_MS)
        {
            b2.longHandled = true;
            b2.clickCount = 0;
            setLocalWindPercent(0);
        }
    }

    if (!down && b2.pressed)
    {
        b2.pressed = false;
        if (!b2.longHandled)
        {
            b2.clickCount++;
            b2.lastReleaseMs = millis();
        }
    }

    if (b2.clickCount > 0 && (millis() - b2.lastReleaseMs) > DOUBLECLICK_GAP_MS)
    {
        if (b2.clickCount == 1)
            setLocalWindPercent(30);
        else
            setLocalWindPercent(80);
        b2.clickCount = 0;
    }
}

void initButtons()
{
    b1 = BtnCtx{};
    b2 = BtnCtx{};

    b1.lastReading = digitalRead(PIN_BTN1);
    b2.lastReading = digitalRead(PIN_BTN2);
}

void loopButtons()
{
    handleBtn1Storm();
    handleBtn2Wind();
}