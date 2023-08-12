#include "ZscriptParameters.hpp"
#include <ZscriptFullInclude.hpp>

void setup() {
    Serial.begin(9600);
    ZscriptI2cChannel.setAddress(0x61);
    ZscriptSetup.setup();
}

void loop() {
    Zscript::Zscript<ZscriptParams>::zscript.progress();
}
