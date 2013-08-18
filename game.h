#include "noise.h"
#define TIMEWINDOW 30
#define SHOT 10
#define PENALTY 4
#define PUCK_PADDING 1 // must be odd

CRGB color1 = CRGB(0, 255, 0);
CRGB color2 = CRGB(128, 0, 128);
CRGB off = CRGB(0);
CRGB red = CRGB(255,0,0);
CRGB green = CRGB(0,255,0);
CRGB blue = CRGB(0,0,255);

class game {
public:
  int num_leds;
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

  void setup() {
    puck = num_leds/2 + 1;
    lockout = 0;
    b1fired = 0;
    b2fired = 0;
  }

  void loop() {
    makeNoise();
    CRGB puck_color = red;
    if (lockout) {
      puck_color = blue;
    }

    for (int i=puck - PUCK_PADDING; i < puck + PUCK_PADDING+1; i++) {
      strip.set_color_led(i, puck_color);
    }

    b1fired = 0;
    for (int i=puck-PUCK_PADDING-1; i > 0; i--) {
      b1fired |= strip.leds[i].r | strip.leds[i].g | strip.leds[i].b;
      strip.leds[i] = strip.leds[i-1];
    }
    strip.set_color_led(0, 0, 0, 0);

    b2fired = 0;
    for (int i=puck+PUCK_PADDING+1; i < num_leds-1; i++) {
      b2fired |= strip.leds[i].r | strip.leds[i].g | strip.leds[i].b;
      strip.leds[i] = strip.leds[i+1];
    }
    strip.set_color_led(num_leds-1, 0, 0, 0);

    if (!b1fired && !b2fired) {
      whichTune = -1;
    }

    if (strip.leds[puck-PUCK_PADDING-1] != off) {
      strip.set_color_led(puck-PUCK_PADDING-1, 0, 0, 0);

      puck++;
    }

    if (strip.leds[puck+PUCK_PADDING+1] != off) {
      strip.set_color_led(puck+PUCK_PADDING+1, 0, 0, 0);
      puck--;
    }

    if (!b1fired && !b2fired && !lockout && (random(250) == 0 || since_last_lockout > 1000)) {
      lockout = 500;
      since_last_lockout = 0;
    }
    else if (lockout) {
      lockout--;
    }
    since_last_lockout++;
    handleButtons_timing();

    LEDS.show();

    if (puck == 0) {
      winner(color2);
    }

    if (puck == num_leds-1) {
      winner(color1);
    }
  }

  void winner(CRGB color) {
    strip.flash(color, 10, 100);
    setup();
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
