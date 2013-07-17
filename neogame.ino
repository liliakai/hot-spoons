#include <Adafruit_NeoPixel.h>

#define PIN 13

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(240, PIN, NEO_GRB + NEO_KHZ800);

#define B1 A6
#define B2 A7
#define B3 A8

uint32_t c1, c2;
int center = strip.numPixels()/2;

void setup() {
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  digitalWrite(B1, HIGH);
  digitalWrite(B2, HIGH);
  digitalWrite(B3, HIGH);

  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  c1 = strip.Color(255, 0, 0);
  c2 = strip.Color(0, 255, 0);
}

void game_step() {
  strip.setPixelColor(center, strip.Color(255, 255, 255));

  for (int i=center-1; i > -1; i--) {
    strip.setPixelColor(i, strip.getPixelColor(i-1));
  }
  for (int i=center+1; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.getPixelColor(i+1));
  }

  if (strip.getPixelColor(center-1)) {
    strip.setPixelColor(center-1, 0);
    center++;
  }
  if (strip.getPixelColor(center+1)) {
    strip.setPixelColor(center+1, 0);
    center--;
  }
}

int b1prev, b2prev;
boolean debounce(int button, int* prev) {
  int newval = digitalRead(button);
  int oldval = *prev;
  *prev = newval;

  return (newval == 0) && (newval != oldval);
}

int lockout = 0;
void handleButtons() {
  boolean b1 = debounce(B1, &b1prev);
  boolean b2 = debounce(B3, &b2prev);
  if (!lockout) {
    if (b1 || b2) lockout = 5;

    if (b1) {  
      Serial.println("pew!");
      strip.setPixelColor(0, c1);
    }  

    if (b2) {
      Serial.println("bew!");
      strip.setPixelColor(strip.numPixels()-1, c2);
    }
    
  } else {
    lockout--;
  }  
}

void loop() {  
  game_step();
  handleButtons();
  strip.show();
}











