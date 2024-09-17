// putting this in another file too,
#include "tap.h"

// bpm calc stuff
unsigned long tapTimes[TAPS_TO_ACTIVATE];
uint8_t tapIndex = 0;
unsigned long totalDiff = 0;
int tapTempo = 0;
int storedTapTempo = 0;

// stuff for debouncing the button
bool currentState = false;
bool lastReadState = false;
unsigned long timeAtButtonStateChange = 0;

// actual button state
// TODO: thas a lot of globals.. maybe i should bump button handling to another file,
unsigned long buttonDownTime = 0;
bool buttonTapped = false;
bool buttonHeld = false;

// state management stuff,
TapState tapState = TapState::inactive;
bool wasActiveBeforeTapping = false;

// update and return tap state
TapState getTapState(unsigned long millisNow) {
  // update button state
  // TODO: make the function do that in a less clownshoes way,
  updateButtonState(millisNow);

  // if the button is held, reset and return
  if (buttonHeld) {
    tapTempo = 0;
    totalDiff = 0;
    tapState = TapState::hold;
    return tapState;
  }

  // if we were in hold state and the button isnt held anymore, set it back to inactive
  if (tapState == TapState::hold && !buttonHeld) {
    // reset these too so we don't end up triggering the tap state with consecutive holds lmao
    tapIndex = 0;
    tapTempo = 0;
    totalDiff = 0;
    wasActiveBeforeTapping = false;
    tapState = TapState::inactive;
  }

  // if it's been longer than TIMEBETWEEN or whatever then reset and revert state to tap/inactive as appropriate
  if (millisNow - tapTimes[(tapIndex + TAPS_TO_ACTIVATE - 1) % TAPS_TO_ACTIVATE] > RESET_TIMEOUT) {
    tapIndex = 0;
    tapTempo = 0;
    totalDiff = 0;
    if (wasActiveBeforeTapping) {
      tapState = TapState::tap;
    } else {
      tapState = TapState::inactive;
    }
  }

  // otherwise if a tap wasn't just registered nothin changed
  if (!buttonTapped) return tapState;

  // if we're here there was a tap. if it was the first, set shit up and return
  // NOTE: i'm not setting state to tapping here cos it blocks clean inactive -> hold transitions
  if (tapIndex == 0) {
    tapTimes[0] = millisNow;
    tapIndex = 1;
    totalDiff = 0;
    wasActiveBeforeTapping = tapState == TapState::tap;
    return tapState;
  }

  // im not looping the tapIndex (rename to tapCount?) so once its 8 or more we're in tap mode
  if (tapIndex >= TAPS_TO_ACTIVATE - 1) {
    wasActiveBeforeTapping = true;
    tapState = TapState::tap;
  } else {
    // if we're here then we're on tap 1 - 7 so activate tapping mode
    tapState = TapState::tapping;
  }

  // calculate the average, cap it, and store it
  tapTempo = constrain(calcRollingAverageDifference(millisNow), MIN_BPM, MAX_BPM);

  // if we're in tap mode then start updating the stored tempo too
  if (tapState == TapState::tap) {
    storedTapTempo = tapTempo;
  }

  // finally return the state
  return tapState;
}

// returns the tapped tempo (rename to "displayTapTempo"?)
int getTapTempo() {
  return tapTempo;
}

// returns the tapped tempo
int getStoredTapTempo() {
  return storedTapTempo;
}

// TODO: split tap/hold code somehow or rename this
void updateButtonState(unsigned long millisNow) {
  bool newState = digitalRead(BUTTON_PIN);
  buttonTapped = false;

  if (newState != lastReadState) {
    // on change, just note the new state and when it changed
    timeAtButtonStateChange = millisNow;
    lastReadState = newState;
  } else if ((millisNow - timeAtButtonStateChange) > DEBOUNCE_DELAY) {
    // no change... if the button's been down for longer than the debounce time, update state
    // i considered flipping this so the debouncing happens after the press is registered but this way handles noise better,
    // (having said that i have the debounce time at 0 rn so....)
    if (!currentState && newState) {
      if (DEBUG_ENABLED) Serial.println(F("TAP!"));
      buttonTapped = true;
      buttonDownTime = millisNow;
    } else if (currentState && !newState) {
      buttonDownTime = 0;
      buttonHeld = false;
    } else if (currentState && millisNow - buttonDownTime > HOLD_DELAY) {
      buttonHeld = true;
    }
    currentState = newState;
  }
}

// calculates the new rolling average of differences between times
int calcRollingAverageDifference(unsigned long millisNow) {
  // subtract the oldest difference (the difference between the times in the current and next positions)
  if (tapIndex >= TAPS_TO_ACTIVATE)
    totalDiff -= tapTimes[(tapIndex + 1) % TAPS_TO_ACTIVATE] - tapTimes[tapIndex % TAPS_TO_ACTIVATE];
  // record the current time
  tapTimes[tapIndex % TAPS_TO_ACTIVATE] = millisNow;
  // add the difference between the current time and the previous time
  totalDiff += tapTimes[tapIndex % TAPS_TO_ACTIVATE] - tapTimes[(tapIndex - 1) % TAPS_TO_ACTIVATE];
  // advance and return the new average
  tapIndex++;
  return ((min(tapIndex, TAPS_TO_ACTIVATE) - 1) * 60000) / totalDiff;
}
