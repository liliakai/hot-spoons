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
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  digitalWrite(B1, HIGH); // turn on pull-up resistors for buttons
  digitalWrite(B2, HIGH);
  digitalWrite(B3, HIGH);

  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  c1 = strip.Color(255, 0, 0);  // c1 = red?
  c2 = strip.Color(0, 255, 0);  // c2 = green?
}

void game_step() {
  strip.setPixelColor(puck, strip.Color(255, 255, 255));  // puck, white, where it goes

  for (int i=puck-1; i > -1; i--) {
    strip.setPixelColor(i, strip.getPixelColor(i-1));
  }
  for (int i=puck+1; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.getPixelColor(i+1));
  }

  if (strip.getPixelColor(puck-1)) {
    strip.setPixelColor(puck-1, 0);
    puck++;
  }
  if (strip.getPixelColor(puck+1)) {
    strip.setPixelColor(puck+1, 0);
    puck--;
  }
} // game_step()

int b1prev, b2prev;
boolean debounce(int button, int* prev) {
  int newval = digitalRead(button);
  int oldval = *prev;
  *prev = newval;

  return (newval == 0) && (newval != oldval);
}

int lockout = 0;
int locker = 0;
void handleButtons() {
  boolean b1 = debounce(B1, &b1prev);
  boolean b2 = debounce(B2, &b2prev);

  if (lockout) { lockout--; }

  if ( (b1 | b2) && (b1 ^ b2) ) {
    if (lockout == 0) {
      locker = b1 ? B1 : B2 ;
      lockout = TIMEWINDOW;       // 20 is the count value for the time window
    } 
    else {
      if (locker == B1 && b2) {  
        Serial.println("pew!");
        strip.setPixelColor(0, c1);
        lockout = 0;
      }  
      else if (locker == B2 && b1) {
        Serial.println("bew!");
        strip.setPixelColor(strip.numPixels()-1, c2);
        lockout = 0;
      }
    }
  }
}

void loop() {  
  game_step();
  handleButtons();
  strip.show();
}













