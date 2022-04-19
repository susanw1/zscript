package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.ZcodeParameters;

public class ZcodeInterruptVectorMap {
    private final byte[] addresses;
    private final byte[] specificities;
    private final byte[] buses;
    private final byte[] types;
    private final int[]  vectors;
    private int          vectorNum  = 0;
    private int          mostRecent = 0;

    public ZcodeInterruptVectorMap(final ZcodeParameters params) {
        addresses = new byte[params.interruptVectorNum];
        specificities = new byte[params.interruptVectorNum];
        buses = new byte[params.interruptVectorNum];
        types = new byte[params.interruptVectorNum];
        vectors = new int[params.interruptVectorNum];
    }

    public boolean setDefaultVector(final int vector) {
        return setVector((byte) 0, (byte) 0, (byte) 0, (byte) 0, vector);
    }

    public boolean setVector(final byte type, final int vector) {
        return setVector(type, (byte) 0, (byte) 0, (byte) 0x01, vector);
    }

    public boolean setVector(final byte type, final byte bus, final byte addr, final boolean hasAddress, final int vector) {
        return setVector(type, bus, addr, (byte) (hasAddress ? 0x03 : 0x02), vector);
    }

    public boolean hasVector(final byte type, final byte bus, final byte addr, final boolean hasAddress) {
        return hasVector(type, bus, addr, (byte) (hasAddress ? 0x03 : 0x02));
    }

    public int getVector(final byte type, final byte bus, final byte addr, final boolean hasAddress) {
        return getVector(type, bus, addr, (byte) (hasAddress ? 0x03 : 0x02));
    }

    private boolean setVector(final byte type, final byte bus, final byte addr, final byte specificity, final int vector) {
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || types[i] == type && (specificity == 1 || buses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))) {
                vectors[i] = vector;
                return true;
            }
        }
        if (vectorNum >= vectors.length) {
            return false;
        }
        specificities[vectorNum] = specificity;
        addresses[vectorNum] = addr;
        buses[vectorNum] = bus;
        types[vectorNum] = type;
        vectors[vectorNum] = vector;
        vectorNum++;
        return true;
    }

    private boolean hasVector(final byte type, final byte bus, final byte addr, final byte specificity) {
        if (specificities[mostRecent] == specificity
                && (specificity == 0 || types[mostRecent] == type && (specificity == 1 || buses[mostRecent] == bus && (specificity == 0x02 || addresses[mostRecent] == addr)))) {
            return true;
        }
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || types[i] == type && (specificity == 1 || buses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))) {
                mostRecent = i;
                return true;
            }
        }
        return specificity == 0 ? false : hasVector(type, bus, addr, (byte) (specificity - 1));
    }

    private int getVector(final byte type, final byte bus, final byte addr, final byte specificity) {
        if (specificities[mostRecent] == specificity
                && (specificity == 0 || types[mostRecent] == type && (specificity == 1 || buses[mostRecent] == bus && (specificity == 0x02 || addresses[mostRecent] == addr)))) {
            return vectors[mostRecent];
        }
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || types[i] == type && (specificity == 1 || buses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))) {
                mostRecent = i;
                return vectors[i];
            }
        }
        return specificity == 0 ? 0 : getVector(type, bus, addr, (byte) (specificity - 1));
    }

}
