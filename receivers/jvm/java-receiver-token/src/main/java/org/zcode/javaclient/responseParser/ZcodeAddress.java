package org.zcode.javaclient.responseParser;

import java.util.Arrays;

public class ZcodeAddress {
    private final int[] addr;

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
