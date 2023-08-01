package net.zscript.javareceiver;

public class ZcodeLockSet {
    private final byte[] locks;
    private final byte[] w_nr;
    private int          lockNum;

    public ZcodeLockSet(final ZcodeParameters params) {
        locks = new byte[params.lockNum];
        w_nr = new byte[(params.lockNum + 1) / 8];
        lockNum = 0;
    }

    public void addLock(final byte lock, final boolean isWrite) {
        for (int i = 0; i < lockNum; i++) {
            if (locks[i] == lock) {
                if (isWrite) {
                    w_nr[i / 8] |= 1 << i % 8;
                }
                break;
            }
        }
        locks[lockNum] = lock;
        if (isWrite) {
            w_nr[lockNum / 8] |= 1 << lockNum % 8;
        }
        lockNum++;
    }

    public byte[] getLocks() {
        return locks;
    }

    public int getLockNum() {
        return lockNum;
    }

    public byte[] getW_nr() {
        return w_nr;
    }
}
