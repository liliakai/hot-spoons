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

#include "fastspi_strip.h"

#define DEFAULT_EFFECT 39
#define DATA_PIN 19
#define MAX_EFFECT_NUMBER 36 // last effect to hit when cycling through next/prev

fastspi_strip::fastspi_strip(int n) : num_leds(n), BOTTOM_INDEX(0), TOP_INDEX(n/2), EVENODD(n%2),
  effectNumber(DEFAULT_EFFECT)
{
  leds = new CRGB[num_leds];

  LEDS.addLeds<WS2811, DATA_PIN, GRB>(leds, num_leds);
  LEDS.setBrightness(128); // SET BRIGHTNESS TO 1/2 POWER
  ihue = 120;
}

void fastspi_strip::clear()
{
  LEDS.clear();
}

//------------------MAIN LOOP------------------
void fastspi_strip::loop() {

  switch(effectNumber) {
    case 0:
      one_color_all(0,0,0);             //---STRIP OFF - "0"
      break;
    case 1:
      color_bounce(10);                 //---CYLON v1
      break;
    case 2:
      color_bounceFADE(10);             //---CYLON v2
      break;
    case 3:
      police_lightsONE(20);             //---POLICE SINGLE
      break;
    case 4:
      one_color_all(255,255,255);       //---STRIP SOLID WHITE
      break;
    case 5:
      rainbow_fade(0);                 //---STRIP RAINBOW FADE
      break;
    case 6:
      rainbow_loop(10, 10);             //---RAINBOW LOOP
      break;
    case 7:
      random_burst(10);                 //---RANDOM
      break;
    case 8:
      police_lightsALL(10);             //---POLICE SOLID
      break;
    case 9:
      flicker(200,255);                 //---STRIP FLICKER
      break;
    case 10:
      pulse_one_color_all(0, 5);      //--- PULSE COLOR BRIGHTNESS
      break;
    case 11:
      pulse_one_color_all_rev(0, 5);  //--- PULSE COLOR SATURATION
      break;
    case 12:
      fade_vertical(240, 20);          //--- VERTICAL SOMETHING
      break;
    case 13:
      rule30(100);                     //--- CELL AUTO - RULE 30 (RED)
      break;
    case 14:
      random_march(30);                //--- MARCH RANDOM COLORS
      break;
    case 15:
      rwb_march(50);                   //--- MARCH RWB COLORS
      break;
    case 16:
      radiation(120, 60);              //--- RADIATION SYMBOL (OR SOME APPROXIMATION)
      break;
    case 17:
      color_loop_vardelay(50, 200);           //--- VARIABLE DELAY LOOP
      break;
    case 18:
      white_temps();                   //--- WHITE TEMPERATURES
      break;
    case 19:
      sin_bright_wave(240, 35);        //--- SIN WAVE BRIGHTNESS
      break;
    case 20:
      pop_horizontal(300, 20);        //--- POP LEFT/RIGHT
      break;
    case 21:
      quad_bright_curve(240, 100);     //--- QUADRATIC BRIGHTNESS CURVE
      break;
    case 22:
      flame();                         //--- FLAME-ISH EFFECT
      break;
    case 23:
      rainbow_vertical(10, 20);        //--- VERITCAL RAINBOW
      break;
    case 24:
      pacman(100);                      //--- PACMAN
      break;
    case 25:
      rainbow_strobe(100);                 //---STRIP RAINBOW FADE
      break;
    case 26:
      pop_random(300, 20);        //--- POP randomly
      break;
    case 27:
      random_red();        //randomize
      break;
    case 28:
      beat_march(10);
      break;
    case 29:
      strip_march_ccw(100);            //--- MARCH WHATEVERS ON THE STRIP NOW CC-W
      break;
    case 30:
      strip_march_cw(100);             //--- MARCH WHATEVERS ON THE STRIP NOW C-W
      break;
    case 31:
      one_color_all(255,0,0);     //---101- STRIP SOLID RED
      break;
    case 32:
      one_color_all(0,255,0);     //---102- STRIP SOLID GREEN
      break;
    case 33:
      one_color_all(0,0,255);     //---103- STRIP SOLID BLUE
      break;
    case 34:
      one_color_all(255,255,0);   //---104- STRIP SOLID YELLOW
      break;
    case 35:
      one_color_all(0,255,255);   //---105- STRIP SOLID TEAL?
      break;
    case 36:
      one_color_all(255,0,255);   //---106- STRIP SOLID VIOLET?
      break;
    case 37:
      dream();   // protect your dreams
      break;
    case 38:
      firekithe();   // protect your dreams
      break;
    case 39:
      rainbow_pulse(10, 50);             //---RAINBOW LOOP
      break;
  }
}

