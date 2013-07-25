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

