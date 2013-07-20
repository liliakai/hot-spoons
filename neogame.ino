#include <Adafruit_NeoPixel.h>
#include <FastSPI_LED2.h>
#include <SerialCommand.h>
#define SERIALCOMMAND_DEBUG 1
#define NUM_LEDS 240

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

#define FREEPLAY_MODE 0
#define FB_MODE 1
#define TIMEWINDOW 20

uint32_t c1, c2;
int puck;
int b1fired, b2fired = 0;
int mode = FREEPLAY_MODE;

void game_setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  c1 = strip.Color(255, 0, 0);
  c2 = strip.Color(0, 255, 0);
  puck = strip.numPixels()/2;
}

void setup() {

  pinMode(B1, INPUT);
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

int puck_padding = 1; // must be odd
void game_step() {
  for (int i=puck - puck_padding; i < puck + puck_padding+1; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }

  for (int i=puck-puck_padding-1; i > -1; i--) {
    strip.setPixelColor(i, strip.getPixelColor(i-1));
  }
  for (int i=puck+puck_padding+1; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.getPixelColor(i+1));
  }

  if (strip.getPixelColor(puck-puck_padding-1)) {
    strip.setPixelColor(puck-puck_padding-1, 0);
    b1fired--;
    puck++;
  }
  if (strip.getPixelColor(puck+puck_padding+1)) {
    strip.setPixelColor(puck+puck_padding+1, 0);
    b2fired--;
    puck--;
  }

  if (puck == 0) {
    flash(c2, 10, 100);
    setup();
  }
  if (puck == strip.numPixels()-1) {
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


int b1prev, b2prev, b3prev;
boolean debounce(int button, int* prev) {
  int newval = digitalRead(button);
  int oldval = *prev;
  *prev = newval;

  return (newval == 0) && (newval != oldval);
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


int lockout = 0;
int locker = 0;
void handleButtons_timing() {
  boolean b1 = debounce(B1, &b1prev);
  boolean b2 = debounce(B3, &b2prev);

  if (lockout) {
    lockout--;
    if (!lockout)  { // the firster didn't get seconded so they get punished!
      if (locker == B1) puck--; // punish the guilty (they pressed and were not seconded)
      else if (locker == B2) puck++;
      return;
    }
  }

  if (b2fired || b1fired) return;

  if ( (b1 | b2) && (b1 ^ b2) ) {  // if one button is pressed
    if (lockout == 0) {
      locker = b1 ? B1 : B2 ;  // say which button pin number locked (firsted) it
      lockout = TIMEWINDOW;       // 20 is the count value for the time window
    }
    else {  // we're firsted, who hit it 2nd?
      if ((locker == B1) && b2) {  // if B1 firsted it and b2 is pressed
        Serial.println("pew!");
        strip.setPixelColor(0, c1);  // b1 fires a shot from 0!
        strip.setPixelColor(1, c1);  // b1 fires a shot from 0!
        b1fired+=2;  // lock everything out until it's gone
        lockout = 0;
      }
      else if ((locker == B2) && b1) {  // if B2 firsted it and b1 is pressed
        Serial.println("bew!");
        strip.setPixelColor(strip.numPixels()-1, c2);  // b2 fires a shot from n-1!
        strip.setPixelColor(strip.numPixels()-2, c2);  // b2 fires a shot from n-1!
        b2fired+=2;  // lock everything out until it's gone
        lockout = 0;
      }
    }
  }
}  // handleButtons()


void game_loop() {
  game_step();
  if (mode == FREEPLAY_MODE) {
    handleButtons_freeplay();
  } 
  else {
    handleButtons_timing();
  }
  strip.show();
}

void loop() { 
  if (debounce(B2, &b3prev)) {
    mode = (mode + 1) % 3;
    setup();
  } 

  if (mode == FB_MODE) {
    fb_loop();
  }
  else {
    game_loop();
  }
}

/*
### teldredge
 ### www.funkboxing.com
 ### teldredge1979@gmail.com
 
 ### FAST_SPI2 LED FX EXAMPLES
 ### MOSTLY DESIGNED FOR A LOOP/RING OF LEDS (ORIGINALLY FOR A SIGN)
 ### BUT PLENTY USEFUL FOR A LINEAR STRIP TOO
 
 ### NOTES:
 ### - THIS IS REALLY AN EXAMPLE LIBRARY MORE THAN A MODULE OR ANYTHING, MEANING YOU'LL PROBABLY HAVE TO CUT AND PASTE
 ### - SO FAR TESTED WITH WS2801 AND WS2811 USING ARDUINO NANO 328
 ### - USES THE 'SERIALCOMMAND' LIBRARY TO CHANGE 'MODES', BUT THESE FXNS COULD BE LOOPED ANYWAY YOU WANT
 
 ### LICENSE:::USE FOR WHATEVER YOU WANT, WHENEVER YOU WANT, WHYEVER YOU WANT
 ### BUT YOU MUST YODEL ONCE FOR FREEDOM AND MAYBE DONATE TO SOMETHING WORTHWHILE
 --------------------------------------------------------------------------------------------------
 */

/*----------------------------------------------------|
 |                                                     |
 |           FROM THE FAST_SPI2 EXAMPLE FILE           |
 |                                                     |
 |----------------------------------------------------*/
// Uncomment this line if you have any interrupts that are changing pins - this causes the library to be a little bit more cautious
// #define FAST_SPI_INTERRUPTS_WRITE_PINS 1

// Uncomment this line to force always using software, instead of hardware, SPI (why?)
// #define FORCE_SOFTWARE_SPI 1

// Uncomment this line if you want to talk to DMX controllers
// #define FASTSPI_USE_DMX_SIMPLE 1



int BOTTOM_INDEX = 0;
int TOP_INDEX = int(NUM_LEDS/2);
int EVENODD = NUM_LEDS%2;

struct CRGB leds[NUM_LEDS];

int ledsX[NUM_LEDS][3]; //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, ETC)

SerialCommand sCmd;     //-SETUP SerialCommand OBJECT

//int ledMode = 888;      //-START IN DEMO MODE (THIS MODE BLOCKS OUT SERIAL COMMANDS FOR AWHILE)
//int ledMode = 8;      //-START IN POLICE MODE
int ledMode = 5;      //-START IN CYLON MODE
//int ledMode = 16;

//-PERISTENT VARS
int idex = 0;        //-LED INDEX (0 to NUM_LEDS-1
int idx_offset = 0;  //-OFFSET INDEX (BOTTOM LED TO ZERO WHEN LOOP IS TURNED/DOESN'T REALLY WORK)
int ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)
int bouncedirection = 0;  //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;      //-INC VAR FOR SIN LOOPS
int lcount = 0;      //-ANOTHER COUNTING VAR

//------------------------------------- UTILITY FXNS --------------------------------------

//-SET THE COLOR OF A SINGLE RGB LED
void set_color_led(int adex, int cred, int cgrn, int cblu) {  
  int bdex;

  if (idx_offset > 0) {  //-APPLY INDEX OFFSET 
    bdex = (adex + idx_offset) % NUM_LEDS;
  }
  else {
    bdex = adex;
  }

  leds[bdex].r = cred;
  leds[bdex].g = cgrn;
  leds[bdex].b = cblu;  
}


//-FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {
    return BOTTOM_INDEX;
  }
  if (i == TOP_INDEX && EVENODD == 1) {
    return TOP_INDEX + 1;
  }
  if (i == TOP_INDEX && EVENODD == 0) {
    return TOP_INDEX;
  }
  return NUM_LEDS - i;  
}

