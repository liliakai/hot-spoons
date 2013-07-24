#include "Arduino.h"

class Button {
        int pin;
public:
        Button(int p);
        boolean pressed();
        int read();
};
