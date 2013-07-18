#include <Adafruit_NeoPixel.h>

#define STRIPPIN 13

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(240, STRIPPIN, NEO_GRB + NEO_KHZ800);

#define B1 A6
#define B2 A8
#define B3 A7

#define TIMEWINDOW 20  // HOW MANY cycles is the time window

uint32_t c1, c2;
int puck = strip.numPixels()/2;

void setup() {
  pinMode(B1, INPUT); // pins default to inputs anyway
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  digitalWrite(B1, HIGH); // turn on pull-up resistors for buttons
  digitalWrite(B2, HIGH);
  digitalWrite(B3, HIGH);

  Serial.begin(57600);
  strip.begin();  // parameters in variable "strip"
  strip.show(); // Initialize all pixels to 'off'

  c1 = strip.Color(255, 0, 0);  // c1 = red
  c2 = strip.Color(0, 255, 0);  // c2 = green
}

void game_step() {
  strip.setPixelColor(puck, strip.Color(255, 255, 255));  // puck, white, where it goes

  for (int i=puck-1; i > -1; i--) {
    strip.setPixelColor(i, strip.getPixelColor(i-1));  // move pixels up toward puck
  }
  for (int i=puck+1; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.getPixelColor(i+1));  // move pixels down toward puck
  }

  if (strip.getPixelColor(puck-1)) {  // if red is just below puck, move puck up toward green
    strip.setPixelColor(puck-1, 0);
    puck++;
  }
  if (strip.getPixelColor(puck+1)) { // if green is just above puck, move puck down toward red
    strip.setPixelColor(puck+1, 0);
    puck--;
  }
} // game_step()

int b1prev, b2prev;
boolean debounce(int button, int* prev) {  // doesn't actually debounce
  int newval = digitalRead(button);
  int oldval = *prev;
  *prev = newval;

  return (newval == 0) && (newval != oldval);
} // debounce(

int lockout = 0;
int locker = 0;
void handleButtons() {
  boolean b1 = debounce(B1, &b1prev);
  boolean b2 = debounce(B2, &b2prev);

  if (lockout) { lockout--; }

  if ( (b1 | b2) && (b1 ^ b2) ) {  // if one button is pressed
    if (lockout == 0) {
      locker = b1 ? B1 : B2 ;  // say which button pin number locked (firsted) it
      lockout = TIMEWINDOW;       // 20 is the count value for the time window
    }
    else {  // we're firsted, who hit it 2nd?
      if ((locker == B1) && b2) {  // if B1 firsted it and b2 is pressed
        Serial.println("pew!");
        strip.setPixelColor(0, c1);  // b1 fires a shot from 0!
        lockout = 0;
      }  
      else if ((locker == B2) && b1) {  // if B2 firsted it and b1 is pressed
        Serial.println("bew!");
        strip.setPixelColor(strip.numPixels()-1, c2);  // b2 fires a shot from n-1!
        lockout = 0;
      }
    }
  }
}  // handleButtons()

void loop() {  
  game_step();
  handleButtons();
  strip.show();
}














