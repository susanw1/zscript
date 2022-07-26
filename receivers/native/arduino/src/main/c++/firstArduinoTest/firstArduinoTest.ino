

#include "ZcodeParameters.hpp"

#include <ZcodeFullInclude.hpp>

#include <Wire.h>


void setup() {
    Serial.begin(9600);
    Wire.begin(0x61);
    ZcodeSetup.setup();
}

void loop() {
    Zcode<ZcodeParams>::zcode.progressZcode();
}
