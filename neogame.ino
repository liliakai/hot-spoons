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

void setup() {
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  digitalWrite(A6, HIGH);
  digitalWrite(A7, HIGH);
  digitalWrite(A8, HIGH);
  
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  strip.setPixelColor(120, strip.Color(255, 255, 255));

  strip.show();

  c1 = strip.Color(255, 0, 0);
  c2 = strip.Color(0, 255, 0);
}

void game_step() {
  uint32_t c, n;
  c = strip.getPixelColor(0);
  for (int i=0; i < 120; i++) {
    n = strip.getPixelColor(i+1);
    if (n == 0) {
      strip.setPixelColor(i+1, c);
    }
    strip.setPixelColor(i, 0);
    c = n;
  }
  
  c = strip.getPixelColor(strip.numPixels()-1);
  for (int i=strip.numPixels()-1; i > 120; i--) {
    n = strip.getPixelColor(i-1);
    if (n == 0) {
      strip.setPixelColor(i-1, c);
    }
    strip.setPixelColor(i, 0);
    c = n;
  }
}

void readButtons() {
  if (digitalRead(A6) == 0) {  
    Serial.println("pew!");
    strip.setPixelColor(0, c1);
  }

  if (digitalRead(A8) == 0) {
    Serial.println("bew!");
    strip.setPixelColor(strip.numPixels()-1, c2);
  }
}

void loop() {  
  readButtons();
  strip.show();
}










