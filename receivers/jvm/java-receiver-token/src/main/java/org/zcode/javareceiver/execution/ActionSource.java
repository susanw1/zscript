package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction;

public interface ActionSource {
    ZcodeSemanticAction getAction();

    ZcodeOutStream getOutStream();
}
