package com.wittsfamily.rcode.javareceiver;

public class RCodeLocks {
    private final RCodeParameters params;
    private final byte[] locks;
    private final byte[] rwLocks;

    public RCodeLocks(RCodeParameters params) {
        this.params = params;
        locks = new byte[(params.highestBasicLockNum + 7) / 8];
        rwLocks = new byte[params.highestRwLockNum - params.lowestRwLockNum];
    }

    public boolean canLock(RCodeLockSet l) {
        for (int i = 0; i < l.getLockNum(); i++) {
            if (l.getLocks()[i] < params.highestBasicLockNum) {
                if ((locks[i / 8] & 1 << i % 8) != 0) {
                    return false;
                }
            } else if (l.getLocks()[i] < params.highestRwLockNum && l.getLocks()[i] >= params.lowestRwLockNum) {
                if ((l.getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    if (rwLocks[l.getLocks()[i] - params.lowestRwLockNum] != 0) {
                        return false;
                    }
                } else {
                    if (rwLocks[l.getLocks()[i] - params.lowestRwLockNum] == (byte) 255) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    public void lock(RCodeLockSet l) {
        for (int i = 0; i < l.getLockNum(); i++) {
            if (l.getLocks()[i] < params.highestBasicLockNum) {
                locks[i / 8] |= 1 << i % 8;
            } else if (l.getLocks()[i] < params.highestRwLockNum && l.getLocks()[i] >= params.lowestRwLockNum) {
                if ((l.getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    rwLocks[l.getLocks()[i] - params.lowestRwLockNum] = (byte) 255;
                } else {
                    rwLocks[l.getLocks()[i] - params.lowestRwLockNum]++;
                }
            }
        }
    }

    public void unlock(RCodeLockSet l) {
        for (int i = 0; i < l.getLockNum(); i++) {
            if (l.getLocks()[i] < params.highestBasicLockNum) {
                locks[i / 8] &= ~(1 << i % 8);
            } else if (l.getLocks()[i] < params.highestRwLockNum && l.getLocks()[i] >= params.lowestRwLockNum) {
                if ((l.getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    rwLocks[l.getLocks()[i] - params.lowestRwLockNum] = (byte) 0;
                } else {
                    rwLocks[l.getLocks()[i] - params.lowestRwLockNum]--;
                }
            }
        }
    }
}
