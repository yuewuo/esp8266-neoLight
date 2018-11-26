#include <Adafruit_NeoPixel.h>
#include "neo_exec.h"
#define TIME_INTV 20  // ms，每一帧的间隔

#define NEO_PIN D1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_N, NEO_PIN, NEO_GRB + NEO_KHZ800);

const char* test1 = "1 testcase1\n128 0 100 1\nfg:0 FF0000;10 0000FF;90 FF00FF\nsg:10000\n0 FF0000;90 0000FF\n";
int ms, fidx, startms, todelay;

void setup() {

  Serial.begin(115200);
  
  // End of trinket special code
  // strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initialize neo_exec
  neo_exec_init();

  Serial.println(millis());
  neo_exec_load(test1);

  Serial.println(millis());
  neo_exec_draw(TIME_INTV);

  Serial.println(millis());
  show_frame(frame);
  
  Serial.println(millis());
  startms = millis();
  fidx = 0;
}

char buf[256];
void loop() {
  fidx += 1;
  ms = millis();
  todelay = TIME_INTV * fidx + startms - ms;
  sprintf(buf, "f(% 3d): sleep %d ms\n", fidx, todelay);
  Serial.print(buf);
  delay(todelay);
  neo_exec_draw(TIME_INTV);
  show_frame(frame);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void show_frame(struct neo_color *frame) {
  for(uint16_t i=0; i<NEO_N; i++) {
    strip.setPixelColor(i, frame[i].r, frame[i].g, frame[i].b);
  }
  strip.show();
}

