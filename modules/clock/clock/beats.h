#ifndef BEATS_H
#define BEATS_H

#include <Arduino.h>
#include "clock_config.h"
#include "pots.h"

int readingToBpm(int reading);
int8_t readingToSubtempo(int reading);
int8_t readingToSwing(int reading);
bool* pinsShouldBeHigh(unsigned long delta, unsigned long msPerBeat, int8_t *subtempo, int8_t *swing);
bool rootShouldBeHigh(unsigned long msPerBeat, int8_t *subtempo, int8_t swing);
bool subtempoShouldBeHigh(unsigned long msPerBeat, int8_t subtempo, int8_t swing, bool rootHigh, uint8_t rootBeatsPassed);
bool applySwing(int8_t swingVal, unsigned long msPerSubtempoBeat, unsigned long msSinceLastSubtempoBeat, bool onOddBeat);

#endif