void fastspi_strip::set_effect(int num) {
  effectNumber = num;
  Serial.print("~~~***NEW EFFECT-");
  Serial.println(effectNumber);
  if (effectNumber == 13) {  //-FOR CELL AUTO
    random_red();
  }
}
void fastspi_strip::next() {
  set_effect( (effectNumber+1) % MAX_EFFECT_NUMBER );
}
void fastspi_strip::prev() {
  if (effectNumber) {
    set_effect( (effectNumber-1) % MAX_EFFECT_NUMBER );
  } else {
    set_effect(MAX_EFFECT_NUMBER);
  }
}
//-SET THE COLOR OF A SINGLE RGB LED
void fastspi_strip::set_color_led(int idex, int cred, int cgrn, int cblu) {

  leds[idex].r = cred;
  leds[idex].g = cgrn;
  leds[idex].b = cblu;
}

void fastspi_strip::set_color_led(int idex, CRGB& color) {
  leds[idex] = color;
}


//-FIND INDEX OF HORIZONAL OPPOSITE LED
int fastspi_strip::horizontal_index(int i) {
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
  return num_leds - i;
}

//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int fastspi_strip::antipodal_index(int i) {
  //int N2 = int(num_leds/2);
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {
    iN = ( i + TOP_INDEX ) % num_leds;
  }
  return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int fastspi_strip::adjacent_cw(int i) {
  int r;
  if (i < num_leds - 1) {
    r = i + 1;
  }
  else {
    r = 0;
  }
  return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int fastspi_strip::adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = num_leds - 1;
  }
  return r;
}


