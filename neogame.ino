#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "fastspi_strip.h"
#include "button.h"

#define SERIALCOMMAND_DEBUG 1
#define FREEPLAY_MODE 0
#define SPECTRUM_MODE 1
#define TIMING_MODE 2
#define LIGHTSHOW_MODE 3
#define DEMO_MODE 4
#define MAX_MODE 5
#include "game.h"

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

  sCmd.addCommand("m",   set_lightshow_effect);
  sCmd.setDefaultHandler(unrecognized);

  strip.setup();

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

  if (mode == DEMO_MODE) {
    demo_mode();
  }
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

void demo_mode(){
  int r = 10;
  /*
  for(int i=0; i<r; i++) {
    strip.one_color_all(255,255,255);
  }
  */
  for(int i=0; i<r*25; i++) {
    strip.rainbow_strobe(100);
  }
  for(int i=0; i<r*50; i++) {
    strip.rainbow_loop(10, 20);
  }
  for(int i=0; i<r*15; i++) {
    strip.random_burst(20);
    strip.strip_march_cw(100);
  }
  /*
  for(int i=0; i<r*24; i++) {
    color_bounce(20);
  }
  */
  for(int i=0; i<r*48; i++) {
    strip.color_bounceFADE(20);
  }
  /*
  for(int i=0; i<r*5; i++) {
    police_lightsONE(40);
  }
  for(int i=0; i<r*5; i++) {
    police_lightsALL(40);
  }
  */
  for(int i=0; i<r*35; i++) {
    strip.flicker(240, 255);
  }
  for(int i=0; i<r*50; i++) {
    strip.pulse_one_color_all(0, 10);
  }
  /*
  for(int i=0; i<r*35; i++) {
    strip.pulse_one_color_all_rev(128¨, 10);
  }
  */
  for(int i=0; i<r*15; i++) {
    strip.fade_vertical(240, 60);
  }
  /*
  strip.random_red();
  for(int i=0; i<r*5; i++) {
    strip.rule30(100);
  }

  for(int i=0; i<r; i++) {
    strip.strip_march_ccw(100);
  }
  for(int i=0; i<r; i++) {
    strip.strip_march_cw(100);
  }

  for(int i=0; i<r*25; i++) {
    strip.random_march(30);
  }
  */
  for(int i=0; i<r*24; i++) {
    strip.rwb_march(30);
  }
  for(int i=0; i<r*3; i++) {
    strip.strip_march_ccw(100);
  }
  for(int i=0; i<r*3; i++) {
    strip.strip_march_cw(100);
  }

  strip.one_color_all(0,0,0);
  for(int i=0; i<r*15; i++) {
    strip.radiation(120, 60);
  }
  for(int i=0; i<r*3; i++) {
    strip.strip_march_ccw(100);
  }
  for(int i=0; i<r*3; i++) {
    strip.strip_march_cw(100);
  }

  for(int i=0; i<r*24; i++) {
    strip.color_loop_vardelay(50, 200);
  }
  /*
  for(int i=0; i<r*5; i++) {
    strip.white_temps();
  }
  */
  for(int i=0; i<r*10; i++) {
    strip.sin_bright_wave(240, 35);
  }
    for(int i=0; i<r*3; i++) {
    strip.strip_march_ccw(100);
  }
  for(int i=0; i<r*3; i++) {
    strip.strip_march_cw(100);
  }
  for(int i=0; i<r*12; i++) {
    strip.pop_horizontal(300, 100);
  }
  for(int i=0; i<r*12; i++) {
    strip.pop_random(300, 100);
  }
  /*
  for(int i=0; i<r*5; i++) {
    strip.quad_bright_curve(240, 100);
  }
  */
  for(int i=0; i<r; i++) {
    strip.flame();
  }
  /*
  for(int i=0; i<r*10; i++) {
    strip.pacman(50);
  */
  for(int i=0; i<r*15; i++) {
    strip.rainbow_vertical(15, 50);
  }
  /*
  for(int i=0; i<r*2; i++) {
    strip.one_color_all(255,0,0);
  }
  for(int i=0; i<r*2; i++) {
    strip.one_color_all(0,255,0);
  }
  for(int i=0; i<r*2; i++) {
    strip.one_color_all(0,0,255);
  }
  for(int i=0; i<r*2; i++) {
    strip.one_color_all(255,255,0);
  }
  for(int i=0; i<r*2; i++) {
    strip.one_color_all(0,255,255);
  }
  for(int i=0; i<r*2; i++) {
    strip.one_color_all(255,0,255);
  }
  */
}
