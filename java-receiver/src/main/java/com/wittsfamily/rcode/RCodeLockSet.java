package com.wittsfamily.rcode;

public class RCodeLockSet {
    private byte[] locks;
    private byte[] w_nr;
    private int lockNum;

    public RCodeLockSet(RCodeParameters params) {
        locks = new byte[params.lockNum];
        w_nr = new byte[(params.lockNum + 1) / 8];
        lockNum = 0;
    }

    public void addLock(byte lock, boolean isWrite) {
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
