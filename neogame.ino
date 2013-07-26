#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "fastspi_strip.h"
#include "button.h"
#include "game.h"

#define SERIALCOMMAND_DEBUG 1
#define LIGHTSHOW_MODE 3

#define NUM_LEDS 240
int mode = SPECTRUM_MODE;

Button button1 = Button(A6);
Button button2 = Button(A7);
Button button3 = Button(A8);
fastspi_strip strip = fastspi_strip(NUM_LEDS);
game g = game(NUM_LEDS, strip, button1, button3);
SerialCommand sCmd;

void setup() {
  Serial.begin(115200);

  sCmd.addCommand("m",   set_lightshow_mode);
  sCmd.setDefaultHandler(unrecognized);

  strip.setup();

  if (mode != LIGHTSHOW_MODE) {
    g.setup(mode);
  }

  Serial.println("---SETUP COMPLETE---");
}

void loop() {
  if (button2.pressed()) {
    mode = (mode + 1) % 4;
    setup();
  }

  sCmd.readSerial(); // process serial commands

  if (mode == LIGHTSHOW_MODE) {
    if(button1.pressed()) {
      strip.prev_mode();
    }
    if(button3.pressed()) {
      strip.next_mode();
    }

    strip.loop();
  }
  else {
    g.loop();
  }
}

// Called by SerialCommand to handle light show mode change commands
void set_lightshow_mode() {
  char *arg;
  arg = sCmd.next();

  if (arg != NULL) {
    strip.set_mode(atoi(arg));
  }
}

void unrecognized(const char *command) {
  Serial.println("wat");
}
