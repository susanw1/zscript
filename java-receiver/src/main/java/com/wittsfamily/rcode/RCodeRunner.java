package com.wittsfamily.rcode;

import com.wittsfamily.rcode.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.commands.RCodeCommand;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeCommandSlot;

public class RCodeRunner {
    private final RCode rcode;
    private RCodeCommandSequence[] running;
    private int parallelNum = 0;
    private boolean canBeParallel = false;

    public RCodeRunner(RCode rcode, RCodeParameters params) {
        this.rcode = rcode;
        this.running = new RCodeCommandSequence[params.maxParallelRunning];
    }

    public void runNext() {
        RCodeCommandSequence current = null;
        int targetInd = 0;
        for (; targetInd < parallelNum; targetInd++) {
            if (running[targetInd].peekFirst().isComplete()) {
                current = running[targetInd];
                break;
            }
        }
        if (current != null) {
            if (current.peekFirst().isStarted()) {
                current.peekFirst().getCommand().finish(current.peekFirst(), current.getOutStream());
                current.peekFirst().reset();
            }
            if (current.peekFirst().getEnd() == '\n') {
                current.getOutStream().writeCommandSequenceSeperator();
            } else {
                current.getOutStream().writeCommandSeperator();
            }
            current.peekFirst().reset();
            current.popFirst();
            if (current.peekFirst() == null) {
                if (!current.getChannel().isPacketBased()) {
                    current.getOutStream().close();
                }
                current.releaseOutStream();
                if (!current.canBeParallel()) {
                    canBeParallel = true;
                } else if (parallelNum == 1) {
                    canBeParallel = true;
                }
                current.unlock();
                current.reset();
                for (int i = targetInd; i < parallelNum - 1; i++) {
                    running[i] = running[i + 1];
                }
                current = null;
                parallelNum--;
            }
        }
        if (current == null && !canBeParallel && parallelNum == 1 && running[0].canBeParallel() && running[0].isFullyParsed() && running[0].canLock()) {
            running[0].lock();
            canBeParallel = true;
        }
        if (current == null && parallelNum < running.length) {
            RCodeCommandChannel[] channels = rcode.getChannels();
            if (canBeParallel || parallelNum == 0) {
                canBeParallel = true;
                for (int i = 0; i < channels.length; i++) {
                    if (!channels[i].getOutStream().isLocked() && channels[i].getCommandSequence().isFullyParsed() && channels[i].getCommandSequence().canLock()
                            && channels[i].getCommandSequence().canBeParallel()) {
                        current = channels[i].getCommandSequence();
                        current.lock();
                        break;
                    }
                }
            }
            if (canBeParallel && current == null && parallelNum == 0) {
                for (int i = 0; i < channels.length; i++) {
                    if (!channels[i].getOutStream().isLocked() && channels[i].getCommandSequence().peekFirst() != null) {
                        current = channels[i].getCommandSequence();
                        canBeParallel = false;
                        break;
                    }
                }
            }
            if (current != null) {
                RCodeOutStream out = current.getOutStream();
                out.lock();
                if (out.isOpen() && out.mostRecent != current) {
                    out.close();
                    out.mostRecent = current;
                }
                out.openResponse(current.getChannel());
                if (current.isBroadcast()) {
                    out.markBroadcast();
                }
                running[parallelNum++] = current;
            }
        }
        if (current != null) {
            runSequence(current);
        }
    }

    private void runSequence(RCodeCommandSequence current) {
        RCodeOutStream out = current.getOutStream();
        RCodeCommandSlot cmd = current.peekFirst();
        cmd.getFields().copyFieldTo(out, 'E');
        if (cmd.getStatus() != RCodeResponseStatus.OK) {
            cmd.setComplete(true);
            out.writeStatus(cmd.getStatus());
            out.writeBigStringField(cmd.getErrorMessage());
        } else {
            RCodeCommand c = cmd.getCommand();
            if (c == null) {
                out.writeStatus(RCodeResponseStatus.UNKNOWN_CMD);
                out.writeBigStringField("Command not found");
                current.fail();
            } else if (Byte.toUnsignedInt(cmd.getFields().get('R', (byte) 0xFF)) > RCodeActivateCommand.MAX_SYSTEM_CODE && !RCodeActivateCommand.isActivated()) {
                out.writeStatus(RCodeResponseStatus.NOT_ACTIVATED);
                out.writeBigStringField("Not a system command, and not activated");
                current.fail();
            } else {
                cmd.start();
                c.execute(cmd, current, out);
            }
        }
    }

}
