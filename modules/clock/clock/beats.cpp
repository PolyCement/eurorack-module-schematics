// stuff for calculating beat-related stuff
#include "beats.h"

unsigned long msSinceLastBeat = 0;
// stuff for applying swing,
bool globalSwingBeat = false;
uint8_t rootBeatsPassed[NUM_SUBTEMPOS];

// TODO: idk if i wanna keep these im just doin it as part of the restructure
// probably i'll just switch back to how it was and have the min/max values passed in on init?
int readingToBpm(int reading) {
  return potValToRange(reading, MIN_BPM, MAX_BPM);
}

int8_t readingToSubtempo(int reading) {
  return potValToRange(reading, MIN_SUBTEMPO, MAX_SUBTEMPO);
}

int8_t readingToSwing(int reading) {
  return potValToRange(reading, MIN_SWING, MAX_SWING);
}

// return an array indicating if the pins should be high or not
bool* pinsShouldBeHigh(unsigned long delta, unsigned long msPerBeat, int8_t *subtempo, int8_t *swing) {
  static bool pinStates[NUM_OUTPUTS];
  msSinceLastBeat += delta;
  pinStates[0] = rootShouldBeHigh(msPerBeat, subtempo, swing[0]);
  for (int subtempoNum = 0, swingNum = 1; subtempoNum < NUM_SUBTEMPOS; subtempoNum++, swingNum++) {
    pinStates[swingNum] = subtempoShouldBeHigh(msPerBeat, subtempo[subtempoNum], swing[swingNum], pinStates[0], rootBeatsPassed[subtempoNum]);
  }
  return pinStates;
}

bool rootShouldBeHigh(unsigned long msPerBeat, int8_t *subtempo, int8_t swing) {
  if (msSinceLastBeat >= msPerBeat) {
    if (DEBUG_ENABLED) Serial.println(msSinceLastBeat);
    // track when the last beat *should have been* instead of when it actually was so we don't drift out of sync
    // (that's kind of the whole point of this maneuver,)
    msSinceLastBeat %= msPerBeat;
    // toggle global swing tracker
    globalSwingBeat = !globalSwingBeat;
    // for any fractional subtempos (ie. < 0 (actually < -1 since -1 is equivalent to x1)),
    // increment the root beats passed (we're going up to 2x the relative tempo so we know if we're on an odd subtempo beat)
    for (int subtempoNum = 0; subtempoNum < NUM_SUBTEMPOS; subtempoNum++) {
      if (subtempo[subtempoNum] < -1) {
        rootBeatsPassed[subtempoNum] = (rootBeatsPassed[subtempoNum] + 1) % (-2 * subtempo[subtempoNum]);
      }
    }
  }
  return applySwing(swing, msPerBeat, msSinceLastBeat, globalSwingBeat);
}

bool subtempoShouldBeHigh(unsigned long msPerBeat, int8_t subtempo, int8_t swing, bool rootHigh, uint8_t rootBeatsPassed) {
  // TODO: move to top? or somethin idk theres a LOT of shit here im sure i can simplify harder
  unsigned long msPerSubtempoBeat = msPerBeat;
  unsigned long msSinceLastSubtempoBeat = msSinceLastBeat;
  bool onOddBeat = globalSwingBeat;
  bool subShouldBeHigh = rootHigh;

  // if the subtempo is a multiple (ie. > 1), split the root ms per beat into sections
  if (subtempo > 1) {
    msPerSubtempoBeat = msPerBeat / subtempo;
    msSinceLastSubtempoBeat = msSinceLastBeat % msPerSubtempoBeat;
    // check if the current beat is odd so we can calculate how long it's been since the last flat beat
    onOddBeat = (msSinceLastBeat / msPerSubtempoBeat) % 2 == 1;
    // invert if the subtempo is odd and we're on global swing
    if (subtempo % 2 == 1 && globalSwingBeat) {
      onOddBeat = !onOddBeat;
    }
  }

  // if the subtempo is fractional (ie. < -1), use the number of beats passed to figure out where we're at
  if (subtempo < -1) {
    msPerSubtempoBeat = msPerBeat * subtempo * -1;
    msSinceLastSubtempoBeat = ((rootBeatsPassed % (-1 * subtempo)) * msPerBeat) + msSinceLastBeat;
    // check if the current beat is odd so we can calculate how long it's been since the last flat beat
    onOddBeat = ((rootBeatsPassed * msPerBeat) + msSinceLastBeat) >= msPerSubtempoBeat;
  }

  return applySwing(swing, msPerSubtempoBeat, msSinceLastSubtempoBeat, onOddBeat);
}

// apply swing to determine if the pin should be high or not
bool applySwing(int8_t swingVal, unsigned long msPerSubtempoBeat, unsigned long msSinceLastSubtempoBeat, bool onOddBeat) {
  // calc lengths of flat and swing beats
  unsigned long msPerFlatBeat = (msPerSubtempoBeat * (swingVal + MAX_SWING) * 2) / (MAX_SWING - MIN_SWING);
  unsigned long msPerSwingBeat = (msPerSubtempoBeat * 2) - msPerFlatBeat;
  // now calc ms since last *flat* subtempo beat
  unsigned long msSinceLastFlatSubtempoBeat = msSinceLastSubtempoBeat;
  if (onOddBeat) {
    msSinceLastFlatSubtempoBeat += msPerSubtempoBeat;
  }
  // NOW we can figure out the clock state.
  return msSinceLastFlatSubtempoBeat < (msPerFlatBeat / 2)
           || (msSinceLastFlatSubtempoBeat > msPerFlatBeat && msSinceLastFlatSubtempoBeat < (msPerFlatBeat + (msPerSwingBeat / 2)));
}