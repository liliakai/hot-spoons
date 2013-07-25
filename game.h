#define FREEPLAY_MODE 0
#define SPECTRUM_MODE 1
#define TIMING_MODE 2
#define TIMEWINDOW 30
#define SHOT 10
#define PENALTY 4
#define PUCK_PADDING 1 // must be odd

CRGB color1 = CRGB(255, 0, 0);
CRGB color2 = CRGB(0, 255, 0);

class game {

public:

  int num_leds;
  int mode;
  int puck;
  int lockout;
  int b1fired;
  int b2fired;
  funkbox& fb;
  Button& button1, button2;

  game(int n, funkbox& fbref, Button& b1, Button& b2)
    : num_leds(n), fb(fbref), button1(b1), button2(b2) {
    setup();
  }

  void setup(int gameMode=-1) {
    if (gameMode >=0) mode = gameMode;
    puck = num_leds/2 + 1;
    lockout = 0;
    b1fired = 0;
    b2fired = 0;
  }

  void step() {
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

    for (int i=puck+PUCK_PADDING+1; i < num_leds-1; i++) {
      fb.leds[i] = fb.leds[i+1];
    }
    fb.set_color_led(num_leds-1, 0, 0, 0);

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
        fb.flash(color2, 10, 100);
      setup();
    }

    if (puck == num_leds-1) {
      if (mode != SPECTRUM_MODE)
        fb.flash(color1, 10, 100);
      setup();
    }

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
      fb.leds[num_leds-1-i] = color2; // b2 fires a shot from n-1!
    }
    b2fired+=SHOT;  // lock everything out until it's gone
  }

};
