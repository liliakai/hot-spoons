#include "spectrum.h"
#include "noise.h"
#define TIMEWINDOW 30
#define SHOT 10
#define PENALTY 4
#define PUCK_PADDING 1 // must be odd

CRGB color1 = CRGB(255, 0, 0);
CRGB color2 = CRGB(0, 255, 0);

class game {

public:
  enum {
    timing_mode,
    freeplay_mode,
    spectrum_mode
  };

  int num_leds;
  int mode;
  int puck;
  int lockout;
  int b1fired;
  int b2fired;
  int since_last_lockout;
  fastspi_strip& strip;
  Button& button1, button2;

  game(int n, fastspi_strip& fbref, Button& b1, Button& b2)
    : num_leds(n), strip(fbref), button1(b1), button2(b2) {
    setup();
  }

  void setup(int gameMode=-1) {
    if (gameMode >=0) mode = gameMode;
    puck = num_leds/2 + 1;
    lockout = 0;
    b1fired = 0;
    b2fired = 0;
  }

  void loop() {
    makeNoise();
    CRGB puck_color = CRGB(0,0,255);
    if (lockout) {
      puck_color = CRGB(128,128,128);
    }

    for (int i=puck - PUCK_PADDING; i < puck + PUCK_PADDING+1; i++) {
      strip.set_color_led(i, puck_color);
    }

    for (int i=puck-PUCK_PADDING-1; i > 0; i--) {
      strip.leds[i] = strip.leds[i-1];
    }
    strip.set_color_led(0, 0, 0, 0);

    for (int i=puck+PUCK_PADDING+1; i < num_leds-1; i++) {
      strip.leds[i] = strip.leds[i+1];
    }
    strip.set_color_led(num_leds-1, 0, 0, 0);

    if (strip.leds[puck-PUCK_PADDING-1] != CRGB(0)) {
      strip.set_color_led(puck-PUCK_PADDING-1, 0, 0, 0);
      if (b1fired) b1fired--;

      puck++;
    }

    if (strip.leds[puck+PUCK_PADDING+1] != CRGB(0)) {
      strip.set_color_led(puck+PUCK_PADDING+1, 0, 0, 0);
      if (b2fired) b2fired--;
      puck--;
    }

    if (mode == freeplay_mode) {
      handleButtons_freeplay();
    }
    else if (mode == timing_mode) {
      if (!b1fired && !b2fired && !lockout && (random(250) == 0 || since_last_lockout > 1000)) {
        lockout = 500;
        since_last_lockout = 0;
      }
      else if (lockout) {
        lockout--;
      }
      since_last_lockout++;
      handleButtons_timing();
    }
    else if (mode == spectrum_mode) {
      handleSpectrum();
    }
    LEDS.show();

    if (puck == 0) {
      if (mode != spectrum_mode)
        strip.flash(color2, 10, 100);
      setup();
    }

    if (puck == num_leds-1) {
      if (mode != spectrum_mode)
        strip.flash(color1, 10, 100);
      setup();
    }
  }

  void handleSpectrum () {
    boolean b1 = button1.pressed() || spectrum(LOW_SPECTRUM);
    boolean b2 = button2.pressed() || spectrum(HIGH_SPECTRUM);

    if (b1) {
      b1fire();
    }
    if (b2) {
      b2fire();
    }
  }

  void handleButtons_freeplay() {
    boolean b1 = button1.read();
    boolean b2 = button2.read();

    if (b1 == 0) {
      b1fire();
    }

    if (b2 == 0) {
      b2fire();
    }

  }

  void handleButtons_timing() {
    boolean b1 = button1.pressed();
    boolean b2 = button2.pressed();
    if (!lockout) {
        if (b1) {
          puck -= PENALTY;
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
      strip.set_color_led(i, color1);
    }
    b1fired+=SHOT;  // lock everything out until it's gone
  }

  void b2fire() {
    Serial.println("bew!");
    whichTune = BEW;  // make the BEW noise!
    for (int i=0; i < SHOT; i++) {
      strip.set_color_led(num_leds-1-i, color2);
    }
    b2fired+=SHOT;  // lock everything out until it's gone
  }
};
