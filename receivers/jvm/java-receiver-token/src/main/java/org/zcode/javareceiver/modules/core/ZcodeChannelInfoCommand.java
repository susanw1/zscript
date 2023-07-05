package org.zcode.javareceiver.modules.core;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeChannelInfoCommand {

    public static void execute(ZcodeCommandView view) {
        List<ZcodeChannel> chs = Zcode.getZcode().getChannels();

        int current = 0;
        for (ZcodeChannel ch : chs) {
            if (ch.getOutStream() == view.getOutStream()) {
                break;
            }
            current++;
        }
        view.getOutStream().writeField('C', chs.size());
        if (current <= chs.size()) {
            view.getOutStream().writeField('U', current);
        }
        int target = view.getField((byte) 'C', current);
        if (target >= chs.size()) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        chs.get(target).channelInfo(view);
    }

}
