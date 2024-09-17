#ifndef POTS_H
#define POTS_H

#include <Arduino.h>
#include "clock_config.h"

void initPots();
void updatePotReadings();
int getBpmReading();
int getSubtempoReading(uint8_t subtempoNum);
int getSwingReading(uint8_t swingNum);
int potValToRange(int potVal, int minRange, int maxRange);
int calcRollingAverage(int potNum);
bool valChangePastThreshold(int currentVal, int recordedVal, int threshold);

#endif