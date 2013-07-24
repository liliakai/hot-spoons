#include "button.h"
Button::Button(int p) : pin(p) {
        pinMode(p, INPUT);
        digitalWrite(p, HIGH);
}

boolean Button::pressed() {
        static boolean oldval = false;

        boolean newval = read();
        boolean ret = (newval == 0) && (newval != oldval);

        oldval = newval;
        return ret;
}

int Button::read() {
        return digitalRead(pin);
}