//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  //int N2 = int(NUM_LEDS/2);
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {
    iN = ( i + TOP_INDEX ) % NUM_LEDS; 
  }
  return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < NUM_LEDS - 1) {
    r = i + 1;
  }
  else {
    r = 0;
  }
  return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = NUM_LEDS - 1;
  }
  return r;
}


//-CONVERT HSV VALUE TO RGB
void HSVtoRGB(int hue, int sat, int val, int colors[3]) {
  // hue: 0-359, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;

  if (sat == 0) { // Achromatic color (gray).
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;
  } 
  else  {
    base = ((255 - sat) * val)>>8;
    switch(hue/60) {
    case 0:
      r = val;
      g = (((val-base)*hue)/60)+base;
      b = base;
      break;
    case 1:
      r = (((val-base)*(60-(hue%60)))/60)+base;
      g = val;
      b = base;
      break;
    case 2:
      r = base;
      g = val;
      b = (((val-base)*(hue%60))/60)+base;
      break;
    case 3:
      r = base;
      g = (((val-base)*(60-(hue%60)))/60)+base;
      b = val;
      break;
    case 4:
      r = (((val-base)*(hue%60))/60)+base;
      g = base;
      b = val;
      break;
    case 5:
      r = val;
      g = base;
      b = (((val-base)*(60-(hue%60)))/60)+base;
      break;
    }
    colors[0]=r;
    colors[1]=g;
    colors[2]=b;
  }
}


