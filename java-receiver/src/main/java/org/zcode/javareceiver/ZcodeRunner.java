package org.zcode.javareceiver;

import org.zcode.javareceiver.commands.ZcodeActivateCommand;
import org.zcode.javareceiver.commands.ZcodeCommand;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;
import org.zcode.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeRunner {
    private final Zcode                  zcode;
    private final ZcodeCommandSequence[] running;
    
    private int                          parallelNum   = 0;
    private boolean                      canBeParallel = false;

    public ZcodeRunner(final Zcode zcode, final ZcodeParameters params) {
        this.zcode = zcode;
        this.running = new ZcodeCommandSequence[params.maxParallelRunning];
    }

    private ZcodeCommandSequence findNewSequenceToRun() {
        ZcodeCommandSequence        current  = null;
        final ZcodeCommandChannel[] channels = zcode.getChannels();
        if (canBeParallel || parallelNum == 0) {
            canBeParallel = true;
            for (int i = 0; i < channels.length; i++) {
                if (channels[i].getCommandSequence().isFullyParsed() && channels[i].getCommandSequence().canLock() && channels[i].getCommandSequence().canBeParallel()
                        && channels[i].canLock() && (!channels[i].hasOutStream() || !channels[i].acquireOutStream().isLocked())) {
                    current = channels[i].getCommandSequence();
                    channels[i].lock();
                    current.lock();
                    break;
                }
            }
        }
        if (canBeParallel && current == null && parallelNum == 0) {
            for (int i = 0; i < channels.length; i++) {
                if (channels[i].getCommandSequence().isActive() && channels[i].canLock() && (!channels[i].hasOutStream() || !channels[i].acquireOutStream().isLocked())) {
                    current = channels[i].getCommandSequence();
                    channels[i].lock();
                    canBeParallel = false;
                    break;
                }
            }
        }
        if (current != null) {
            final ZcodeOutStream out = current.acquireOutStream();
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
        ZcodeCommandSequence current   = null;
        int                  targetInd = 0;
        for (; targetInd < parallelNum; targetInd++) {
            if (!running[targetInd].hasParsed() || running[targetInd].peekFirst().isComplete() || running[targetInd].peekFirst().needsMoveAlong()
                    || !running[targetInd].peekFirst().isStarted()) {
                current = running[targetInd];
                break;
            }
        }
        if (current != null && current.peekFirst().needsMoveAlong()) {
            current.peekFirst().setNeedsMoveAlong(false);
            current.peekFirst().getCommand().moveAlong(current.peekFirst());
            current = null;
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

    private void runSequence(final ZcodeCommandSequence target, final int targetInd) {
        final ZcodeOutStream   out = target.acquireOutStream();
        final ZcodeCommandSlot cmd = target.peekFirst();
        cmd.getFields().copyFieldTo(out, Zchars.ECHO_PARAM.ch);

        if (cmd.getStatus() != ZcodeResponseStatus.OK) {
            cmd.setComplete(true);
            out.writeStatus(cmd.getStatus());
            out.writeBigStringField(cmd.getErrorMessage());
        } else {
            final ZcodeCommand c = cmd.getCommand();
            if (c == null) {
                out.writeStatus(ZcodeResponseStatus.UNKNOWN_CMD);
                out.writeBigStringField("Command not found");
                target.acquireOutStream().writeCommandSequenceSeparator();
                target.fail(ZcodeResponseStatus.UNKNOWN_CMD);
                finishRunning(target, targetInd);
            } else if (Byte.toUnsignedInt(cmd.getFields().get(Zchars.CMD_PARAM.ch, (byte) 0xFF)) > ZcodeActivateCommand.MAX_SYSTEM_CODE && !ZcodeActivateCommand.isActivated()) {
                out.writeStatus(ZcodeResponseStatus.NOT_ACTIVATED);
                out.writeBigStringField("Not a system command, and not activated");
                target.acquireOutStream().writeCommandSequenceSeparator();
                target.fail(ZcodeResponseStatus.NOT_ACTIVATED);
                finishRunning(target, targetInd);
            } else {
                cmd.start();
                c.execute(cmd, target, out);
            }
        }
    }

    private boolean finishRunning(final ZcodeCommandSequence target, final int targetInd) {
        if (target.hasParsed()) {
            final ZcodeCommandSlot slot = target.peekFirst();
            if (slot.isStarted()) {
                slot.getCommand().finish(slot, target.acquireOutStream());
            }
            if (slot.getStatus() != ZcodeResponseStatus.OK) {
                if (target.fail(slot.getStatus())) {
                    target.acquireOutStream().writeCommandSequenceErrorHandler();
                } else {
                    target.acquireOutStream().writeCommandSequenceSeparator();
                }
            } else if (slot.getEnd() == Zchars.EOL_SYMBOL.ch || (target.isFullyParsed() && slot.next == null)) {
                target.acquireOutStream().writeCommandSequenceSeparator();
            } else if (slot.getEnd() == Zchars.ANDTHEN_SYMBOL.ch) {
                target.acquireOutStream().writeCommandSeparator();
            } else {
                target.fail(ZcodeResponseStatus.UNKNOWN_ERROR);
                target.acquireOutStream().writeCommandSequenceSeparator();
            }
            target.popFirst();
            slot.reset();
        } else if (target.isEmpty()) {
            target.acquireOutStream().writeCommandSequenceSeparator();
            target.releaseOutStream();
        }
        if (!target.hasParsed() && target.isFullyParsed()) {
            if (!target.getChannel().isPacketBased() || !target.getChannel().hasCommandSequence()) {
                target.acquireOutStream().close();
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
