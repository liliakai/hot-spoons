#include <Adafruit_NeoPixel.h>
#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "funkboxing.h"
#include "button.h"
#include "noise.h"
#include "spectrum.h"

#define SERIALCOMMAND_DEBUG 1
#define DATA_PIN 13
#define B1 A6
#define B2 A7
#define B3 A8
#define FREEPLAY_MODE 0
#define FB_MODE 1
#define TIMING_MODE 2
#define SPECTRUM_MODE 3
#define TIMEWINDOW 30
#define SHOT 10
#define PENALTY 4
#define PUCK_PADDING 1 // must be odd

#define NUM_LEDS 240
int mode = SPECTRUM_MODE;

CRGB color1 = CRGB(255, 0, 0);
CRGB color2 = CRGB(0, 255, 0);
funkbox fb = funkbox(NUM_LEDS);
Button button1 = Button(B1);
Button button2 = Button(B2);
Button button3 = Button(B3);
int puck;
int lockout;
int b1fired, b2fired;
SerialCommand sCmd;

void game_setup() {
  puck = NUM_LEDS/2 + 1;
  lockout = 0;
  b1fired = 0;
  b2fired = 0;
}

void setup() {
  Serial.begin(115200);      // SETUP HARDWARE SERIAL (USB)

  sCmd.addCommand("m",   set_mode_strip);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

  fb.setup();

  if (mode != FB_MODE) {
    game_setup();
  }

  Serial.println("---SETUP COMPLETE---");
}

void game_step() {
  CRGB puck_color = CRGB(0,0,255);
  if (lockout) {
    puck_color = CRGB(128,128,128);
  }

  for (int i=puck - PUCK_PADDING; i < puck + PUCK_PADDING+1; i++) {
    fb.leds[i] = puck_color;
  }

  for (int i=puck-PUCK_PADDING-1; i > 0; i--) {
    fb.leds[i] = fb.leds[i-1];
  }
  fb.set_color_led(0, 0, 0, 0);
  for (int i=puck+PUCK_PADDING+1; i < NUM_LEDS-1; i++) {
    fb.leds[i] = fb.leds[i+1];
  }
  fb.set_color_led(NUM_LEDS-1, 0, 0, 0);

  if (fb.leds[puck-PUCK_PADDING-1] != CRGB(0)) {
    fb.set_color_led(puck-PUCK_PADDING-1, 0, 0, 0);
    if (b1fired) b1fired--;

    puck++;
  }
  if (fb.leds[puck+PUCK_PADDING+1] != CRGB(0)) {
    fb.set_color_led(puck+PUCK_PADDING+1, 0, 0, 0);
    if (b2fired) b2fired--;
    puck--;
  }

  if (puck == 0) {
    if (mode != SPECTRUM_MODE)
      flash(color2, 10, 100);
    setup();
  }
  if (puck == NUM_LEDS-1) {
    if (mode != SPECTRUM_MODE)
      flash(color1, 10, 100);
    setup();
  }
}

void flash(CRGB color, int times, int d) {
  for (int j=0; j < times; j++) {
    fb.one_color_allNOSHOW(color.r, color.g, color.b);
    LEDS.show();
    delay(d);

    fb.one_color_allNOSHOW(0,0,0);
    LEDS.show();
    delay(d);
  }
}

void handleSpectrum () {
  boolean b1 = button1.pressed() || spectrum(LOW_SPECTRUM);
  boolean b2 = button3.pressed() || spectrum(HIGH_SPECTRUM);

  if (b1) {
    b1fire();
  }
  if (b2) {
    b2fire();
  }
}

void handleButtons_freeplay() {
  boolean b1 = button1.read();
  boolean b2 = button3.read();

  if (b1 == 0) {  
    b1fire();
  }  

  if (b2 == 0) {
    b2fire();
  }

} 


void handleButtons_timing() {
  boolean b1 = button1.pressed();
  boolean b2 = button3.pressed();
  if (!lockout) {
      if (b1) {
        puck -= PENALTY; // punish the guilty (they pressed and were not seconded)
      }
      if (b2) {
        puck += PENALTY;
      }
      return;
  }

  if (b2fired || b1fired) return;

  if (lockout && (b1 || b2)) {
      if (b1) {
       b1fire();
      }
      if (b2) {
        b2fire();
      }
      lockout = 0;
  }
}  // handleButtons()

void b1fire() {
  Serial.println("pew!");
  whichTune = PEW;  // make the PEW noise!
  for (int i=0; i < SHOT; i++) {
    fb.leds[i] = color1; // b1 fires a shot from 0!
  }
  b1fired+=SHOT;  // lock everything out until it's gone
}

void b2fire() {
  Serial.println("bew!");
  whichTune = BEW;  // make the BEW noise!
  for (int i=0; i < SHOT; i++) {
    fb.leds[NUM_LEDS-1-i] = color2; // b2 fires a shot from n-1!
  }
  b2fired+=SHOT;  // lock everything out until it's gone
}

void game_loop() {
  makeNoise();
  game_step();
  if (mode == FREEPLAY_MODE) {
    handleButtons_freeplay();
  }
  else if (mode == TIMING_MODE) {
    if (!b1fired && !b2fired && !lockout && random(250) == 0) {
      lockout = 500;
    }
    else if (lockout) {
      lockout--;
    }
    handleButtons_timing();
  }
  else if (mode == SPECTRUM_MODE) {
    handleSpectrum();
  }
  LEDS.show();
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