void copy_led_array(){
  for(int i = 0; i < NUM_LEDS; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}


void print_led_arrays(int ilen){
  copy_led_array();
  Serial.println("~~~***ARRAYS|idx|led-r-g-b|ledX-0-1-2");
  for(int i = 0; i < ilen; i++ ) {
    Serial.print("~~~");
    Serial.print(i);
    Serial.print("|");      
    Serial.print(leds[i].r);
    Serial.print("-");
    Serial.print(leds[i].g);
    Serial.print("-");
    Serial.print(leds[i].b);
    Serial.print("|");      
    Serial.print(ledsX[i][0]);
    Serial.print("-");
    Serial.print(ledsX[i][1]);
    Serial.print("-");
    Serial.println(ledsX[i][2]);
  }  
}

//------------------------LED EFFECT FUNCTIONS------------------------

void one_color_all(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
  for(int i = 0 ; i < NUM_LEDS; i++ ) {
    set_color_led(i, cred, cgrn, cblu);
    LEDS.show();       
    delay(1);
  }  
}

void one_color_allNOSHOW(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
  for(int i = 0 ; i < NUM_LEDS; i++ ) {
    set_color_led(i, cred, cgrn, cblu);

  }  
}


void rainbow_fade(int idelay) { //-FADE ALL LEDS THROUGH HSV RAINBOW
  ihue++;
  if (ihue >= 359) {
    ihue = 0;
  }
  int thisColor[3];
  HSVtoRGB(ihue, 255, 255, thisColor);
  for(int idex = 0 ; idex < NUM_LEDS; idex++ ) {
    set_color_led(idex,thisColor[0],thisColor[1],thisColor[2]); 
  }
  LEDS.show();    
  delay(idelay);
}


void rainbow_loop(int istep, int idelay) { //-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + istep;
  int icolor[3];  

  if (idex >= NUM_LEDS) {
    idex = 0;
  }
  if (ihue >= 359) {
    ihue = 0;
  }

  HSVtoRGB(ihue, 255, 255, icolor);
  set_color_led(idex, icolor[0], icolor[1], icolor[2]);
  LEDS.show();
  delay(idelay);
}


void random_burst(int idelay) { //-RANDOM INDEX/COLOR
  int icolor[3];  

  idex = random(0,NUM_LEDS);
  ihue = random(0,359);

  HSVtoRGB(ihue, 255, 255, icolor);
  set_color_led(idex, icolor[0], icolor[1], icolor[2]);
  LEDS.show();  
  delay(idelay);
}


void color_bounce(int idelay) { //-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }  
  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {
      set_color_led(i, 255, 0, 0);
    }
    else {
      set_color_led(i, 0, 0, 0);
    }
  }
  LEDS.show();
  delay(idelay);
}


void police_lightsONE(int idelay) { //-POLICE LIGHTS (TWO COLOR SINGLE LED)
  idex++;
  if (idex >= NUM_LEDS) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);  
  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idexR) {
      set_color_led(i, 255, 0, 0);
    }
    else if (i == idexB) {
      set_color_led(i, 0, 0, 255);
    }    
    else {
      set_color_led(i, 0, 0, 0);
    }
  }
  LEDS.show();  
  delay(idelay);
}


void police_lightsALL(int idelay) { //-POLICE LIGHTS (TWO COLOR SOLID)
  idex++;
  if (idex >= NUM_LEDS) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  set_color_led(idexR, 255, 0, 0);
  set_color_led(idexB, 0, 0, 255);
  LEDS.show();  
  delay(idelay);
}


void color_bounceFADE(int idelay) { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);  
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2); 

  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {
      set_color_led(i, 255, 0, 0);
    }
    else if (i == iL1) {
      set_color_led(i, 100, 0, 0);
    }
    else if (i == iL2) {
      set_color_led(i, 50, 0, 0);
    }
    else if (i == iL3) {
      set_color_led(i, 10, 0, 0);
    }        
    else if (i == iR1) {
      set_color_led(i, 100, 0, 0);
    }
    else if (i == iR2) {
      set_color_led(i, 50, 0, 0);
    }
    else if (i == iR3) {
      set_color_led(i, 10, 0, 0);
    }    
    else {
      set_color_led(i, 0, 0, 0);
    }
  }

  LEDS.show();
  delay(idelay);
}


void flicker(int thishue, int thissat) {
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  int thisColor[3];

  if (random_bool < 10) {
    HSVtoRGB(thishue, thissat, random_bright, thisColor);

    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      set_color_led(i, thisColor[0], thisColor[1], thisColor[2]);
    }

    LEDS.show();    
    delay(random_delay);
  }
}


