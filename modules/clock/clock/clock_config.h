#ifndef CLOCK_CONFIG_H
#define CLOCK_CONFIG_H

// shared consts. not sure if this is best practice but it works for me lol
// most of the consts here are only used in a single file (library? i forget c terminology),
// but i put em here anyway to make config easier for other folks who might use this

// enable debug printouts via serial
#define DEBUG_ENABLED false

// pot config
// NUM_READINGS and POT_VAL_CHANGE_THRESHOLD are for pot smoothing. if ur having noise issues u can
// turn each of em up to increase smoothing, but at the cost of responsiveness and precision (respectively)
#define NUM_POTS 6
#define MAX_POT_VAL 1024
#define NUM_READINGS 10
#define POT_VAL_CHANGE_THRESHOLD 2

// pin mappings
// the code assumes there will be:
// - 1 root tempo pot
// - n subtempo pots
// - n + 1 swing pots
// - n + 1 outputs
// and that the pots and outputs will be listed in these orders in the relevant lookup arrays
// basically what i'm saying is that while u can change NUM_SWINGS here directly, u really shouldnt...
#define NUM_SUBTEMPOS 2
#define NUM_SWINGS NUM_SUBTEMPOS + 1
#define NUM_OUTPUTS NUM_SWINGS
#define BUTTON_PIN 4
const uint8_t potLookup[] = { A0, A2, A6, A1, A3, A7 };
const uint8_t pinLookup[] = { 12, 9, 6 };

// bpm config
// negative subtempos are fractional (eg. a value of -16 will be treated as 1/16)
// if the values seem to "wobble", drop the ranges a bit (or improve ur grounding ;3)
// NOTE: all positive vals and negative subtempo val have to be 1 more due to rounding
// (i might fix that later so that the config is less confusing....)
#define MIN_BPM 20
#define MAX_BPM 301
#define MIN_SUBTEMPO -33
#define MAX_SUBTEMPO 33
#define MIN_SWING -100
#define MAX_SWING 101

// tap tempo config. timeouts are in ms
#define TAPS_TO_ACTIVATE 8
#define DEBOUNCE_DELAY 0
#define HOLD_DELAY 1000
#define RESET_TIMEOUT 2000

#endif