#include "ZcodeParameters.hpp"
#include <ZcodeFullInclude.hpp>

void setup() {
    Serial.begin(9600);
    ZcodeI2cChannelI.setAddress(0x61);
    ZcodeSetup.setup();
}

void loop() {
    Zcode<ZcodeParams>::zcode.progressZcode();
}
