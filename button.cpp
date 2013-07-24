#include "button.h"
Button::Button(int p) : pin(p), oldval(false) {
        pinMode(p, INPUT);
        digitalWrite(p, HIGH);
}

boolean Button::pressed() {
        boolean newval = read();
        boolean ret = (newval == 0) && (newval != oldval);

        oldval = newval;
        return ret;
}

int Button::read() {
        return digitalRead(pin);
}
