package org.zcode.javareceiver.modules.outerCore;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeChannelSetupCommand {

    public static void execute(ZcodeCommandView view) {
        List<ZcodeChannel> chs = Zcode.getZcode().getChannels();

        int current = 0;
        for (ZcodeChannel ch : chs) {
            if (ch.getOutStream() == view.getOutStream()) {
                break;
            }
            current++;
        }
        int target = view.getField((byte) 'C', current);
        if (target >= chs.size()) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        chs.get(target).channelSetup(view);
    }

}
