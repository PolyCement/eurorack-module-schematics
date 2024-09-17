// stuff for printing to the display
#include "screen.h"

// TODO: ...can this be a const?
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

// tile for line drawing since i can't find a built-in font that has this one
// maybe i'll bake it into the custom font at some point,
const uint8_t verticalLineTile[8] = {0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00};

// set up the screen,
void initScreen() {
  u8x8.begin();

  // doktor! turn off my internal pull-up resistors!
  // TODO: uh do i still need this? i assume so since i'm still using wire??
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);

  // set the font (just once at the start is enough)
  u8x8.setFont(pawfont);

  // print the manifesto
  drawManifesto();
}

// print the bits of the display that won't change
void initUI(int bpm, int8_t *subtempo, int8_t *swing) {
  drawStaticElements();
  drawStyle(0);
  drawBpm(bpm);
  drawSubtempo(subtempo[0], 0);
  drawSubtempo(subtempo[1], 1);
  drawSwing(swing[0], 0);
  drawSwing(swing[1], 1);
  drawSwing(swing[2], 2);
}

// print the manifesto
// (censored for github ;3)
void drawManifesto() {
  u8x8.setCursor(1, 0);
  u8x8.print(F("PUPPYGIRLS"));
  u8x8.setCursor(6, 2);
  u8x8.print(F("FOR LIFE!"));
  u8x8.setCursor(1, 4);
  u8x8.print(F("T4T IS"));
  u8x8.setCursor(7, 6);
  u8x8.print(F("ETERNAL!"));
}

// draws parts of the screen that won't change
void drawStaticElements() {
  u8x8.clear();
  u8x8.setCursor(0, 0);
  u8x8.print(F(">>>"));
  u8x8.setCursor(13, 0);
  u8x8.print(F("<<<"));
  u8x8.setCursor(0, 2);
  u8x8.print(F("D"));
  u8x8.setCursor(0, 4);
  u8x8.print(F("S1"));
  u8x8.setCursor(0, 6);
  u8x8.print(F("S2"));
  // gay little trick to draw a line in text-only mode
  for (uint8_t yPos = 2; yPos < 8; yPos++) {
    u8x8.drawTile(3, yPos, 1, verticalLineTile);
    u8x8.drawTile(10, yPos, 1, verticalLineTile);
  }
}

// print the root bpm
void drawBpm(int bpm) {
  u8x8.setCursor(5, 2);
  // TODO: would this be faster if i did a single print? hmmm....
  u8x8.print(bpmToPrefix(bpm));
  u8x8.print(bpm);
}

// print the subtempo
void drawSubtempo(int8_t subtempo, uint8_t idx) {
  u8x8.setCursor(5, 4 + 2 * idx);
  u8x8.print(subtempoToPrefix(subtempo));
  u8x8.print(max(abs(subtempo), 1));
}

// print the swing amount
void drawSwing(int8_t swing, uint8_t idx) {
  u8x8.setCursor(12, 2 + 2 * idx);
  u8x8.print(swingToPrefix(swing));
  u8x8.print(swing);
}

// print the swing amount
void drawStyle(uint8_t style) {
  u8x8.setCursor(5, 0);
  // this should probably be a case statement,
  if (style == 0) {
    u8x8.print(F("NORMAL"));
  } else if (style == 1) {
    u8x8.print(F("TAPPED"));
  } else if (style == 2) {
    u8x8.print(F("TAP IN"));
  } else if (style == 3) {
    u8x8.print(F(" HOLD "));
  } else {
    u8x8.print(F("SCARY!"));
  }
}

// return the prefix to print before each type of value to make sure the padding is normal
// TODO: these are clownshoes bullshit. they work but i don't like it.
__FlashStringHelper* bpmToPrefix(int b) {
  if (b < 100) return F("  ");
  return F(" ");
}

__FlashStringHelper* subtempoToPrefix(int s) {
  if (s < -9) return F("1/");
  if (s < -1) return F(" 1/");
  if (s < 10) return F("   ");
  return F("  ");
}

__FlashStringHelper* swingToPrefix(int w) {
  if (w < -99) return F("");
  if (w < -9 || w > 99) return F(" ");
  if (w < 0 || w > 9) return F("  ");
  return F("   ");
}
