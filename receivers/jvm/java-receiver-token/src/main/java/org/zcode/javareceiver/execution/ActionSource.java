package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeOutStream;

public interface ActionSource {
    ZcodeAction getAction();

    ZcodeOutStream getOutStream(Zcode zcode);
}
