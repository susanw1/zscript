package net.zscript.javaclient.nodes;

import java.util.BitSet;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class EchoAssigner {
    private static final Logger LOG = LoggerFactory.getLogger(EchoAssigner.class);

    private final static int SEGMENT_TIMEOUTS_BEFORE_CHANGE = 0x0010;
    private final static int SEGMENT_MAX_WAITING            = 0x00C0;
    private final static int SEGMENT_SIZE                   = 0x0100;

    private final long minSegmentChangeTimeNanos;

    private BitSet previousMessages = new BitSet(0);
    private BitSet sentMessages     = new BitSet(SEGMENT_SIZE);

    private int previousSegmentOffset = SEGMENT_SIZE;
    private int currentSegmentOffset  = SEGMENT_SIZE;

    private int timeoutCount = 0;
    private int waitingCount = 0;
    private int currentEcho  = 0;

    private final long lastSegmentChangeTimeNanos = System.nanoTime();

    public EchoAssigner(long minSegmentChangeTimeNanos) {
        this.minSegmentChangeTimeNanos = minSegmentChangeTimeNanos;
    }

    public void moveEcho() {
        if (waitingCount >= SEGMENT_MAX_WAITING) {
            LOG.error("Too many messages waiting for response ({}). Reduce command rate or latency.", waitingCount);
        }
        if (sentMessages.get(currentEcho)) {
            throw new IllegalStateException("Current echo value invalid");
        }
        sentMessages.set(currentEcho);
        currentEcho = sentMessages.nextClearBit(currentEcho + 1);
        if (currentEcho == SEGMENT_SIZE) {
            currentEcho = sentMessages.nextClearBit(0);
            if (currentEcho == SEGMENT_SIZE) {
                throw new IllegalStateException("Ran out of echo values to assign");
            }
        }
        waitingCount++;
    }

    public void manualEchoUse(int echo) {
        int relativeEcho = echo - currentSegmentOffset;
        if (relativeEcho >= 0 && relativeEcho < SEGMENT_SIZE) {
            if (sentMessages.get(relativeEcho)) {
                LOG.warn("Echo manually reused when timed out: {}", Integer.toHexString(echo));
            } else if (relativeEcho == currentEcho) {
                moveEcho();
            } else {
                sentMessages.set(relativeEcho);
            }
        }
        int relativeEchoPrev = echo - previousSegmentOffset;
        if (relativeEchoPrev >= 0 && relativeEchoPrev < SEGMENT_SIZE) {
            if (previousMessages.get(relativeEcho)) {
                LOG.warn("Echo manually reused when timed out: {}", Integer.toHexString(echo));
            }
            previousMessages.set(relativeEcho);
        }
    }

    public int getEcho() {
        return currentEcho + currentSegmentOffset;
    }

    public boolean isWaiting(int echo) {
        int relativeEcho = echo - currentSegmentOffset;
        if (relativeEcho >= 0 && relativeEcho < SEGMENT_SIZE) {
            return sentMessages.get(echo);
        }
        int relativeEchoPrev = echo - previousSegmentOffset;
        if (relativeEchoPrev >= 0 && relativeEchoPrev < SEGMENT_SIZE) {
            return previousMessages.get(echo);
        }
        return false;
    }

    public void responseArrivedNormal(int echo) {
        int     relativeEcho   = echo - currentSegmentOffset;
        BitSet  messagesTarget = sentMessages;
        boolean count          = true;
        if (relativeEcho < 0 || relativeEcho >= SEGMENT_SIZE) {
            relativeEcho = echo - previousSegmentOffset;
            if (relativeEcho < 0 || relativeEcho >= SEGMENT_SIZE) {
                return;
            }
            messagesTarget = previousMessages;
            count = false;
        }
        if (messagesTarget.get(relativeEcho)) {
            messagesTarget.clear(relativeEcho);
            if (count) {
                waitingCount--;
            }
        }
    }

    public void timeout(int echo) {
        int relativeEcho = echo - currentSegmentOffset;
        if (relativeEcho < 0 || relativeEcho >= SEGMENT_SIZE) {
            // if in previous, no action required
            return;
        }
        // if not a current message, no action needed
        if (sentMessages.get(relativeEcho)) {
            timeoutCount++;
            if (timeoutCount >= SEGMENT_TIMEOUTS_BEFORE_CHANGE) {
                long time = System.nanoTime();
                if (time - lastSegmentChangeTimeNanos < minSegmentChangeTimeNanos) {
                    LOG.error("Connection timing out too much.");
                } else {
                    LOG.info("Lingering timeout count: ({}). Changing echo value segment.", timeoutCount);
                }
                timeoutCount = 0;
                waitingCount = 0;
                currentEcho = 0;
                previousMessages = sentMessages;
                sentMessages = new BitSet(SEGMENT_SIZE);
                previousSegmentOffset = currentSegmentOffset;
                currentSegmentOffset += SEGMENT_SIZE;
                if (currentSegmentOffset + SEGMENT_SIZE > 0x10000) {
                    currentSegmentOffset = SEGMENT_SIZE; // Skip the first segment, to leave space for manual echo
                }
            }
        }
    }

    public boolean unmatchedReceive(int echo) {
        int     relativeEcho   = echo - currentSegmentOffset;
        BitSet  messagesTarget = sentMessages;
        boolean count          = true;
        if (relativeEcho < 0 || relativeEcho >= SEGMENT_SIZE) {
            relativeEcho = echo - previousSegmentOffset;
            if (relativeEcho < 0 || relativeEcho >= SEGMENT_SIZE) {
                // go to the unmatched handler, as message is very old (or not one we're keeping track of)
                return false;
            }
            messagesTarget = previousMessages;
            count = false;
        }
        if (messagesTarget.get(relativeEcho)) {
            messagesTarget.clear(relativeEcho);
            if (count) {
                timeoutCount--;
            }
            return true;
        } else {
            // goes to the unmatched handler
            return false;
        }
    }

}