void pulse_one_color_all(int ahue, int idelay) { //-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR 

  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {
      bouncedirection = 0;
    }         
  }  

  int acolor[3];
  HSVtoRGB(ahue, 255, ibright, acolor);

  for(int i = 0 ; i < NUM_LEDS; i++ ) {
    set_color_led(i, acolor[0], acolor[1], acolor[2]);
  }
  LEDS.show();    
  delay(idelay);
}


void pulse_one_color_all_rev(int ahue, int idelay) { //-PULSE SATURATION ON ALL LEDS TO ONE COLOR 

  if (bouncedirection == 0) {
    isat++;
    if (isat >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    isat = isat - 1;
    if (isat <= 1) {
      bouncedirection = 0;
    }         
  }  

  int acolor[3];
  HSVtoRGB(ahue, isat, 255, acolor);

  for(int i = 0 ; i < NUM_LEDS; i++ ) {
    set_color_led(i, acolor[0], acolor[1], acolor[2]);
  }
  LEDS.show();
  delay(idelay);
}


void random_red() { //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED  
  int temprand;
  for(int i = 0; i < NUM_LEDS; i++ ) {
    temprand = random(0,100);
    if (temprand > 50) {
      leds[i].r = 255;
    }
    if (temprand <= 50) {
      leds[i].r = 0;
    }
    leds[i].b = 0; 
    leds[i].g = 0;
  }
  LEDS.show();  
}


void rule30(int idelay) { //1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  copy_led_array();
  int iCW;
  int iCCW;
  int y = 100;  
  for(int i = 0; i < NUM_LEDS; i++ ) {
    iCW = adjacent_cw(i);
    iCCW = adjacent_ccw(i);
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] > y) {
      leds[i].r = 0;
    }
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {
      leds[i].r = 0;
    }
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {
      leds[i].r = 0;
    }
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {
      leds[i].r = 255;
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] > y) {
      leds[i].r = 255;
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {
      leds[i].r = 255;
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {
      leds[i].r = 255;
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {
      leds[i].r = 0;
    }
  }

  LEDS.show();  
  delay(idelay);
}


void random_march(int idelay) { //RANDOM MARCH CCW
  copy_led_array();
  int iCCW;

  int acolor[3];
  HSVtoRGB(random(0,360), 255, 255, acolor);

  leds[0].r = acolor[0];
  leds[0].g = acolor[1];
  leds[0].b = acolor[2];

  for(int i = 1; i < NUM_LEDS ; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }

  LEDS.show();  
  delay(idelay);
}


void rwb_march(int idelay) { //R,W,B MARCH CCW
  copy_led_array();
  int iCCW;

  idex++;
  if (idex > 2) {
    idex = 0;
  }

  switch (idex) {
  case 0:
    leds[0].r = 255;
    leds[0].g = 0;
    leds[0].b = 0;
    break;
  case 1:
    leds[0].r = 255;
    leds[0].g = 255;
    leds[0].b = 255;
    break;
  case 2:
    leds[0].r = 0;
    leds[0].g = 0;
    leds[0].b = 255;
    break;
  }

  for(int i = 1; i < NUM_LEDS; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }

  LEDS.show();  
  delay(idelay);
}


void white_temps() {
  int N9 = int(NUM_LEDS/9);
  for (int i = 0; i < NUM_LEDS; i++ ) {
    if (i >= 0 && i < N9) {
      leds[i].r = 255; 
      leds[i].g = 147; 
      leds[i].b = 41;
    } //-CANDLE - 1900
    if (i >= N9 && i < N9*2) {
      leds[i].r = 255; 
      leds[i].g = 197; 
      leds[i].b = 143;
    } //-40W TUNG - 2600
    if (i >= N9*2 && i < N9*3) {
      leds[i].r = 255; 
      leds[i].g = 214; 
      leds[i].b = 170;
    } //-100W TUNG - 2850
    if (i >= N9*3 && i < N9*4) {
      leds[i].r = 255; 
      leds[i].g = 241; 
      leds[i].b = 224;
    } //-HALOGEN - 3200
    if (i >= N9*4 && i < N9*5) {
      leds[i].r = 255; 
      leds[i].g = 250; 
      leds[i].b = 244;
    } //-CARBON ARC - 5200
    if (i >= N9*5 && i < N9*6) {
      leds[i].r = 255; 
      leds[i].g = 255; 
      leds[i].b = 251;
    } //-HIGH NOON SUN - 5400
    if (i >= N9*6 && i < N9*7) {
      leds[i].r = 255; 
      leds[i].g = 255; 
      leds[i].b = 255;
    } //-DIRECT SUN - 6000
    if (i >= N9*7 && i < N9*8) {
      leds[i].r = 201; 
      leds[i].g = 226; 
      leds[i].b = 255;
    } //-OVERCAST SKY - 7000
    if (i >= N9*8 && i < NUM_LEDS) {
      leds[i].r = 64; 
      leds[i].g = 156; 
      leds[i].b = 255;
    } //-CLEAR BLUE SKY - 20000  
  }
  LEDS.show();  
  delay(100);
}


void color_loop_vardelay() { //-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  idex++;
  if (idex > NUM_LEDS) {
    idex = 0;
  }

  int acolor[3];
  HSVtoRGB(0, 255, 255, acolor);

  int di = abs(TOP_INDEX - idex); //-DISTANCE TO CENTER    
  int t = constrain((10/di)*10, 10, 500); //-DELAY INCREASE AS INDEX APPROACHES CENTER (WITHIN LIMITS)

  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {
      leds[i].r = acolor[0]; 
      leds[i].g = acolor[1]; 
      leds[i].b = acolor[2];
    }
    else {
      leds[i].r = 0; 
      leds[i].g = 0; 
      leds[i].b = 0;
    }
  }

  LEDS.show();  
  delay(t);
}


