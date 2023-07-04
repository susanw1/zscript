package org.zcode.javareceiver.modules.core;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.execution.ZcodeField;
import org.zcode.javareceiver.tokenizer.OptIterator;

public class ZcodeEchoCommand {

    public static void execute(ZcodeCommandView view) {
        ZcodeOutStream          out      = view.getOutStream();
        OptIterator<ZcodeField> iterator = view.fieldIterator();
        for (Optional<ZcodeField> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ZcodeField field = opt.get();
            if (field.getKey() == 'S') {
                int status = field.getValue();
                view.status((byte) status);
                continue;
            }
            if (field.getKey() != 'Z') {
                out.writeField(field);
            }
        }
    }

}
