#include "Arduino.h"

class Button {
        int pin;
        boolean oldval;
public:
        Button(int p);
        boolean pressed();
        int read();
};
