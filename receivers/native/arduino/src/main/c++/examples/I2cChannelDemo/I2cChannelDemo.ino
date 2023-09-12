#include "ZscriptParameters.hpp"
#include <ZscriptFullInclude.hpp>

void setup() {
    Serial.begin(9600);
    ZscriptSetup.setup();
}

void loop() {
    //Zscript::ZscriptServoModule<ZscriptParams>::moveAlongServos();
    Zscript::Zscript<ZscriptParams>::zscript.progress();
}
