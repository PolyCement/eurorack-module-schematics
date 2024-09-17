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
#define POT_VAL_CHANGE_THRESHOLD 3

// pin mappings
// the code assumes there will be:
// - 1 root tempo pot
// - n subtempo pots
// - n + 1 swing pots
// and that these will be listed in this order in potLookup
// basically what i'm saying is that while u can change NUM_SWINGS here directly, u really shouldnt...
#define NUM_SUBTEMPOS 2
#define NUM_SWINGS NUM_SUBTEMPOS + 1
const uint8_t potLookup[] = { A0, A2, A6, A1, A3, A7 };

// bpm config
// negative subtempos are fractional (eg. a value of -16 will be treated as 1/16)
// if the values seem to "wobble", drop the ranges a bit (or improve ur grounding ;3)
#define MIN_BPM 40
#define MAX_BPM 280
#define MIN_SUBTEMPO -32
#define MAX_SUBTEMPO 32
#define MIN_SWING -100
#define MAX_SWING 100

// tap tempo config. timeouts are in ms
#define TAPS_TO_ACTIVATE 8
#define RESET_TIMEOUT 2000
#define MAX_TAP_TEMPO 400
#define DEBOUNCE_TIMEOUT 0
#define HOLD_TIME 1000

#endif