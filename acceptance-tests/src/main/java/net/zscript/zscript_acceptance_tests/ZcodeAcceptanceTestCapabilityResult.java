package net.zscript.zscript_acceptance_tests;

public class ZcodeAcceptanceTestCapabilityResult {
    private static byte[]  supportedCodes       = null;
    private static int     generalCapabilities  = -1;
    private static int     maxBigFieldLength    = -1;
    private static int     maxFieldNum          = -1;
    private static int     maxFieldSize         = -1;
    private static int     fieldSizeUnit        = 1;
    private static int     maxPageNum           = -1;
    private static long    persistentMemorySize = -1;
    private static boolean hasBeenRead          = false;

    public static void addSupportedCodes(byte[] supportedCodes) {
        if (ZcodeAcceptanceTestCapabilityResult.supportedCodes == null) {
            ZcodeAcceptanceTestCapabilityResult.supportedCodes = supportedCodes;
        } else {
            byte[] tmp = ZcodeAcceptanceTestCapabilityResult.supportedCodes;
            ZcodeAcceptanceTestCapabilityResult.supportedCodes = new byte[tmp.length + supportedCodes.length];
            System.arraycopy(tmp, 0, ZcodeAcceptanceTestCapabilityResult.supportedCodes, 0, tmp.length);
            System.arraycopy(supportedCodes, 0, ZcodeAcceptanceTestCapabilityResult.supportedCodes, tmp.length, supportedCodes.length);
        }
    }

    public static void setGeneralCapabilities(int generalCapabilities) {
        ZcodeAcceptanceTestCapabilityResult.generalCapabilities = generalCapabilities;
    }

    public static void setMaxBigFieldLength(int maxBigFieldLength) {
        ZcodeAcceptanceTestCapabilityResult.maxBigFieldLength = maxBigFieldLength;
    }

    public static void setMaxFieldNum(int maxFieldNum) {
        ZcodeAcceptanceTestCapabilityResult.maxFieldNum = maxFieldNum;
    }

    public static void setMaxFieldSize(int maxFieldSize) {
        ZcodeAcceptanceTestCapabilityResult.maxFieldSize = maxFieldSize;
    }

    public static void setFieldSizeUnit(int fieldSizeUnit) {
        ZcodeAcceptanceTestCapabilityResult.fieldSizeUnit = fieldSizeUnit;
    }

    public static void setMaxPageNum(int maxPageNum) {
        ZcodeAcceptanceTestCapabilityResult.maxPageNum = maxPageNum;
    }

    public static void setPersistentMemorySize(long persistentMemorySize) {
        ZcodeAcceptanceTestCapabilityResult.persistentMemorySize = persistentMemorySize;
    }

    public static void setHasBeenRead() {
        ZcodeAcceptanceTestCapabilityResult.hasBeenRead = true;
    }

    public static byte[] getSupportedCodes() {
        return supportedCodes;
    }

    public static int getGeneralCapabilities() {
        return generalCapabilities;
    }

    public static int getMaxBigFieldLength() {
        return maxBigFieldLength;
    }

    public static int getMaxFieldNum() {
        return maxFieldNum;
    }

    public static int getMaxFieldSize() {
        return maxFieldSize;
    }

    public static int getFieldSizeUnit() {
        return fieldSizeUnit;
    }

    public static int getMaxPageNum() {
        return maxPageNum;
    }

    public static long getPersistentMemorySize() {
        return persistentMemorySize;
    }

    public static boolean hasBeenRead() {
        return hasBeenRead;
    }

}
