package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.RCodeParameters;

public class RCodeInterruptVectorMap {
    private final byte[] addresses;
    private final byte[] specificities;
    private final byte[] busses;
    private final byte[] types;
    private final int[] vectors;
    private int vectorNum = 0;
    private int mostRecent = 0;

    public RCodeInterruptVectorMap(RCodeParameters params) {
        addresses = new byte[params.interruptVectorNum];
        specificities = new byte[params.interruptVectorNum];
        busses = new byte[params.interruptVectorNum];
        types = new byte[params.interruptVectorNum];
        vectors = new int[params.interruptVectorNum];
    }

    public boolean setDefaultVector(int vector) {
        return setVector((byte) 0, (byte) 0, (byte) 0, (byte) 0, vector);
    }

    public boolean setVector(byte type, int vector) {
        return setVector(type, (byte) 0, (byte) 0, (byte) 0x01, vector);
    }

    public boolean setVector(byte type, byte bus, byte addr, boolean hasAddress, int vector) {
        return setVector(type, bus, addr, (byte) (hasAddress ? 0x03 : 0x02), vector);
    }

    public boolean hasVector(byte type, byte bus, byte addr, boolean hasAddress) {
        return hasVector(type, bus, addr, (byte) (hasAddress ? 0x03 : 0x02));
    }

    public int getVector(byte type, byte bus, byte addr, boolean hasAddress) {
        return getVector(type, bus, addr, (byte) (hasAddress ? 0x03 : 0x02));
    }

    private boolean setVector(byte type, byte bus, byte addr, byte specificity, int vector) {
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || types[i] == type && (specificity == 1 || busses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))) {
                vectors[i] = vector;
                return true;
            }
        }
        if (vectorNum >= vectors.length) {
            return false;
        }
        specificities[vectorNum] = specificity;
        addresses[vectorNum] = addr;
        busses[vectorNum] = bus;
        types[vectorNum] = type;
        vectors[vectorNum] = vector;
        vectorNum++;
        return true;
    }

    private boolean hasVector(byte type, byte bus, byte addr, byte specificity) {
        if (specificities[mostRecent] == specificity
                && (specificity == 0 || types[mostRecent] == type && (specificity == 1 || busses[mostRecent] == bus && (specificity == 0x02 || addresses[mostRecent] == addr)))) {
            return true;
        }
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || types[i] == type && (specificity == 1 || busses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))) {
                mostRecent = i;
                return true;
            }
        }
        return specificity == 0 ? false : hasVector(type, bus, addr, (byte) (specificity - 1));
    }

    private int getVector(byte type, byte bus, byte addr, byte specificity) {
        if (specificities[mostRecent] == specificity
                && (specificity == 0 || types[mostRecent] == type && (specificity == 1 || busses[mostRecent] == bus && (specificity == 0x02 || addresses[mostRecent] == addr)))) {
            return vectors[mostRecent];
        }
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || types[i] == type && (specificity == 1 || busses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))) {
                mostRecent = i;
                return vectors[i];
            }
        }
        return specificity == 0 ? 0 : getVector(type, bus, addr, (byte) (specificity - 1));
    }

}
