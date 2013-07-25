#define TONEPIN 53  // which pin sound comes out of
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
