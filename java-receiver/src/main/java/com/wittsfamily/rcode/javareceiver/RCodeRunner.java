package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeRunner {
    private final RCode rcode;
    private RCodeCommandSequence[] running;
    private int parallelNum = 0;
    private boolean canBeParallel = false;

    public RCodeRunner(RCode rcode, RCodeParameters params) {
        this.rcode = rcode;
        this.running = new RCodeCommandSequence[params.maxParallelRunning];
    }

    private RCodeCommandSequence findNewSequenceToRun() {
        RCodeCommandSequence current = null;
        RCodeCommandChannel[] channels = rcode.getChannels();
        if (canBeParallel || parallelNum == 0) {
            canBeParallel = true;
            for (int i = 0; i < channels.length; i++) {
                if (channels[i].getCommandSequence().isFullyParsed() && !channels[i].getOutStream().isLocked() && channels[i].getCommandSequence().canLock()
                        && channels[i].getCommandSequence().canBeParallel() && channels[i].canLock()) {
                    current = channels[i].getCommandSequence();
                    channels[i].lock();
                    current.lock();
                    break;
                }
            }
        }
        if (canBeParallel && current == null && parallelNum == 0) {
            for (int i = 0; i < channels.length; i++) {
                if (channels[i].getCommandSequence().isActive() && !channels[i].getOutStream().isLocked() && channels[i].canLock()) {
                    current = channels[i].getCommandSequence();
                    channels[i].lock();
                    canBeParallel = false;
                    break;
                }
            }
        }
        if (current != null) {
            RCodeOutStream out = current.getOutStream();
            if (out.isOpen() && out.mostRecent != current) {
                out.close();
            }
            out.mostRecent = current;
            if (!out.isOpen()) {
                out.openResponse(current.getChannel());
            }
            if (current.isBroadcast()) {
                out.markBroadcast();
            }
            running[parallelNum++] = current;
            if (current.isEmpty() && finishRunning(current, parallelNum - 1)) {
                current = null;
            }
        }
        return current;
    }

    public void runNext() {
        RCodeCommandSequence current = null;
        int targetInd = 0;
        for (; targetInd < parallelNum; targetInd++) {
            if (!running[targetInd].hasParsed() || running[targetInd].peekFirst().isComplete() || !running[targetInd].peekFirst().isStarted()) {
                current = running[targetInd];
                break;
            }
        }
        if (current != null && (!current.hasParsed() || current.peekFirst().isComplete())) {
            if (finishRunning(current, targetInd)) {
                current = null;
            }
        }
        if (current == null && !canBeParallel && parallelNum == 1 && running[0].canBeParallel() && running[0].isFullyParsed() && running[0].canLock()) {
            running[0].lock();
            canBeParallel = true;
        }
        if (current == null && parallelNum < running.length) {
            targetInd = parallelNum;
            current = findNewSequenceToRun();
        }
        if (current != null && current.hasParsed()) {
            runSequence(current, targetInd);
        }
    }

    private void runSequence(RCodeCommandSequence target, int targetInd) {
        RCodeOutStream out = target.getOutStream();
        RCodeCommandSlot cmd = target.peekFirst();
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
                target.getOutStream().writeCommandSequenceSeperator();
                target.fail(RCodeResponseStatus.UNKNOWN_CMD);
                finishRunning(target, targetInd);
            } else if (Byte.toUnsignedInt(cmd.getFields().get('R', (byte) 0xFF)) > RCodeActivateCommand.MAX_SYSTEM_CODE && !RCodeActivateCommand.isActivated()) {
                out.writeStatus(RCodeResponseStatus.NOT_ACTIVATED);
                out.writeBigStringField("Not a system command, and not activated");
                target.getOutStream().writeCommandSequenceSeperator();
                target.fail(RCodeResponseStatus.NOT_ACTIVATED);
                finishRunning(target, targetInd);
            } else {
                cmd.start();
                c.execute(cmd, target, out);
            }
        }
    }

    private boolean finishRunning(RCodeCommandSequence target, int targetInd) {
        if (target.hasParsed()) {
            RCodeCommandSlot slot = target.peekFirst();
            if (slot.isStarted()) {
                slot.getCommand().finish(slot, target.getOutStream());
            }
            if (slot.getStatus() != RCodeResponseStatus.OK) {
                target.getOutStream().writeCommandSequenceSeperator();
                target.fail(slot.getStatus());
            } else if (slot.getEnd() == '\n' || (target.isFullyParsed() && slot.next == null)) {
                target.getOutStream().writeCommandSequenceSeperator();
            } else {
                target.getOutStream().writeCommandSeperator();
            }
            target.popFirst();
            slot.reset();
        } else if (target.isEmpty()) {
            target.getOutStream().writeCommandSequenceSeperator();
        }
        if (!target.hasParsed() && target.isFullyParsed()) {
            if (!target.getChannel().isPacketBased() || !target.getChannel().hasCommandSequence()) {
                target.getOutStream().close();
            }
            target.releaseOutStream();
            if (!target.canBeParallel()) {
                canBeParallel = true;
            } else if (parallelNum == 1) {
                canBeParallel = true;
            }
            target.unlock();
            target.getChannel().unlock();
            target.reset();
            for (int i = targetInd; i < parallelNum - 1; i++) {
                running[i] = running[i + 1];
            }
            parallelNum--;
            return true;
        }
        return false;
    }

}
