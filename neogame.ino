#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "fastspi_strip.h"
#include "button.h"
#include "game.h"

#define NUM_LEDS 240

Button button4 = Button(11);
Button button5 = Button(12);

fastspi_strip strip = fastspi_strip(NUM_LEDS);
game g = game(NUM_LEDS, strip, button4, button5);

void setup() {
  Serial.begin(115200);

  strip.clear();
  g.setup();

  Serial.println("---SETUP COMPLETE---");
}

void loop() {
  g.loop();
}
