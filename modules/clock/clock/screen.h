#ifndef SCREEN_H
#define SCREEN_H

// TODO: do i actually need to include wire here,
#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h>
#include "clock_config.h"
#include "tap.h"
#include "pawfont.h"

// TODO: how do i like. make some of these private. is it possible? do i even care?
void initScreen();
void initUI(int bpm, int8_t *subtempo, int8_t *swing);
void drawManifesto();
void drawStaticElements();
void drawBpm(int bpm);
void drawSubtempo(int8_t subtempo, uint8_t idx);
void drawSwing(int8_t swing, uint8_t idx);
void drawState(TapState state);
__FlashStringHelper* bpmToPrefix(int b);
__FlashStringHelper* subtempoToPrefix(int s);
__FlashStringHelper* swingToPrefix(int w);

#endif