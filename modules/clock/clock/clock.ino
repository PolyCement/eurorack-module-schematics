// my freakeng clock module !
// the code is bad because i haven't used c in over a decade. plus im just a small dog.
#include "clock_config.h"
#include "pots.h"
#include "tap.h"
#include "beats.h"
#include "screen.h"

#define DEBUG_ENABLED false

// bpm and milliseconds per beat
// 40 - 280 should be enough. once noise is dealt with i might try and push for 20 - 300 tho ;3
int bpm = 0;
unsigned long msPerBeat = 0;

// subtempos. i'm not calculating the bpm for these, instead it's a relative bpm between 1/32 and 32x the root tempo
// the ms per beat is calculated relative to the root tempo too. i'm gonna do it on the fly for now but maybe i'll
// add an array for those later too
int8_t relativeTempo[NUM_SUBTEMPOS];

// swing ur shit girl !
// i might wanna make this like +-95 instead of +-100 to stop the swing beat
// overlapping the flat beat...? idk i'll experiment once it's built
int8_t swing[NUM_SWINGS];

// time delta stuff
unsigned long millisOnLastLoop = 0;
unsigned long delta = 0;

// display state tracking
int displayBpm = 0;
TapState displayTapState = TapState::inactive;

void setup() {
  if (DEBUG_ENABLED) Serial.begin(9600);

  initScreen();

  // how much sram do we actually have free after the buffer is set up?
  if (DEBUG_ENABLED) {
    int size = 2048;
    byte *buf;
    while ((buf = (byte *) malloc(--size)) == NULL);
    free(buf);
    Serial.print(size);
  }

  // set up inputs and outputs
  for (int pinNum = 0; pinNum < NUM_OUTPUTS; pinNum++) {
    pinMode(pinLookup[pinNum], OUTPUT);
  }
  pinMode(BUTTON_PIN, INPUT);

  // get initial pot values
  initPots();

  // init bpm, subtempos and swing vals

  displayBpm = bpm = readingToBpm(getBpmReading());
  msPerBeat = 60000 / (unsigned long) bpm;
  for (int subtempoNum = 0; subtempoNum < NUM_SUBTEMPOS; subtempoNum++) {
    relativeTempo[subtempoNum] = readingToSubtempo(getSubtempoReading(subtempoNum));
  }
  for (int swingNum = 0; swingNum < NUM_SWINGS; swingNum++) {
    swing[swingNum] = readingToSwing(getSwingReading(swingNum));
  }

  // let the manifesto sit for a bit then get this thing goin
  delay(2000);
  initUI(displayBpm, relativeTempo, swing);
}

void loop() {
  updatePotReadings();

  unsigned long millisNow = millis();
  delta = millisNow - millisOnLastLoop;

  // convert pot val (or tap tempo) to bpm
  // TODO: show what the bpm will be in the usual place, even in tapping and hold states
  // (its the least confusing way i can think of to do it)
  // i guess if i really wanted i could put a * before it or somethin idk, i got 1 character of space to use there
  int newBpm, newDisplayBpm;
  TapState newTapState = getTapState(millisNow);
  if (newTapState == TapState::tap) {
    // if tap state is tap, set bpm and display from pot
    newDisplayBpm = newBpm = getStoredTapTempo();
  } else if (newTapState == TapState::tapping) {
    // if tap state is tapping, hold bpm but set display bpm from tap
    newBpm = bpm;
    newDisplayBpm = getTapTempo();
  } else if (newTapState == TapState::hold) {
    // if tap state is hold, hold bpm but set display bpm from pot
    newBpm = bpm;
    newDisplayBpm = readingToBpm(getBpmReading());
  } else {
    // tap state is inactive, set bpm and display from pot
    newDisplayBpm = newBpm = readingToBpm(getBpmReading());
  }

  // if the bpm changed, recalc stuff
  if (newBpm != bpm) {
    bpm = newBpm;
    msPerBeat = 60000 / (unsigned long) bpm;
  }
  // if the display bpm (different thing!) changed, update the display
  if (newDisplayBpm != displayBpm) {
    displayBpm = newDisplayBpm;
    drawBpm(displayBpm);
  }
  // same for style
  if (newTapState != displayTapState) {
    displayTapState = newTapState;
    drawState(displayTapState);
  }

  // same type of shit but for subtempo & swing pots
  for (uint8_t subtempoNum = 0; subtempoNum < NUM_SUBTEMPOS; subtempoNum++) {
    int8_t newRelativeTempo = readingToSubtempo(getSubtempoReading(subtempoNum));
    if (newRelativeTempo != relativeTempo[subtempoNum]) {
      relativeTempo[subtempoNum] = newRelativeTempo;
      drawSubtempo(relativeTempo[subtempoNum], subtempoNum);
    }
  }
  for (uint8_t swingNum = 0; swingNum < NUM_SWINGS; swingNum++) {
    int8_t newSwing = readingToSwing(getSwingReading(swingNum));
    if (newSwing != swing[swingNum]) {
      swing[swingNum] = newSwing;
      drawSwing(swing[swingNum], swingNum);
    }
  }

  // set pin states
  // TODO: track state and only write if we need to update...?
  bool* pinStates = pinsShouldBeHigh(delta, msPerBeat, relativeTempo, swing);
  for (uint8_t pinNum = 0; pinNum < NUM_OUTPUTS; pinNum++) {
    digitalWrite(pinLookup[pinNum], pinStates[pinNum]);
  }

  millisOnLastLoop = millisNow;
}