//-CONVERT HSV VALUE TO RGB
void fastspi_strip::HSVtoRGB(int hue, int sat, int val, int colors[3]) {
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


void fastspi_strip::copy_led_array(){
  for(int i = 0; i < num_leds; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}


void fastspi_strip::print_led_arrays(int ilen){
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

void fastspi_strip::one_color_all(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
  set_color_led(idex, cred, cgrn, cblu);
  LEDS.show();
  delay(1);
  idex = (idex + 1) % num_leds;
}

void fastspi_strip::one_color_allNOSHOW(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
  for(int i = 0 ; i < num_leds; i++ ) {
    set_color_led(i, cred, cgrn, cblu);

  }  
}

void fastspi_strip::rainbow_strobe(int idelay) {
  rainbow_fade(idelay);
  one_color_allNOSHOW(0, 0, 0);
  LEDS.show();
  delay(idelay);
}

void fastspi_strip::rainbow_fade(int idelay) { //-FADE ALL LEDS THROUGH HSV RAINBOW
  ihue++;
  if (ihue >= 359) {
    ihue = 0;
  }
  int thisColor[3];
  HSVtoRGB(ihue, 255, 128, thisColor);
  one_color_allNOSHOW(thisColor[0],thisColor[1],thisColor[2]); 
  LEDS.show();
  delay(idelay);
}


void fastspi_strip::rainbow_pulse(int istep, int idelay) { //-LOOP HSV RAINBOW
  idex++;
  if (idex >= num_leds) {
    idex = 0;
  }

  one_color_allNOSHOW(0,0,0);
  int icolor[3];
  int count;
  int i = idex;
  ihue = 0;
  while (count < 50) {
    HSVtoRGB(ihue, 255, 255, icolor);
    set_color_led(i, icolor[0], icolor[1], icolor[2]);

    i = adjacent_ccw(i);
    ihue += 255 / 50;

    ++count;
  }

  LEDS.show();
  delay(idelay);
}
void fastspi_strip::rainbow_loop(int istep, int idelay) { //-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + istep;
  int icolor[3];  

  if (idex >= num_leds) {
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


void fastspi_strip::random_burst(int idelay) { //-RANDOM INDEX/COLOR
  int icolor[3];  

  idex = random(0,num_leds);
  ihue = random(0,359);

  HSVtoRGB(ihue, 255, 255, icolor);
  set_color_led(idex, icolor[0], icolor[1], icolor[2]);
  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::color_bounce(int idelay) { //-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == num_leds) {
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
  for(int i = 0; i < num_leds; i++ ) {
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


void fastspi_strip::police_lightsONE(int idelay) { //-POLICE LIGHTS (TWO COLOR SINGLE LED)
  idex++;
  if (idex >= num_leds) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);  
  for(int i = 0; i < num_leds; i++ ) {
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


void fastspi_strip::police_lightsALL(int idelay) { //-POLICE LIGHTS (TWO COLOR SOLID)
  idex++;
  if (idex >= num_leds) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  set_color_led(idexR, 255, 0, 0);
  set_color_led(idexB, 0, 0, 255);
  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::color_bounceFADE(int idelay) { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == num_leds) {
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

  for(int i = 0; i < num_leds; i++ ) {
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


void fastspi_strip::flicker(int thishue, int thissat) {
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  int thisColor[3];

  if (random_bool < 10) {
    HSVtoRGB(thishue, thissat, random_bright, thisColor);
    
    one_color_allNOSHOW(thisColor[0], thisColor[1], thisColor[2]);
    
    LEDS.show();    
    delay(random_delay);
  }
}

void fastspi_strip::flash(CRGB color, int times, int d) {
  for (int j=0; j < times; j++) {
    one_color_allNOSHOW(color.r, color.g, color.b);
    LEDS.show();
    delay(d);

    one_color_allNOSHOW(0,0,0);
    LEDS.show();
    delay(d);
  }
}

void fastspi_strip::pulse_one_color_all(int ahue, int idelay) { //-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR

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

  for(int i = 0 ; i < num_leds; i++ ) {
    set_color_led(i, acolor[0], acolor[1], acolor[2]);
  }
  LEDS.show();    
  delay(idelay);
}


void fastspi_strip::pulse_one_color_all_rev(int ahue, int idelay) { //-PULSE SATURATION ON ALL LEDS TO ONE COLOR

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

  for(int i = 0 ; i < num_leds; i++ ) {
    set_color_led(i, acolor[0], acolor[1], acolor[2]);
  }
  LEDS.show();
  delay(idelay);
}


void fastspi_strip::random_red() { //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED
  int temprand;
  for(int i = 0; i < num_leds; i++ ) {
    for(int c=0; c < 2; c++) {
        temprand = random(0,100);
      if (temprand > 50) {
        leds[i][c] = 255;
      }
      if (temprand <= 50) {
        leds[i][c] = 0;
      }
    }
  }
  LEDS.show();  
}


void fastspi_strip::rule30(int idelay) { //1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  copy_led_array();
  int iCW;
  int iCCW;
  int y = 100;  
  for(int i = 0; i < num_leds; i++ ) {
    iCW = adjacent_cw(i);
    iCCW = adjacent_ccw(i);
    for (int c=0; c < 2; c++) {
      if (ledsX[iCCW][c] > y && ledsX[i][c] > y && ledsX[iCW][c] > y) {
        leds[i][c] = 0;
      }
      if (ledsX[iCCW][c] > y && ledsX[i][c] > y && ledsX[iCW][c] <= y) {
        leds[i][c] = 0;
      }
      if (ledsX[iCCW][c] > y && ledsX[i][c] <= y && ledsX[iCW][c] > y) {
        leds[i][c] = 0;
      }
      if (ledsX[iCCW][c] > y && ledsX[i][c] <= y && ledsX[iCW][c] <= y) {
        leds[i][c] = 255;
      }
      if (ledsX[iCCW][c] <= y && ledsX[i][c] > y && ledsX[iCW][c] > y) {
        leds[i][c] = 255;
      }
      if (ledsX[iCCW][c] <= y && ledsX[i][c] > y && ledsX[iCW][c] <= y) {
        leds[i][c] = 255;
      }
      if (ledsX[iCCW][c] <= y && ledsX[i][c] <= y && ledsX[iCW][c] > y) {
        leds[i][c] = 255;
      }
      if (ledsX[iCCW][c] <= y && ledsX[i][c] <= y && ledsX[iCW][c] <= y) {
        leds[i][c] = 0;
      }
    }
  }

  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::random_march(int idelay) { //RANDOM MARCH CCW
  copy_led_array();
  int iCCW;

  int acolor[3];
  HSVtoRGB(random(0,360), 255, 255, acolor);

  leds[0].r = acolor[0];
  leds[0].g = acolor[1];
  leds[0].b = acolor[2];

  for(int i = 1; i < num_leds ; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }

  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::rwb_march(int idelay) { //R,W,B MARCH CCW
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
    leds[0].b = 255;
    break;
  case 1:
    leds[0].r = 0;
    leds[0].g = 255;
    leds[0].b = 255;
    break;
  case 2:
    leds[0].r = 0;
    leds[0].g = 0;
    leds[0].b = 255;
    break;
  }

  for(int i = 1; i < num_leds; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }

  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::white_temps() {
  int N9 = int(num_leds/9);
  for (int i = 0; i < num_leds; i++ ) {
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
    if (i >= N9*8 && i < num_leds) {
      leds[i].r = 64; 
      leds[i].g = 156; 
      leds[i].b = 255;
    } //-CLEAR BLUE SKY - 20000  
  }
  LEDS.show();  
  delay(100);
}


void fastspi_strip::color_loop_vardelay(int iperiod, int idelay) { //-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  idex++;
  if (idex > num_leds) {
    idex = 0;
  }

  int acolor[3];
  HSVtoRGB(0, 255, 255, acolor);

  int di = abs(iperiod - idex%iperiod); //-DISTANCE TO node
  //int t = constrain((10/di)*10, 10, 500); //-DELAY INCREASE AS INDEX APPROACHES CENTER (WITHIN LIMITS)
  int t = constrain(idelay * di / iperiod, 1, idelay);//-DELAY INCREASE AS INDEX APPROACHES node (WITHIN LIMITS)

  for(int i = 0; i < num_leds; i++ ) {
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


void fastspi_strip::strip_march_cw(int idelay) { //-MARCH STRIP C-W
  copy_led_array();
  int iCCW;  
  for(int i = 0; i < num_leds; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCCW = adjacent_ccw(i);
    leds[i].r = ledsX[iCCW][0];
    leds[i].g = ledsX[iCCW][1];
    leds[i].b = ledsX[iCCW][2];    
  }
  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::strip_march_ccw(int idelay) { //-MARCH STRIP C-W
  copy_led_array();
  int iCW;  
  for(int i = 0; i < num_leds; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    iCW = adjacent_cw(i);
    leds[i].r = ledsX[iCW][0];
    leds[i].g = ledsX[iCW][1];
    leds[i].b = ledsX[iCW][2];    
  }
  LEDS.show();  
  delay(idelay);
}

void fastspi_strip::strip_march_iw() { //-MARCH STRIP inward toward center
  for(int i = num_leds/2; i > 0; i-- ) {
    leds[i] = leds[i-1];
  }
  leds[0] = 0;
  for(int i = num_leds/2+1+EVENODD; i < num_leds-1; i++ ) {  //-GET/SET EACH LED COLOR FROM CCW LED
    leds[i] = leds[i+1];
  }
  leds[num_leds-1] = 0;
}


void fastspi_strip::pop_random(int ahue, int idelay) {
  int acolor[3];
  HSVtoRGB(ahue, 255, 255, acolor);

  int ix = random(num_leds);
  
  for(int i = 0; i < num_leds; i++ ) {
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
void fastspi_strip::pop_horizontal(int ahue, int idelay) {  //-POP FROM LEFT TO RIGHT UP THE RING
  int acolor[3];
  HSVtoRGB(ahue, 255, 255, acolor);

  int ix;

  if (bouncedirection == 0) {
    bouncedirection = 1;
    ix = idex;  
  }
  else if (bouncedirection == 1) {
    bouncedirection = 0;
    if (idex > TOP_INDEX) {
      ix = horizontal_index(idex);
    }
    else {
      ix = horizontal_index(idex - TOP_INDEX);
    }
    idex++;
    if (idex >= num_leds) {
      idex = 0;
    }      
  }

  for(int i = 0; i < num_leds; i++ ) {
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


void fastspi_strip::quad_bright_curve(int ahue, int idelay) {  //-QUADRATIC BRIGHTNESS CURVER
  int acolor[3];
  int ax;

  for(int x = 0; x < num_leds; x++ ) {
    if (x <= TOP_INDEX) {
      ax = x;
    }
    else if (x > TOP_INDEX) {
      ax = num_leds-x;
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


void fastspi_strip::flame() {
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


void fastspi_strip::radiation(int ahue, int idelay) { //-SORT OF RADIATION SYMBOLISH-
  //int N2 = int(num_leds/2);
  int N3 = int(num_leds/3);
  int N6 = int(num_leds/6);
  int N12 = int(num_leds/12);
  int acolor[3];

  for(int i = 0; i < N6; i++ ) { //-HACKY, I KNOW...
    tcount = tcount + .02;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount)*255);

    int j0 = (i + num_leds - N12) % num_leds;
    int j1 = (j0+N3) % num_leds;
    int j2 = (j1+N3) % num_leds;
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


void fastspi_strip::sin_bright_wave(int ahue, int idelay) {
  int acolor[3];

  for(int i = 0; i < num_leds; i++ ) {
    tcount = tcount + .1;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount)*255);

    HSVtoRGB(ahue, 255, ibright, acolor);

    leds[i].r = acolor[0]; 
    leds[i].g = acolor[1]; 
    leds[i].b = acolor[2];

  }
    LEDS.show();    
    delay(idelay);
}


void fastspi_strip::fade_vertical(int ahue, int idelay) { //-FADE 'UP' THE LOOP
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


void fastspi_strip::rainbow_vertical(int istep, int idelay) { //-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {
    idex = 0;
  }  
  ihue = ihue + istep;
  if (ihue > 359) {
    ihue = 0;
  }
  //Serial.println(ihue);
  int idexA = idex;
  int idexB = horizontal_index(idexA);

  int acolor[3];
  HSVtoRGB(ihue, 255, 255, acolor);

  set_color_led(idexA, acolor[0], acolor[1], acolor[2]);  
  set_color_led(idexB, acolor[0], acolor[1], acolor[2]);  

  LEDS.show();  
  delay(idelay);
}


void fastspi_strip::firekithe() {

  if (bouncedirection == 0) {
    ibright+=10;
    if (ibright >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ibright -= 10;
    if (ibright <= 1) {
      bouncedirection = 0;
      kithestate = (kithestate + 1) % 3;
    }
  }

  one_color_allNOSHOW(0,0,0);
  int acolor[3];
  HSVtoRGB(30, 255, ibright, acolor);
  if (kithestate == 0) {

    // bottom left
    for(int i = 300 ; i < 360; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // bottom right
    for(int i = 633 ; i < 693; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }
  } else if (kithestate == 1) {
    // bottom + middle left
    for(int i = 300 ; i < 423; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // bottom + middle right
    for(int i = 573 ; i < 693; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }
    // eyeball 1
    for(int i = 0 ; i < 15; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }
    for(int i = 45 ; i < 60; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // eyeball 2
    for(int i = 766 ; i < 781; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }
    for(int i = 810 ; i < 825; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }
  } else {
    // eyeball 1
    for(int i = 0 ; i < 60; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // bottom left
    for(int i = 300 ; i < 360; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    //bottom right
    for(int i = 630 ; i < 693; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // top left
    for(int i = 423 ; i < 490; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // top right
    for(int i = 508 ; i < 573; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }

    // eyeball 2
    for(int i = 766 ; i < 825; i++ ) {
      set_color_led(i, acolor[0], acolor[1], acolor[2]);
    }
  }
  LEDS.show();
  delay(idelay);
}
void fastspi_strip::dream() { //-MARCH STRIP C-W
  idex = idex + 1;
  if (idex >= num_leds) {
    idex = 0;
  }
  ihue++;
  if (ihue >= 180) {
    ihue = 120;
  }
  int blue = 200;
  int gold = 60;
  int thisColor[3];
  one_color_allNOSHOW(0,0,0);
  for (int i=0; i < num_leds; i+=20) {
    if (ihue != 0) {
      ihue = 0;
    } else {
      ihue = 255;
    }
    int x = (i+idex) % num_leds;
    int brightness = 200;
    HSVtoRGB(blue, ihue, brightness, thisColor);
    set_color_led(x, thisColor[0],thisColor[1],thisColor[2]);
    int L = adjacent_cw(x);
    int R = adjacent_ccw(x);
    for (int j=0; j < 13; ++j) { // width of the pulses
      brightness = brightness / 2;
      HSVtoRGB(blue, ihue, brightness, thisColor);

      set_color_led(L , thisColor[0],thisColor[1],thisColor[2] );
      set_color_led(R , thisColor[0],thisColor[1],thisColor[2] );
      L = adjacent_cw(L);
      R = adjacent_ccw(R);
    }
  }

  // random bursts
  if (random(0,10) > 5) {
    set_color_led(random(0,num_leds), 255, 255, 0);
  }

  LEDS.show();

  if (bouncedirection == 0) {
    idelay = idelay + 4;      // slowing down
    if (idelay >= 100) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    idelay = idelay - 2;        // speeding up
    if (idelay <= -num_leds/2) { // go negative, spend more time in fast modejjj
      bouncedirection = 0;
    }
  }
  // clamp(idelay, 0, 300)
  int t = idelay < 0 ? 0 : (idelay > 100 ? 100 : idelay);
  delay(t);
}

void fastspi_strip::pacman(int idelay) { //-MARCH STRIP C-W
  int s = int(num_leds/4);
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

//int low_beat, mid_beat, high_beat;
void fastspi_strip::beat_march(int iwidth) {
  /* todo
  if ( !low_beat && spectrum(LOW_SPECTRUM) ) {
    low_beat = iwidth;
  }
  if (!mid_beat && spectrum(MID_SPECTRUM)) {
    mid_beat = iwidth;
  }
  if (!high_beat && spectrum(HIGH_SPECTRUM)) {
    high_beat = iwidth;
  }

  if (low_beat) {
    leds[num_leds-1].r = 255;
    low_beat--;
  } else {
    leds[num_leds-1].r = 0;
  }

  if (mid_beat) {
    leds[num_leds-1].g = 255;
    mid_beat--;
  } else {
    leds[num_leds-1].g = 0;
  }

  if (high_beat) {
    leds[num_leds-1].b = 255;
    high_beat--;
  } else {
    leds[num_leds-1].b = 0;
  }
  strip_march_cw(0);
  */
}
