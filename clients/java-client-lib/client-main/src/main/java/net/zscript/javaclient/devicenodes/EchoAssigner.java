package net.zscript.javaclient.devicenodes;

import java.util.BitSet;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Utility for keeping track of echo-field values, which are used to reconnect responses to their corresponding command-sequence messages.
 * <p>Echo fields are assigned from here to CommandSequences which don't already have an echo value, and this class does the bookkeeping. The goal is to be able to reserve values
 * for the brief amount of time that is needed between call and response, accommodating the fact that in a complex nested hierarchy of a device tree, many messages may be in-flight
 * at once.
 * <p>Values are allocated out of a block of size 256, and once a response has been received and matched, the value may be reused. The first block starts at 256 (0x100), with the
 * space below (0-0xff) being reserved for manually set echo values.
 * <p>Echo values are reserved in the current block by messages that need them, and are freed when their matching response comes in. If a message times out, we are notified, and
 * if the current block becomes over-full of timed-out messages, then we move to allocating from a fresh block, immediately above the current one (eg 512, etc) and looping back to
 * 256 once the 2-byte space is exhausted. The previous block is retained, so that slightly late-arriving messages can be signed off properly.
 * <p>Note: the separation between getEcho, and moveEcho and manualEchoUse, creates a delicate coupling between ConnectionBuffer and EchoAssigner. This should be fixed. Also, the
 * block mechanism could be simplified by refactoring into a Block class.
 */
public class EchoAssigner {
    private static final Logger LOG = LoggerFactory.getLogger(EchoAssigner.class);

    /** Sets the number of messages which can timeout before a block is considered overly contaminated with timed-out messages, requiring a block shift. */
    private final static int BLOCK_MAX_TIMEOUTS_BEFORE_CHANGE = 0x0010;
    /** High water mark, triggering warning messages. */
    private final static int BLOCK_MAX_RESERVATIONS           = 0x00C0;
    /** Size of a block. */
    private final static int BLOCK_SIZE                       = 0x0100;
    /** Start offset of the first and lowest assigned block. */
    private final static int LOWEST_BLOCK_LEVEL               = 0x0100;

    private Block currentBlock;
    private Block previousBlock;

    EchoAssigner() {
        currentBlock = new Block();
        previousBlock = currentBlock;
    }

    /**
     * Retrieves the next free echo value. Note that {@link #moveEcho()} must be called before the next call to move the echo value on.
     *
     * @return an available echo value
     */
    public int getEcho() {
        return currentBlock.getCurrentEchoValue();
    }

    /**
     * Marks the current echo as "in flight" (the message is queued for sending and we're waiting for a response), and moves the current offset along.
     * <p>A warning is logged if the current block is getting full.
     *
     * @throws IllegalStateException if the current echo is already "in flight", or no values are available.
     */
    public void moveEcho() {
        currentBlock.moveEcho();
    }

    /**
     * Marks the supplied echo value as being manually assigned. We can't stop manual assignment, because an echo field can be written straight into the Zscript, so we just have to
     * track the value and not reuse it until it's freed up.
     *
     * @param echo the echo value to mark
     */
    public void manualEchoUse(int echo) {
        if (currentBlock.inRange(echo)) {
            currentBlock.manualEchoUse(echo);
        } else if (previousBlock.inRange(echo)) {
            previousBlock.setReservation(echo);
        }
    }

    /**
     * Determines whether the supplied echo value is reserved, and we're waiting for a response.
     *
     * @param echo the echo value to check
     * @return true if it's reserved, false otherwise
     */
    public boolean isReserved(int echo) {
        return currentBlock.isReserved(echo) || previousBlock.isReserved(echo);
    }

    /**
     * Notification that a response has been received, so that its value can be deregistered and freed up.
     *
     * @param echo the echo value to be released
     */
    public void responseArrivedNormal(int echo) {
        Block b = currentBlock;
        if (!b.inRange(echo)) {
            b = previousBlock;
            if (!b.inRange(echo)) {
                return;
            }
        }
        b.clearReservation(echo);
    }

