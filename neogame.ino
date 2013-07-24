#include <Adafruit_NeoPixel.h>
#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#include "funkboxing.h"
#define SERIALCOMMAND_DEBUG 1
#define NUM_LEDS 240

#define STRIPPIN 13
#define TONEPIN 53  // which pin sound comes out of

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(240, STRIPPIN, NEO_GRB + NEO_KHZ800);
funkbox fb = funkbox(240);

SerialCommand sCmd;     //-SETUP SerialCommand OBJECT
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

int mode = FB_MODE;

#define TUNESIZE 7 // how many notes per tune
  // play notes from F#-0 (0x1E) to F#-5 (0x5A):
int tunes[][TUNESIZE] = {
  { 0x4A, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44 }, // PEW
  { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 }  // BEW
};

#define TUNERATE 5 // how many LED frames per tunes advancement 
#define NONE -1 // no tune playing
#define PEW 0 // this is one tune
#define BEW 1 // this is another
int whichTune = -1; // which tune we are playing right now
int tunePosition = 0; // this increments every LED animation frame

void makeNoise() {
  if (whichTune > -1) { // if we are playing a tune
    tone(TONEPIN, tunes[whichTune][tunePosition++ / TUNERATE]); // play at the appropriate rate
    if (tunePosition / TUNERATE >= TUNESIZE) {
      tunePosition = 0; // the song is over (can repeat or stop now)
      // whichTune = NONE;  // the tune must stop (comment this out to repeat tune until...)
    }
  }
    else noTone(TONEPIN); // there is no tune playing
} // makeNoise()

uint32_t c1, c2;
int puck;
int lockout;
int locker;
int b1fired, b2fired;
int b1prev, b2prev, b3prev;


void game_setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  c1 = strip.Color(255, 0, 0);
  c2 = strip.Color(0, 255, 0);
  puck = strip.numPixels()/2;
  lockout = 0;
  locker = 0;
  b1fired = 0;
  b2fired = 0;
}
#define SPECTS 3  // HOW MANY SPECTRUMS
#define HIGH_SPECTRUM 0 // these signals are analog and are highest at the peak of the sound
#define MID_SPECTRUM 1
#define LOW_SPECTRUM 2

int spectrumPin[SPECTS] = { A13, A14, A15 }; // what pin this spectrum is on
int lastSpectrumRead[SPECTS] = { 0 }; // last time we read that value
int spectrumRead[SPECTS]; // this time
int spectrumThreshold[SPECTS] = { 200, 200, 200 }; // above this value is a beat
int spectrumHysteresis[SPECTS] = { 20, 20, 20 }; // must be this much lower to end beat

boolean spectrum(int spect) {
  spectrumRead[spect] = analogRead(spectrumPin[spect]);  // read the damn signal value
  if (!(lastSpectrumRead[spect]) && (spectrumRead[spect] > spectrumThreshold[spect])) {
    lastSpectrumRead[spect] = spectrumRead[spect];
    return(true); // first time it went over threshold
  }
  else {
    if (lastSpectrumRead[spect] - spectrumRead[spect] > spectrumHysteresis[spect]) {
      lastSpectrumRead[spect] = 0;
    }
  }
  return(false);
}

void setup() {
  pinMode(B1, INPUT); // pins default to inputs anyway
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  digitalWrite(B1, HIGH);
  digitalWrite(B2, HIGH);
  digitalWrite(B3, HIGH);

  if (mode == FB_MODE) {
    fb_setup();
  } 
  else {
    game_setup();
  }
}

void game_step() {
  uint32_t puck_color = strip.Color(0,0,255);
  if (lockout) {
    puck_color = strip.Color(128,128,128);
  }
  for (int i=puck - PUCK_PADDING; i < puck + PUCK_PADDING+1; i++) {
    strip.setPixelColor(i, puck_color);
  }

  for (int i=puck-PUCK_PADDING-1; i > -1; i--) {
    strip.setPixelColor(i, strip.getPixelColor(i-1));
  }
  for (int i=puck+PUCK_PADDING+1; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.getPixelColor(i+1));
  }

  if (strip.getPixelColor(puck-PUCK_PADDING-1)) {
    strip.setPixelColor(puck-PUCK_PADDING-1, 0);
    if (b1fired) b1fired--; 
    
    puck++;
  }
  if (strip.getPixelColor(puck+PUCK_PADDING+1)) {
    strip.setPixelColor(puck+PUCK_PADDING+1, 0);
    if (b2fired) b2fired--;
    puck--;
  }

  if (puck == 0) {
    if (mode != SPECTRUM_MODE)
      flash(c2, 10, 100);
    setup();
  }
  if (puck == strip.numPixels()-1) {
    if (mode != SPECTRUM_MODE)
      flash(c1, 10, 100);
    setup();
  }
}

void flash(uint32_t c, int times, int d) {
  for (int j=0; j < times; j++) {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
    }
    strip.show();
    delay(d);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
    strip.show();
    delay(d);

  }
}

boolean debounce(int button, int* prev) {
  int newval = digitalRead(button);
  int oldval = *prev;
  *prev = newval;

  return (newval == 0) && (newval != oldval);
} // debounce(


void handleSpectrum () {
  boolean b1 = debounce(B1, &b1prev) || spectrum(LOW_SPECTRUM);
  boolean b2 = debounce(B3, &b3prev) || spectrum(HIGH_SPECTRUM);

  if (b1) {
   b1fire();
  }
  if (b2) {
    b2fire();
  }
}

void handleButtons_freeplay() {
  boolean b1 = digitalRead(B1);
  boolean b2 = digitalRead(B3);
  if (b1 == 0) {  
    Serial.println("pew!");
    strip.setPixelColor(0, c1);
  }  

  if (b2 == 0) {
    Serial.println("bew!");
    strip.setPixelColor(strip.numPixels()-1, c2);
  }

} 


void handleButtons_timing() {
  boolean b1 = debounce(B1, &b1prev);
  boolean b2 = debounce(B3, &b3prev);
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
          strip.setPixelColor(i, c1);  // b1 fires a shot from 0!
        }
        b1fired+=SHOT;  // lock everything out until it's gone
}

void b2fire() {
    Serial.println("bew!");
	whichTune = BEW;  // make the BEW noise!
        for (int i=0; i < SHOT; i++) {
          strip.setPixelColor(strip.numPixels()-1-i, c2);  // b2 fires a shot from n-1!
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
    if (!b1fired && !b2fired && !lockout && random(150) == 0) {
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
  strip.show();
}

void loop() { 
  if (debounce(B2, &b2prev)) {
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

//------------------SETUP------------------
void fb_setup()  
{
  Serial.begin(115200);      // SETUP HARDWARE SERIAL (USB)

  sCmd.addCommand("m",   set_mode_strip);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

  fb.one_color_all(0,0,0); //-BLANK STRIP

  LEDS.show();  
  Serial.println("---SETUP COMPLETE---");
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
  if(debounce(B1, &b1prev)) {
    fb.ledMode--;
    fb.ledMode %= 28;
  }
  if(debounce(B3, &b3prev)) {
    fb.ledMode++;
    fb.ledMode %= 28;
  }

  sCmd.readSerial();     //-PROCESS SERIAL COMMANDS

  fb.loop();
}


// GETS CALLED BY SERIALCOMMAND WHEN NO MATCHING COMMAND
void unrecognized(const char *command) {
  Serial.println("nothin fo ya...");
}
