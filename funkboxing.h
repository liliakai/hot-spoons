#include <FastSPI_LED2.h>

class fastspi_strip {
public:

  int BOTTOM_INDEX;
  int TOP_INDEX;
  int EVENODD;
  int num_leds;

  struct CRGB* leds;
  int** ledsX;

  int ledMode;

  int idex;             //-LED INDEX (0 to num_leds-1
  int ihue;             //-HUE (0-360)
  int ibright;          //-BRIGHTNESS (0-255)
  int isat;             //-SATURATION (0-255)
  int bouncedirection;  //-SWITCH FOR COLOR BOUNCE (0-1)
  int lcount;
  float tcount;

  fastspi_strip(int n);
  void loop();
  void setup();
  void set_color_led(int adex, int cred, int cgrn, int cblu);
  int horizontal_index(int i);
  int antipodal_index(int i);
  int adjacent_cw(int i);
  int adjacent_ccw(int i);
  void HSVtoRGB(int hue, int sat, int val, int colors[3]);
  void copy_led_array();
  void print_led_arrays(int ilen);
  void one_color_all(int cred, int cgrn, int cblu) ; //-SET ALL LEDS TO ONE COLOR
  void one_color_allNOSHOW(int cred, int cgrn, int cblu) ; //-SET ALL LEDS TO ONE COLOR
  void rainbow_strobe(int idelay) ;
  void rainbow_fade(int idelay) ; //-FADE ALL LEDS THROUGH HSV RAINBOW
  void rainbow_loop(int istep, int idelay) ; //-LOOP HSV RAINBOW
  void random_burst(int idelay) ; //-RANDOM INDEX/COLOR
  void color_bounce(int idelay) ; //-BOUNCE COLOR (SINGLE LED)
  void police_lightsONE(int idelay) ; //-POLICE LIGHTS (TWO COLOR SINGLE LED)
  void police_lightsALL(int idelay) ; //-POLICE LIGHTS (TWO COLOR SOLID)
  void color_bounceFADE(int idelay) ; //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  void flicker(int thishue, int thissat) ;
  void flash(CRGB color, int times, int d);
  void pulse_one_color_all(int ahue, int idelay) ; //-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR 
  void pulse_one_color_all_rev(int ahue, int idelay) ; //-PULSE SATURATION ON ALL LEDS TO ONE COLOR 
  void random_red() ; //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED  
  void rule30(int idelay) ; //1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  void random_march(int idelay) ; //RANDOM MARCH CCW
  void rwb_march(int idelay) ; //R,W,B MARCH CCW
  void white_temps() ;
  void color_loop_vardelay(int iperiod, int idelay) ; //-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  void strip_march_cw(int idelay) ; //-MARCH STRIP C-W
  void strip_march_ccw(int idelay) ; //-MARCH STRIP C-W 
  void pop_random(int ahue, int idelay) ;
  void pop_horizontal(int ahue, int idelay) ;  //-POP FROM LEFT TO RIGHT UP THE RING
  void quad_bright_curve(int ahue, int idelay) ;  //-QUADRATIC BRIGHTNESS CURVER
  void flame() ;
  void radiation(int ahue, int idelay) ; //-SORT OF RADIATION SYMBOLISH- 
  void sin_bright_wave(int ahue, int idelay) ;  
  void fade_vertical(int ahue, int idelay) ; //-FADE 'UP' THE LOOP
  void rainbow_vertical(int istep, int idelay) ; //-RAINBOW 'UP' THE LOOP
  void pacman(int idelay) ; //-MARCH STRIP C-W
  void beat_march(int iwidth) ;
  void demo_mode();
  void set_mode(int newMode);
  void strip_march_iw();
};