    /**
     * Notification that a response with the supplied echo value has not been received within the timeout period. If the number of timed out values in the current block exceeds a
     * limit, then assign a new block, and move the current one to "previous" (abandoning the old "previous" block).
     *
     * @param echo the echo value of the message that has timed out
     */
    public void timeout(int echo) {
        if (!currentBlock.inRange(echo)) {
            return;
        }

        if (currentBlock.isReserved(echo) && currentBlock.timeout(echo)) {
            LOG.info("Lingering timeout count: ({}). Changing echo value block.", currentBlock.timeoutCount);
            previousBlock = currentBlock;
            currentBlock = currentBlock.createNext();
        }
    }

    /**
     * Handle the case where a response has come in which matches no messages in the buffer - if the echo value is reserved, then release it anyway.
     *
     * @param echo the echo value of the response that is unmatched
     * @return true if the value was reserved, false otherwise
     */
    public boolean unmatchedReceive(int echo) {
        Block b = currentBlock;
        if (!b.inRange(echo)) {
            b = previousBlock;
            if (!b.inRange(echo)) {
                return false;
            }
        }
        return b.clearReservation(echo);
    }

    int getTimeoutCount() {
        return currentBlock.timeoutCount;
    }

    int getReservationCount() {
        return currentBlock.reservationCount;
    }

    static class Block {
        private       BitSet reservations = new BitSet(BLOCK_SIZE);
        private final int    baseOffset;

        private int timeoutCount      = 0;
        /** Number of reserved echo values on in-flight messages awaiting response */
        private int reservationCount  = 0;
        /**
         * The offset in the current block for the echo value which is next available (see {@link #getEcho()}) - this always references an available value in the current block
         */
        private int currentEchoOffset = 0;

        Block() {
            this.baseOffset = LOWEST_BLOCK_LEVEL;
        }

        Block(int baseOffset) {
            this.baseOffset = baseOffset;
        }

        Block createNext() {
            return new Block(baseOffset + BLOCK_SIZE < 0xffff ? baseOffset + BLOCK_SIZE : LOWEST_BLOCK_LEVEL);
        }

        boolean inRange(int echoValue) {
            return echoValue >= baseOffset && echoValue < baseOffset + BLOCK_SIZE;
        }

        public int getCurrentEchoValue() {
            return currentEchoOffset + baseOffset;
        }

        public void moveEcho() {
            if (reservationCount >= BLOCK_MAX_RESERVATIONS) {
                LOG.warn("Too many messages waiting for response ({}). Reduce command rate or latency.", reservationCount);
            }
            if (reservations.get(currentEchoOffset)) {
                throw new IllegalStateException("Current echo value already set");
            }
            reservations.set(currentEchoOffset);
            currentEchoOffset = reservations.nextClearBit(currentEchoOffset + 1);
            if (currentEchoOffset == BLOCK_SIZE) {
                // search again, starting at the beginning of the block
                currentEchoOffset = reservations.nextClearBit(0);
                if (currentEchoOffset == BLOCK_SIZE) {
                    throw new IllegalStateException("Ran out of echo values to assign");
                }
            }
            reservationCount++;
        }

        public void manualEchoUse(int echo) {
            int relativeEchoOffset = echo - baseOffset;
            if (relativeEchoOffset >= 0 && relativeEchoOffset < BLOCK_SIZE) {
                if (reservations.get(relativeEchoOffset)) {
                    LOG.warn("Echo manually reused before timed out: 0x{}", Integer.toHexString(echo));
                } else if (relativeEchoOffset == currentEchoOffset) {
                    moveEcho();
                } else {
                    reservations.set(relativeEchoOffset);
                    reservationCount++;
                }
            }
        }

        public boolean clearReservation(int echo) {
            if (inRange(echo) && reservations.get(echo - baseOffset)) {
                reservations.clear(echo - baseOffset);
                reservationCount--;
                return true;
            }
            return false;
        }

        public void setReservation(int echo) {
            if (inRange(echo)) {
                if (!reservations.get(echo - baseOffset)) {
                    reservationCount++;
                }
                reservations.set(echo - baseOffset);
            }
        }

        public boolean isReserved(int echo) {
            return inRange(echo) && reservations.get(echo - baseOffset);
        }

        public boolean timeout(int echo) {
            timeoutCount++;
            return timeoutCount >= BLOCK_MAX_TIMEOUTS_BEFORE_CHANGE;
        }
    }
}
