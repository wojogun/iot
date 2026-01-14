#ifndef MOD_SENSOR_BUTTON_H
#define MOD_SENSOR_BUTTON_H

// BTN1 = Storm Mode On, Off, Auto
// - 1x short click  -> STORM FORCED ON
// - 2x short click  -> STORM FORCED OFF
// - long press      -> STORM AUTO 
// Bleibt immer aktiv  


// BTN2: Wind Simulation 1x = 30%, 2x = 80%, long = 0%
// - 1x short click  -> WIND 30%
// - 2x short click  -> WIND 80% (Über Schwellwert)
// - long press      -> WIND 0%  (Off) 
// Wichtig: Button2 wird IGNORIERT wenn simulation ENABLED ist


void initButtons();
void loopButtons();

#endif