void strip_march_cw(int idelay) { //-MARCH STRIP C-W
  copy_led_array();
  int iCCW;  
  for(int i = 0; i < NUM_LEDS; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }
  LEDS.show();  
  delay(idelay);
}


void strip_march_ccw(int idelay) { //-MARCH STRIP C-W 
  copy_led_array();
  int iCW;  
  for(int i = 0; i < NUM_LEDS; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCW = adjacent_cw(i);
    leds[i].r = ledsX[iCW][0];
    leds[i].g = ledsX[iCW][1];
    leds[i].b = ledsX[iCW][2];    
  }
  LEDS.show();  
  delay(idelay);
}


void pop_horizontal(int ahue, int idelay) {  //-POP FROM LEFT TO RIGHT UP THE RING
  int acolor[3];
  HSVtoRGB(ahue, 255, 255, acolor);

  int ix;

  if (bouncedirection == 0) {
    bouncedirection = 1;
    ix = idex;  
  }
  else if (bouncedirection == 1) {
    bouncedirection = 0;
    ix = horizontal_index(idex);
    idex++;
    if (idex > TOP_INDEX) {
      idex = 0;
    }      
  }

  for(int i = 0; i < NUM_LEDS; i++ ) {
    if (i == ix) {
      leds[i].r = acolor[0]; 
      leds[i].g = acolor[1]; 
      leds[i].b = acolor[2];
    }
    else {
      leds[i].r = 0; 
      leds[i].g = 0; 
      leds[i].b = 0;
    }
  }

  LEDS.show();  
  delay(idelay);    
}


void quad_bright_curve(int ahue, int idelay) {  //-QUADRATIC BRIGHTNESS CURVER
  int acolor[3];
  int ax;

  for(int x = 0; x < NUM_LEDS; x++ ) {
    if (x <= TOP_INDEX) {
      ax = x;
    }
    else if (x > TOP_INDEX) {
      ax = NUM_LEDS-x;
    }

    int a = 1; 
    int b = 1; 
    int c = 0;

    int iquad = -(ax*ax*a)+(ax*b)+c; //-ax2+bx+c
    int hquad = -(TOP_INDEX*TOP_INDEX*a)+(TOP_INDEX*b)+c; //HIGHEST BRIGHTNESS

    ibright = int((float(iquad)/float(hquad))*255);

    HSVtoRGB(ahue, 255, ibright, acolor);

    leds[x].r = acolor[0]; 
    leds[x].g = acolor[1]; 
    leds[x].b = acolor[2];


  }
  LEDS.show();  
  delay(idelay);
}


void flame() {
  int acolor[3];
  int idelay = random(0,35);

  float hmin = 0.1; 
  float hmax = 45.0;
  float hdif = hmax-hmin;
  int randtemp = random(0,3);
  float hinc = (hdif/float(TOP_INDEX))+randtemp;

  int ahue = hmin;
  for(int i = 0; i < TOP_INDEX; i++ ) {

    ahue = ahue + hinc;

    HSVtoRGB(ahue, 255, 255, acolor);

    leds[i].r = acolor[0]; 
    leds[i].g = acolor[1]; 
    leds[i].b = acolor[2];
    int ih = horizontal_index(i);
    leds[ih].r = acolor[0]; 
    leds[ih].g = acolor[1]; 
    leds[ih].b = acolor[2];

    leds[TOP_INDEX].r = 255; 
    leds[TOP_INDEX].g = 255; 
    leds[TOP_INDEX].b = 255;

    LEDS.show();    
    delay(idelay);
  }
}


