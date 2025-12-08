#ifndef PARTYLOGIC_H
#define PARTYLOGIC_H

enum Mode {
  MODE_NORMAL,
  MODE_PARTY,
  MODE_STORM
};

extern String nextPartyText;
extern Mode currentMode;

void initParty();
void loopParty();      // updateEffects()
void startParty(bool publish);
void stopParty(bool publish);
void startStorm(bool publish);
void stopStorm(bool publish);

#endif
