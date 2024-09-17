// i'm putting all the pot handling code in here cos there's so much of it,
#include "pots.h"

int readings[NUM_POTS][NUM_READINGS];
uint8_t readIndex = 0;
int total[NUM_POTS];
int recordedPotVal[NUM_POTS];
int averagePotVal[NUM_POTS];

// main functions

// TODO: maybe take the pots and pins as args so this is more reusable? its fine for now tho probably
void initPots() {
  // initialise readings, totals and averages
  for (int potNum = 0; potNum < NUM_POTS; potNum++) {
    int initialPotVal = analogRead(potLookup[potNum]);
    for (int thisReading = 0; thisReading < NUM_READINGS; thisReading++) {
      readings[potNum][thisReading] = initialPotVal;
    }
    total[potNum] = initialPotVal * NUM_READINGS;
    recordedPotVal[potNum] = averagePotVal[potNum] = total[potNum] / NUM_READINGS;
  }
}

// calculate the current rolling average for each pot
void updatePotReadings() {
  for (int potNum = 0; potNum < NUM_POTS; potNum++) {
    averagePotVal[potNum] = calcRollingAverage(potNum);
    // only update the recorded value if it's changed more than the threshold
    if (valChangePastThreshold(averagePotVal[potNum], recordedPotVal[potNum], POT_VAL_CHANGE_THRESHOLD)) {
      recordedPotVal[potNum] = averagePotVal[potNum];
    }
  }
  readIndex = (readIndex + 1) % NUM_READINGS;
}

// TODO: again maybe take a pot number to check instead of having these tied to the pot's purpose?
// doesn't solve the "having to add an offset" issue tho...
// maybe i should just use an enum or somethin
int getBpmReading() {
  return recordedPotVal[0];
}

int getSubtempoReading(uint8_t subtempoNum) {
  return recordedPotVal[subtempoNum + 1];
}

int getSwingReading(uint8_t swingNum) {
  return recordedPotVal[swingNum + 3];
}

// map the given pot value to the range described by the given min and max values
int potValToRange(int potVal, int minRange, int maxRange) {
  return (((float) potVal / MAX_POT_VAL) * (maxRange - minRange)) + minRange;
}

// helper functions
// TODO: these should be private? how do i do that in c++ lol

// calculates the new rolling average
int calcRollingAverage(int potNum) {
  // update pot readings and total, advance index, then calculate new average pot val
  total[potNum] -= readings[potNum][readIndex];
  readings[potNum][readIndex] = analogRead(potLookup[potNum]);
  total[potNum] += readings[potNum][readIndex];
  return total[potNum] / NUM_READINGS;
}

// returns true if currentVal differs from recordedVal by more than threshold
bool valChangePastThreshold(int currentVal, int recordedVal, int threshold) {
  return currentVal < (recordedVal - threshold) || currentVal > (recordedVal + threshold);
}