void radiation(int ahue, int idelay) { //-SORT OF RADIATION SYMBOLISH- 
  //int N2 = int(NUM_LEDS/2);
  int N3 = int(NUM_LEDS/3);
  int N6 = int(NUM_LEDS/6);  
  int N12 = int(NUM_LEDS/12);  
  int acolor[3];

  for(int i = 0; i < N6; i++ ) { //-HACKY, I KNOW...
    tcount = tcount + .02;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount)*255);

    int j0 = (i + NUM_LEDS - N12) % NUM_LEDS;
    int j1 = (j0+N3) % NUM_LEDS;
    int j2 = (j1+N3) % NUM_LEDS;    
    HSVtoRGB(ahue, 255, ibright, acolor);  
    leds[j0].r = acolor[0]; 
    leds[j0].g = acolor[1]; 
    leds[j0].b = acolor[2];
    leds[j1].r = acolor[0]; 
    leds[j1].g = acolor[1]; 
    leds[j1].b = acolor[2];
    leds[j2].r = acolor[0]; 
    leds[j2].g = acolor[1]; 
    leds[j2].b = acolor[2];    

  }    
  LEDS.show();    
  delay(idelay);    
}


void sin_bright_wave(int ahue, int idelay) {  
  int acolor[3];

  for(int i = 0; i < NUM_LEDS; i++ ) {
    tcount = tcount + .1;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount)*255);

    HSVtoRGB(ahue, 255, ibright, acolor);

    leds[i].r = acolor[0]; 
    leds[i].g = acolor[1]; 
    leds[i].b = acolor[2];

    LEDS.show();    
    delay(idelay);
  }
}


void fade_vertical(int ahue, int idelay) { //-FADE 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {
    idex = 0;
  }  
  int idexA = idex;
  int idexB = horizontal_index(idexA);

  ibright = ibright + 10;
  if (ibright > 255) {
    ibright = 0;
  }
  int acolor[3];
  HSVtoRGB(ahue, 255, ibright, acolor);

  set_color_led(idexA, acolor[0], acolor[1], acolor[2]);  
  set_color_led(idexB, acolor[0], acolor[1], acolor[2]);  

  LEDS.show();  
  delay(idelay);
}


void rainbow_vertical(int istep, int idelay) { //-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {
    idex = 0;
  }  
  ihue = ihue + istep;
  if (ihue > 359) {
    ihue = 0;
  }
  Serial.println(ihue);  
  int idexA = idex;
  int idexB = horizontal_index(idexA);

  int acolor[3];
  HSVtoRGB(ihue, 255, 255, acolor);

  set_color_led(idexA, acolor[0], acolor[1], acolor[2]);  
  set_color_led(idexB, acolor[0], acolor[1], acolor[2]);  

  LEDS.show();  
  delay(idelay);
}


void pacman(int idelay) { //-MARCH STRIP C-W
  int s = int(NUM_LEDS/4);
  lcount++;
  if (lcount > 5) {
    lcount = 0;
  }
  if (lcount == 0) {
    one_color_allNOSHOW(255,255,0);
  }
  if (lcount == 1 || lcount == 5) {
    one_color_allNOSHOW(255,255,0);
    leds[s].r = 0; 
    leds[s].g = 0; 
    leds[s].b = 0;
  }
  if (lcount == 2 || lcount == 4) {
    one_color_allNOSHOW(255,255,0);
    leds[s-1].r = 0; 
    leds[s-1].g = 0; 
    leds[s-1].b = 0;  
    leds[s].r = 0; 
    leds[s].g = 0; 
    leds[s].b = 0;
    leds[s+1].r = 0; 
    leds[s+1].g = 0; 
    leds[s+1].b = 0;
  }        
  if (lcount == 3) {
    one_color_allNOSHOW(255,255,0);
    leds[s-2].r = 0; 
    leds[s-2].g = 0; 
    leds[s-2].b = 0;  
    leds[s-1].r = 0; 
    leds[s-1].g = 0; 
    leds[s-1].b = 0;  
    leds[s].r = 0; 
    leds[s].g = 0; 
    leds[s].b = 0;
    leds[s+1].r = 0; 
    leds[s+1].g = 0; 
    leds[s+1].b = 0;
    leds[s+2].r = 0; 
    leds[s+2].g = 0; 
    leds[s+2].b = 0;
  }

  LEDS.show();  
  delay(idelay);
}


