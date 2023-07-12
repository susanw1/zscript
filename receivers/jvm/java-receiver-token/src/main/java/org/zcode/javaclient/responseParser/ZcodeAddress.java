package org.zcode.javaclient.responseParser;

import java.util.Arrays;

public class ZcodeAddress {
    private final int[] addr;

    public static ZcodeAddress from(int addr0) {
        return new ZcodeAddress(new int[] { addr0 });
    }

    public static ZcodeAddress from(int addr0, int addr1) {
        return new ZcodeAddress(new int[] { addr0, addr1 });
    }

    public static ZcodeAddress from(int addr0, int addr1, int addr2) {
        return new ZcodeAddress(new int[] { addr0, addr1, addr2 });
    }

    public ZcodeAddress(int[] addr) {
        this.addr = addr;
    }

    public int[] getAddr() {
        return addr;
    }

    @Override
    public int hashCode() {
        final int prime  = 31;
        int       result = 1;
        result = prime * result + Arrays.hashCode(addr);
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        ZcodeAddress other = (ZcodeAddress) obj;
        if (!Arrays.equals(addr, other.addr))
            return false;
        return true;
    }

}
