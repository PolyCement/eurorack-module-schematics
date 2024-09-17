#ifndef TAP_H
#define TAP_H

#include <Arduino.h>
#include "clock_config.h"

enum TapState { inactive, tapping, tap, hold };

TapState getTapState(unsigned long millisNow);
int getTapTempo();
int getStoredTapTempo();
void updateButtonState(unsigned long millisNow);
int calcRollingAverageDifference(unsigned long millisNow);

#endif