//------------------SETUP------------------
void fb_setup()  
{


  Serial.begin(9600);      // SETUP HARDWARE SERIAL (USB)
  //Serial.begin(115200);      // SETUP HARDWARE SERIAL (USB)

  LEDS.setBrightness(128); // SET BRIGHTNESS TO 1/2 POWER

  //LEDS.addLeds<WS2811, 13>(leds, NUM_LEDS);
  //LEDS.addLeds<TM1809, 13>(leds, NUM_LEDS);
  //LEDS.addLeds<UCS1903, 13>(leds, NUM_LEDS);
  //LEDS.addLeds<TM1803, 13>(leds, NUM_LEDS);
  //LEDS.addLeds<LPD8806>(leds, NUM_LEDS)->clearLeds(300);

  //LEDS.addLeds<WS2801>(leds, NUM_LEDS);
  //---Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
  //LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
  //LEDS.addLeds<WS2801, RBG>(leds, NUM_LEDS);

  //LEDS.addLeds<SM16716>(leds, NUM_LEDS);
  //LEDS.addLeds<WS2811, 11>(leds, NUM_LEDS);
  //LEDS.addLeds<LPD8806, 10, 11>(leds, NUM_LEDS);
  //LEDS.addLeds<WS2811, 13, BRG>(leds, NUM_LEDS);
  LEDS.addLeds<WS2811, 13, GRB>(leds, NUM_LEDS);
  //LEDS.addLeds<LPD8806, BGR>(leds, NUM_LEDS);

  sCmd.addCommand("m",   set_mode_strip);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

  one_color_all(0,0,0); //-BLANK STRIP

  LEDS.show();  
  Serial.println("---SETUP COMPLETE---");
}



void set_mode_strip() {    //-SETS THE MODE (SOME MODES REQUIRE RANDOM STARTS TO WORK RIGHT
  char *arg;
  arg = sCmd.next();

  if (arg != NULL) {
    ledMode = atoi(arg);
  }
  if (ledMode == 13) {  //-FOR CELL AUTO
    random_red();
  }

  Serial.print("~~~***NEW MODE-");
  Serial.println(ledMode);
}

void demo_mode(){
  int r = 10;
  for(int i=0; i<r*3; i++) {
    one_color_all(255,255,255);
  }
  for(int i=0; i<r*25; i++) {
    rainbow_fade(20);
  }
  for(int i=0; i<r*20; i++) {
    rainbow_loop(10, 20);
  }
  for(int i=0; i<r*20; i++) {
    random_burst(20);
  }
  for(int i=0; i<r*12; i++) {
    color_bounce(20);
  }
  for(int i=0; i<r*12; i++) {
    color_bounceFADE(40);
  }
  for(int i=0; i<r*5; i++) {
    police_lightsONE(40);
  }
  for(int i=0; i<r*5; i++) {
    police_lightsALL(40);
  }
  for(int i=0; i<r*35; i++) {
    flicker(200, 255);
  }
  for(int i=0; i<r*50; i++) {
    pulse_one_color_all(0, 10);
  }
  for(int i=0; i<r*35; i++) {
    pulse_one_color_all_rev(0, 10);
  }
  for(int i=0; i<r*5; i++) {
    fade_vertical(240, 60);
  }
  random_red();
  for(int i=0; i<r*5; i++) {
    rule30(100);
  }
  for(int i=0; i<r*8; i++) {
    random_march(30);
  }
  for(int i=0; i<r*5; i++) {
    rwb_march(80);
  }
  one_color_all(0,0,0);
  for(int i=0; i<r*15; i++) {
    radiation(120, 60);
  }
  for(int i=0; i<r*15; i++) {
    color_loop_vardelay();
  }
  for(int i=0; i<r*5; i++) {
    white_temps();
  }
  for(int i=0; i<r; i++) {
    sin_bright_wave(240, 35);
  }
  for(int i=0; i<r*5; i++) {
    pop_horizontal(300, 100);
  }
  for(int i=0; i<r*5; i++) {
    quad_bright_curve(240, 100);
  }
  for(int i=0; i<r*3; i++) {
    flame();
  }
  for(int i=0; i<r*10; i++) {
    pacman(50);
  }
  for(int i=0; i<r*15; i++) {
    rainbow_vertical(15, 50);
  }

  for(int i=0; i<r*3; i++) {
    strip_march_ccw(100);
  }
  for(int i=0; i<r*3; i++) {
    strip_march_cw(100);
  }
  for(int i=0; i<r*2; i++) {
    one_color_all(255,0,0);
  }
  for(int i=0; i<r*2; i++) {
    one_color_all(0,255,0);
  }
  for(int i=0; i<r*2; i++) {
    one_color_all(0,0,255);
  }
  for(int i=0; i<r*2; i++) {
    one_color_all(255,255,0);
  }
  for(int i=0; i<r*2; i++) {
    one_color_all(0,255,255);
  }
  for(int i=0; i<r*2; i++) {
    one_color_all(255,0,255);
  }
}




