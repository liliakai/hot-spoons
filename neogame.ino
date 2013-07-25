#include <Adafruit_NeoPixel.h>
#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "funkboxing.h"
#include "button.h"
#include "noise.h"
#include "spectrum.h"
#include "game.h"

#define SERIALCOMMAND_DEBUG 1
#define FB_MODE 3

#define NUM_LEDS 240
int mode = SPECTRUM_MODE;

Button button1 = Button(A6);
Button button2 = Button(A7);
Button button3 = Button(A8);
funkbox fb = funkbox(NUM_LEDS);
game g = game(NUM_LEDS, fb, button1, button3);
SerialCommand sCmd;

void setup() {
  Serial.begin(115200);

  sCmd.addCommand("m",   set_mode_strip);
  sCmd.setDefaultHandler(unrecognized);

  fb.setup();

  if (mode != FB_MODE) {
    g.setup(mode);
  }

  Serial.println("---SETUP COMPLETE---");
}

void game_loop() {
  makeNoise();
  g.step();
}

void loop() {
  if (button2.pressed()) {
    mode = (mode + 1) % 4;
    setup();
  }

  if (mode == FB_MODE) {
    fb_loop();
  }
  else {
    game_loop();
  }
}

void set_mode_strip() {    //-SETS THE MODE (SOME MODES REQUIRE RANDOM STARTS TO WORK RIGHT
  char *arg;
  arg = sCmd.next();

  if (arg != NULL) {
	  fb.set_mode(atoi(arg));
  }
  Serial.print("~~~***NEW MODE-");
  Serial.println(fb.ledMode);
}

//------------------MAIN LOOP------------------
void fb_loop() {
  if(button1.pressed()) {
    set_fb_mode((fb.ledMode-1) % 28);
  }
  if(button3.pressed()) {
    set_fb_mode((fb.ledMode+1) % 28);
  }

  sCmd.readSerial();     //-PROCESS SERIAL COMMANDS

  fb.loop();
}

void set_fb_mode(int mode) {
  fb.set_mode(mode);
  Serial.print("~~~***NEW MODE-");
  Serial.println(fb.ledMode);
}


// GETS CALLED BY SERIALCOMMAND WHEN NO MATCHING COMMAND
void unrecognized(const char *command) {
  Serial.println("nothin fo ya...");
}
