

#include <ZcodeDefaultParameters.hpp>


#include <ZcodeFullInclude.hpp>


void setup() {
    Serial.begin(9600);
    Serial.println("hi");
    ZcodeSetup.setup();
}

void loop() {
    ZcodeI.progressZcode();
}
