package net.zscript.javaclient.responseParser;

import java.util.Arrays;

public class ZscriptAddress {
    private final int[] addr;

    public static ZscriptAddress from(int addr0) {
        return new ZscriptAddress(new int[] { addr0 });
    }

    public static ZscriptAddress from(int addr0, int addr1) {
        return new ZscriptAddress(new int[] { addr0, addr1 });
    }

    public static ZscriptAddress from(int addr0, int addr1, int addr2) {
        return new ZscriptAddress(new int[] { addr0, addr1, addr2 });
    }

    public ZscriptAddress(int[] addr) {
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
        ZscriptAddress other = (ZscriptAddress) obj;
        if (!Arrays.equals(addr, other.addr))
            return false;
        return true;
    }

}
