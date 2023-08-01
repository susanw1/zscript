package net.zscript.zscript_acceptance_tests;

public class AcceptanceTestCapabilityResult {
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
        if (AcceptanceTestCapabilityResult.supportedCodes == null) {
            AcceptanceTestCapabilityResult.supportedCodes = supportedCodes;
        } else {
            byte[] tmp = AcceptanceTestCapabilityResult.supportedCodes;
            AcceptanceTestCapabilityResult.supportedCodes = new byte[tmp.length + supportedCodes.length];
            System.arraycopy(tmp, 0, AcceptanceTestCapabilityResult.supportedCodes, 0, tmp.length);
            System.arraycopy(supportedCodes, 0, AcceptanceTestCapabilityResult.supportedCodes, tmp.length, supportedCodes.length);
        }
    }

    public static void setGeneralCapabilities(int generalCapabilities) {
        AcceptanceTestCapabilityResult.generalCapabilities = generalCapabilities;
    }

    public static void setMaxBigFieldLength(int maxBigFieldLength) {
        AcceptanceTestCapabilityResult.maxBigFieldLength = maxBigFieldLength;
    }

    public static void setMaxFieldNum(int maxFieldNum) {
        AcceptanceTestCapabilityResult.maxFieldNum = maxFieldNum;
    }

    public static void setMaxFieldSize(int maxFieldSize) {
        AcceptanceTestCapabilityResult.maxFieldSize = maxFieldSize;
    }

    public static void setFieldSizeUnit(int fieldSizeUnit) {
        AcceptanceTestCapabilityResult.fieldSizeUnit = fieldSizeUnit;
    }

    public static void setMaxPageNum(int maxPageNum) {
        AcceptanceTestCapabilityResult.maxPageNum = maxPageNum;
    }

    public static void setPersistentMemorySize(long persistentMemorySize) {
        AcceptanceTestCapabilityResult.persistentMemorySize = persistentMemorySize;
    }

    public static void setHasBeenRead() {
        AcceptanceTestCapabilityResult.hasBeenRead = true;
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
