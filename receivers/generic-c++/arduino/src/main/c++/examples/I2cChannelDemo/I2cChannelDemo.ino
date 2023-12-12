#include "ZscriptParameters.hpp"
#include <ZscriptFullInclude.hpp>

void setup() {
    ZscriptSetup.setup();
}

void loop() {
    ZscriptSetup.pollAll();
    Zscript::Zscript<ZscriptParams>::zscript.progress();
}
