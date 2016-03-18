#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "fastspi_strip.h"
#include "button.h"
#include "game.h"

#define SERIALCOMMAND_DEBUG 1

enum {
  TIMING_MODE = game::timing_mode,
  FREEPLAY_MODE = game::freeplay_mode,
  SPECTRUM_MODE = game::spectrum_mode,
  LIGHTSHOW_MODE,
  DEMO_MODE,
  MAX_MODE
};

#define NUM_LEDS 788
//int mode = TIMING_MODE;
int mode=LIGHTSHOW_MODE;

Button button1 = Button(A6);
Button button2 = Button(A7);
Button button3 = Button(A8);
Button button4 = Button(11);
Button button5 = Button(12);

fastspi_strip strip = fastspi_strip(NUM_LEDS);
game g = game(NUM_LEDS, strip, button4, button5);
SerialCommand sCmd;

//void demo_mode();
void set_lightshow_effect();
void unrecognized(const char *command);

void setup() {
  Serial.begin(115200);
  Serial.print("Mode ");
  Serial.print(mode);

  sCmd.addCommand("m",   set_lightshow_effect);
  sCmd.setDefaultHandler(unrecognized);

  strip.clear();

  if (mode < LIGHTSHOW_MODE) {
    g.setup(mode);
  }

  Serial.println("---SETUP COMPLETE---");
}

void loop() {
  if (button2.pressed()) {
    mode = (mode + 1) % 5;
    setup();
  }

  sCmd.readSerial(); // process serial commands

/*
  if (mode == DEMO_MODE) {
    demo_mode();
  }
  */
  if (mode == LIGHTSHOW_MODE) {
    if(button1.pressed()) {
      strip.prev();
    }
    if(button3.pressed()) {
      strip.next();
    }

    strip.loop();
  }
  else {
    g.loop();
  }
}

// Called by SerialCommand to handle light show mode change commands
void set_lightshow_effect() {
  char *arg;
  arg = sCmd.next();

  if (arg != NULL) {
    strip.set_effect(atoi(arg));
  }
}

void unrecognized(const char *command) {
  Serial.println("wat");
}