//------------------MAIN LOOP------------------
void fb_loop() {

  sCmd.readSerial();     //-PROCESS SERIAL COMMANDS

  if (ledMode == 0) { one_color_all(0,0,0); }            //---STRIP OFF - "0"
  if (ledMode == 1) { one_color_all(255,255,255); }      //---STRIP SOLID WHITE
  if (ledMode == 2) { rainbow_fade(0); }                //---STRIP RAINBOW FADE
  if (ledMode == 3) { rainbow_loop(10, 10); }            //---RAINBOW LOOP
  if (ledMode == 4) { random_burst(10); }                //---RANDOM
  if (ledMode == 5) { color_bounce(10); }                //---CYLON v1
  if (ledMode == 6) { color_bounceFADE(10); }            //---CYLON v2
  if (ledMode == 7) { police_lightsONE(20); }            //---POLICE SINGLE
  if (ledMode == 8) { police_lightsALL(10); }            //---POLICE SOLID
  if (ledMode == 9) { flicker(200,255); }                //---STRIP FLICKER 
  if (ledMode == 10) { pulse_one_color_all(0, 5); }     //--- PULSE COLOR BRIGHTNESS
  if (ledMode == 11) { pulse_one_color_all_rev(0, 5); } //--- PULSE COLOR SATURATION   
  if (ledMode == 12) { fade_vertical(240, 20); }         //--- VERTICAL SOMETHING
  if (ledMode == 13) { rule30(100); }                    //--- CELL AUTO - RULE 30 (RED)
  if (ledMode == 14) { random_march(30); }               //--- MARCH RANDOM COLORS
  if (ledMode == 15) { rwb_march(50); }                  //--- MARCH RWB COLORS
  if (ledMode == 16) { radiation(120, 60); }             //--- RADIATION SYMBOL (OR SOME APPROXIMATION)
  if (ledMode == 17) { color_loop_vardelay(); }          //--- VARIABLE DELAY LOOP
  if (ledMode == 18) { white_temps(); }                  //--- WHITE TEMPERATURES
  if (ledMode == 19) { sin_bright_wave(240, 35); }       //--- SIN WAVE BRIGHTNESS
  if (ledMode == 20) { pop_horizontal(300, 20); }       //--- POP LEFT/RIGHT
  if (ledMode == 21) { quad_bright_curve(240, 100); }    //--- QUADRATIC BRIGHTNESS CURVE  
  if (ledMode == 22) { flame(); }                        //--- FLAME-ISH EFFECT
  if (ledMode == 23) { rainbow_vertical(10, 20); }       //--- VERITCAL RAINBOW
  if (ledMode == 24) { pacman(100); }                     //--- PACMAN

  if (ledMode == 98) { strip_march_ccw(100); }           //--- MARCH WHATEVERS ON THE STRIP NOW CC-W
  if (ledMode == 99) { strip_march_cw(100); }            //--- MARCH WHATEVERS ON THE STRIP NOW C-W

  if (ledMode == 101) { one_color_all(255,0,0); }    //---101- STRIP SOLID RED
  if (ledMode == 102) { one_color_all(0,255,0); }    //---102- STRIP SOLID GREEN
  if (ledMode == 103) { one_color_all(0,0,255); }    //---103- STRIP SOLID BLUE
  if (ledMode == 104) { one_color_all(255,255,0); }  //---104- STRIP SOLID YELLOW
  if (ledMode == 105) { one_color_all(0,255,255); }  //---105- STRIP SOLID TEAL?
  if (ledMode == 106) { one_color_all(255,0,255); }  //---106- STRIP SOLID VIOLET?

  if (ledMode == 888) { demo_mode(); }               

}


// GETS CALLED BY SERIALCOMMAND WHEN NO MATCHING COMMAND
void unrecognized(const char *command) {
  Serial.println("nothin fo ya...");
}




